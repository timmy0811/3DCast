#include "castpch.h"
#include "Serializer.h"

#include "3DCast/Scene/Entity.h"
#include "3DCast/Scene/Component/Component.h"
#include "3DCast/Scene/Serialization/Encoding.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>

#include "entt/entt.hpp"

namespace Cast::Serialization
{
    SceneSerializer::SceneSerializer(Scene* scene)
        : CurrentScene(scene)
    {
    }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";

        // Todo: Fix prerender popup
        // PopupID = GUI::TempGuiElementCollection::AddElement(new Cast::GUI::NotificationModal("Saving Scene", "Saving scene to file", ICON_FA_FLOPPY_DISK, false, false));

        // Skybox
        out << YAML::Key << "Skybox";
        out << YAML::BeginMap; // Skybox

        out << YAML::Key << "Mode" << YAML::Value << (int)CallbackData.Skybox->Mode;

        switch (CallbackData.Skybox->Mode)
        {
        case Renderer::Skybox::RenderMode::ClearColor:
            out << YAML::Key << "ClearColor" << YAML::Value << CallbackData.Skybox->ClearColor;
            break;

        case Renderer::Skybox::RenderMode::Cubemap:
            out << YAML::Key << "ActiveCubemapName" << YAML::Value << CallbackData.Skybox->ActiveCubemapName;
            out << YAML::Key << "ActiveCubemapDirPath" << YAML::Value << CallbackData.Skybox->ActiveCubemap->GetPath();
            out << YAML::Key << "ActiveCubemapFileFormat" << YAML::Value << CallbackData.Skybox->ActiveCubemap->GetFileFormat();
            break;

        default:
            break;
        }

        out << YAML::Key << "UseEnvironmentLighting" << YAML::Value << *CallbackData.UseEnvironmentLighting;
        out << YAML::EndMap; // Skybox

        // Material Registry
        out << YAML::Key << "MaterialRegistry";
        out << YAML::BeginMap; // MaterialRegistry
        out << YAML::Key << "Entries" << YAML::Value << YAML::BeginSeq;

        // Serialize all materials
        auto& materials = MaterialCacheRegistryInstance.Materials;
        auto& proxyIds = MaterialCacheRegistryInstance.ProxyIds;

        for (const auto& [materialId, material] : materials)
        {
            if (material.isSystemMaterial)
                continue;

            out << YAML::BeginMap;
            out << YAML::Key << "Material" << YAML::Value << materialId;

            // Serialize shader object properties
            out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "DiffuseColor" << YAML::Value << material.shaderObject.diffuseColor;
            out << YAML::Key << "SpecularColor" << YAML::Value << material.shaderObject.specularColor;
            out << YAML::Key << "EmissiveColor" << YAML::Value << material.shaderObject.emissiveColor;
            out << YAML::Key << "Metallic" << YAML::Value << material.shaderObject.metallic;
            out << YAML::Key << "Roughness" << YAML::Value << material.shaderObject.roughness;
            out << YAML::Key << "Shininess" << YAML::Value << material.shaderObject.shininess;
            out << YAML::Key << "Reflectance" << YAML::Value << material.shaderObject.reflectance;
            out << YAML::EndMap; // Properties

            // Find all proxy names associated with this material ID
            out << YAML::Key << "Proxies" << YAML::Value << YAML::BeginSeq;
            for (const auto& [proxyName, id] : proxyIds)
            {
                if (id == materialId)
                {
                    out << proxyName;
                }
            }
            out << YAML::EndSeq; // Proxies

            out << YAML::EndMap; // Material
        }

        out << YAML::EndSeq;
        out << YAML::EndMap; // MaterialRegistry

        // Entities
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        auto& entityMap = CurrentScene->EntityDescriptorPool;
        for (auto& [entityHandle, entity] : entityMap)
        {
            if (entity && !entity->IsChild())
            {
                SerializeEntity(out, entity);
            }
        }

        // Close Yaml Context
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::filesystem::path path(filepath);
        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);

        if (ec)
        {
            LOG_CORE_ERROR("Saving Scene failed. Cannot create directories for path '{0}': {1}", filepath, ec.message());
            return;
        }

        std::ofstream fout(filepath);
        if (!fout)
        {
            LOG_CORE_ERROR("Failed to open scene file for writing: {0}", filepath);
            return;
        }

        fout << out.c_str();
        LOG_CORE_INFO("Scene serialized to '{0}'", filepath);
    }

    void SceneSerializer::SerializeRuntime(const std::string& filepath)
    {
        // Runtime serialization - could be binary or have different format
        // For now, just use the same format
        Serialize(filepath);
    }

    void SceneSerializer::SerializeEntity(YAML::Emitter& out, const Ref<Entity>& entity)
    {
        if (entity->HasComponent<Component::MeshComponent>() && entity->GetComponent<Component::MeshComponent>().GetNodeType() != MeshNodeType::Root)
            return;

        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << static_cast<uint32_t>(*entity);

        // Tag Component
        if (entity->HasComponent<Component::TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent

            const auto& tagComp = entity->GetComponent<Component::TagComponent>();
            out << YAML::Key << "Tag" << YAML::Value << tagComp.Tag;
            out << YAML::Key << "Icon" << YAML::Value << tagComp.Icon;

            out << YAML::EndMap; // TagComponent
        }

        // Transform Component
        if (entity->HasComponent<Component::TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            const auto& tc = entity->GetComponent<Component::TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.scale;
            out << YAML::Key << "IsRegistered" << YAML::Value << tc.isRegistered;

            out << YAML::Key << "BBox" << YAML::Value;
            out << YAML::BeginMap; // BBox
            out << YAML::Key << "Center" << YAML::Value << tc.BBox.Center_;
            out << YAML::Key << "Radius" << YAML::Value << tc.BBox.Radius_;
            out << YAML::Key << "Min" << YAML::Value << tc.BBox.Min_;
            out << YAML::Key << "Max" << YAML::Value << tc.BBox.Max_;
            out << YAML::EndMap; // BBox

            out << YAML::EndMap; // TransformComponent
        }

        // Camera Component
        if (entity->HasComponent<Component::CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

            auto& cc = entity->GetComponent<Component::CameraComponent>();
            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "IsScenePrimary" << YAML::Value << (cc.Camera->GetID() == CallbackData.ActiveCamera->value()->GetID());

            out << YAML::Key << "Projection" << YAML::Value << (int)cc.Camera->ProjectionType;
            out << YAML::Key << "ProjectionMatrix" << YAML::Value << cc.Camera->ProjectionMat;
            out << YAML::Key << "Position" << YAML::Value << cc.Camera->Position;
            out << YAML::Key << "Rotation" << YAML::Value << cc.Camera->Rotation;
            out << YAML::Key << "WorldUp" << YAML::Value << cc.Camera->WorldUp;
            out << YAML::EndMap; // Camera

            out << YAML::EndMap; // CameraComponent
        }

        // Custom Mesh Component
        if (entity->HasComponent<Component::CustomMeshComponent>())
        {
            out << YAML::Key << "CustomMeshComponent";
            out << YAML::BeginMap; // CustomMeshComponent

            const auto& cmc = entity->GetComponent<Component::CustomMeshComponent>();
            out << YAML::Key << "IsIndexed" << YAML::Value << cmc.isIndexed;
            out << YAML::Key << "VertexDataSize" << YAML::Value << cmc.vertexDataSize;
            out << YAML::Key << "IndexDataSize" << YAML::Value << cmc.indexDataSize;

            if (cmc.vertexData && cmc.vertexDataSize > 0) {
                const std::string encodedVertexData = Base64Encode(cmc.vertexData, cmc.vertexDataSize);
                out << YAML::Key << "VertexData" << YAML::Value << encodedVertexData;
            }
            else {
                out << YAML::Key << "VertexData" << YAML::Value << "";
            }

            if (cmc.indexData && cmc.indexDataSize > 0) {
                const std::string encodedIndexData = Base64Encode(cmc.indexData, cmc.indexDataSize);
                out << YAML::Key << "IndexData" << YAML::Value << encodedIndexData;
            }
            else {
                out << YAML::Key << "IndexData" << YAML::Value << "";
            }

            out << YAML::EndMap; // CustomMeshComponent
        }

        // Light Component
        if (entity->HasComponent<Component::LightComponent>())
        {
            out << YAML::Key << "LightComponent";
            out << YAML::BeginMap; // LightComponent

            const auto& lc = entity->GetComponent<Component::LightComponent>();
            out << YAML::Key << "IsEnvironmentLight" << YAML::Value << lc.IsEnvironmentLight;
            out << YAML::Key << "Type" << YAML::Value << lc.LightType;

            switch (lc.LightType)
            {
                case Component::LightComponent::Type::Directional:
                {
                    const auto dirLight = reinterpret_cast<DirectionalLightShaderObject*>(lc.Light);
                    out << YAML::Key << "Direction" << YAML::Value << dirLight->direction;
                    out << YAML::Key << "Ambient" << YAML::Value << dirLight->ambient;
                    out << YAML::Key << "Diffuse" << YAML::Value << dirLight->diffuse;
                    out << YAML::Key << "Specular" << YAML::Value << dirLight->specular;
                    break;
                }
                case Component::LightComponent::Type::Point:
                {
                    const auto pointLight = reinterpret_cast<PointLightShaderObject*>(lc.Light);
                    out << YAML::Key << "Position" << YAML::Value << pointLight->position;
                    out << YAML::Key << "Ambient" << YAML::Value << pointLight->ambient;
                    out << YAML::Key << "Diffuse" << YAML::Value << pointLight->diffuse;
                    out << YAML::Key << "Specular" << YAML::Value << pointLight->specular;
                    out << YAML::Key << "Constant" << YAML::Value << pointLight->constant;
                    out << YAML::Key << "Linear" << YAML::Value << pointLight->linear;
                    out << YAML::Key << "Quadratic" << YAML::Value << pointLight->quadratic;
                    break;
                }
                case Component::LightComponent::Type::Spot:
                {
                    const auto spotLight = reinterpret_cast<SpotLightShaderObject*>(lc.Light);
                    out << YAML::Key << "Position" << YAML::Value << spotLight->position;
                    out << YAML::Key << "Direction" << YAML::Value << spotLight->direction;
                    out << YAML::Key << "Ambient" << YAML::Value << spotLight->ambient;
                    out << YAML::Key << "Diffuse" << YAML::Value << spotLight->diffuse;
                    out << YAML::Key << "Specular" << YAML::Value << spotLight->specular;
                    out << YAML::Key << "Constant" << YAML::Value << spotLight->constant;
                    out << YAML::Key << "Cutoff" << YAML::Value << spotLight->cutOff;
                    out << YAML::Key << "OuterCutoff" << YAML::Value << spotLight->outerCutOff;
                    break;
                }
            }

            out << YAML::EndMap; // LightComponent
        }

        // Material Component
        if (entity->HasComponent<Component::MaterialComponent>())
        {
            out << YAML::Key << "MaterialComponent";
            out << YAML::BeginMap; // MaterialComponent

            const auto& mc = entity->GetComponent<Component::MaterialComponent>();

            if (mc.isPrivateMaterialCreated && mc.isCustomMaterial)
            {
                out << YAML::Key << "PrivateMaterial" << YAML::Value;
                out << YAML::BeginMap; // PrivateMaterial
                out << YAML::Key << "DiffusePath" << YAML::Value << mc.diffuseFile;
                out << YAML::Key << "SpecularPath" << YAML::Value << mc.specularFile;

                out << YAML::Key << "DiffuseLoaded" << YAML::Value << mc.diffuseLoaded;
                out << YAML::Key << "SpecularLoaded" << YAML::Value << mc.specularLoaded;

                out << YAML::Key << "Diffuse" << YAML::Value << mc.privateMaterial.shaderObject.diffuseColor;
                out << YAML::Key << "Specular" << YAML::Value << mc.privateMaterial.shaderObject.specularColor;
                out << YAML::Key << "Emissive" << YAML::Value << mc.privateMaterial.shaderObject.emissiveColor;
                out << YAML::Key << "Metallic" << YAML::Value << mc.privateMaterial.shaderObject.metallic;

                out << YAML::Key << "Roughness" << YAML::Value << mc.privateMaterial.shaderObject.roughness;
                out << YAML::Key << "Shininess" << YAML::Value << mc.privateMaterial.shaderObject.shininess;
                out << YAML::Key << "Reflectance" << YAML::Value << mc.privateMaterial.shaderObject.reflectance;
                out << YAML::EndMap; // PrivateMaterial
            }

            out << YAML::Key << "ParallaxPath" << YAML::Value << mc.parallaxFile;
            out << YAML::Key << "NormalPath" << YAML::Value << mc.normalFile;

            out << YAML::Key << "ParallaxLoaded" << YAML::Value << mc.parallaxLoaded;
            out << YAML::Key << "NormalLoaded" << YAML::Value << mc.normalLoaded;

            out << YAML::Key << "IsCustomMaterial" << YAML::Value << mc.isCustomMaterial;
            out << YAML::Key << "SelectedMaterialProxy" << YAML::Value << mc.selectedItem;

            out << YAML::EndMap; // MaterialComponent
        }

        // Mesh Component
        if (entity->HasComponent<Component::MeshComponent>())
        {
            const auto& mc = entity->GetComponent<Component::MeshComponent>();
            if (mc.TypeNode == MeshNodeType::Root)
            {
                out << YAML::Key << "MeshComponent";
                out << YAML::BeginMap; // MeshComponent

                out << YAML::Key << "ModelPath" << YAML::Value << mc.Path;
                out << YAML::Key << "Filename" << YAML::Value << mc.Filename;

                out << YAML::EndMap; // MeshComponent
            }
        }

        // PBRMaterial Component
        if (entity->HasComponent<Component::PBRMaterialComponent>())
        {
            out << YAML::Key << "PBRMaterialComponent";
        }

        // Rasterizable Component
        if (entity->HasComponent<Component::RasterizableComponent>())
        {
            out << YAML::Key << "RasterizableComponent";
            out << YAML::BeginMap; // RasterizableComponent

            const auto& rc = entity->GetComponent<Component::RasterizableComponent>();
            out << YAML::Key << "Renderable" << YAML::Value << rc.Renderable;

            out << YAML::EndMap; // RasterizableComponent
        }

        // PBR Component
        if (entity->HasComponent<Component::PBRComponent>())
        {
            out << YAML::Key << "PBRComponent";
            out << YAML::BeginMap; // PBRComponent

            const auto& pc = entity->GetComponent<Component::PBRComponent>();
            out << YAML::Key << "Renderable" << YAML::Value << pc.Renderable;

            out << YAML::EndMap; // PBRComponent
        }

        // Shader Component
        if (entity->HasComponent<Component::ShaderComponent>())
        {
            out << YAML::Key << "ShaderComponent";
            out << YAML::BeginMap; // ShaderComponent

            const auto& sc = entity->GetComponent<Component::ShaderComponent>();
            out << YAML::Key << "Proxy" << YAML::Value << sc.Identifier;

            out << YAML::EndMap; // ShaderComponent
        }

        // Serialize children entities recursively
        if (entity->HasChildren())
        {
            out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
            for (const auto& child : entity->GetChildren())
            {
                SerializeEntity(out, child);
            }
            out << YAML::EndSeq;
        }

        out << YAML::EndMap;
    }

    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            LOG_CORE_ERROR("Failed to open scene file: {0}", filepath);
            return false;
        }

        std::stringstream ss;
        ss << file.rdbuf();

        YAML::Node data;
        try
        {
            data = YAML::Load(ss.str());
        }
        catch (YAML::ParserException& e)
        {
            LOG_CORE_ERROR("Failed to parse scene file: {0}", e.what());
            return false;
        }

        if (!data["Scene"])
        {
            LOG_CORE_ERROR("Invalid scene file: missing Scene section");
            return false;
        }

        PrepareNewSerialization();

        // Deserialize Materials first so they can be referenced by entities
        if (auto materialRegistryNode = data["MaterialRegistry"])
        {
            if (auto materialsNode = materialRegistryNode["Entries"])
            {
                for (auto materialNode : materialsNode)
                {
                    // Create material with the original ID
                    auto mtId = materialNode["Material"].as<unsigned int>();
                    auto materialId = UID(mtId);

                    Material material;
                    material.id = materialId;

                    // Deserialize properties
                    if (auto propertiesNode = materialNode["Properties"])
                    {
                        material.shaderObject.diffuseColor = propertiesNode["DiffuseColor"].as<glm::vec3>();
                        material.shaderObject.specularColor = propertiesNode["SpecularColor"].as<glm::vec3>();
                        material.shaderObject.emissiveColor = propertiesNode["EmissiveColor"].as<glm::vec3>();
                        material.shaderObject.metallic = propertiesNode["Metallic"].as<float>();
                        material.shaderObject.roughness = propertiesNode["Roughness"].as<float>();
                        material.shaderObject.shininess = propertiesNode["Shininess"].as<float>();
                        material.shaderObject.reflectance = propertiesNode["Reflectance"].as<float>();
                    }

                    // Add material to registry
                    MaterialCacheRegistryInstance.Materials[materialId] = material;

                    // Register proxy names
                    if (auto proxiesNode = materialNode["Proxies"])
                    {
                        for (auto proxyNode : proxiesNode)
                        {
                            auto proxyName = proxyNode.as<std::string>();
                            MaterialCacheRegistryInstance.AddProxy(materialId, proxyName);
                        }
                    }
                }
            }
        }

        if (auto entitiesNode = data["Entities"])
        {
            for (auto entityNode : entitiesNode)
            {
                std::string name = "Untagged";
                if (auto tagComponent = entityNode["TagComponent"])
                {
                    if (tagComponent["Tag"])
                        name = tagComponent["Tag"].as<std::string>();
                }

                auto entity = CurrentScene->CreateEntity(name);
                DeserializeEntityRecursive(entityNode, entity);
            }
        }

        if (!NewActiveCameraSet)
            LOG_CORE_ERROR("A valid scene must define an active scene camera.");

        // Parse Skybox
        if (auto skyboxNode = data["Skybox"])
        {
            CallbackData.Skybox->Mode = (Renderer::Skybox::RenderMode)skyboxNode["Mode"].as<int>();

            switch (CallbackData.Skybox->Mode)
            {
            case Renderer::Skybox::RenderMode::ClearColor:
                {
                    CallbackData.Skybox->ClearColor = skyboxNode["ClearColor"].as<glm::vec4>();
                    break;
                }
            case Renderer::Skybox::RenderMode::Cubemap:
                {
                    auto name = skyboxNode["ActiveCubemapName"].as<std::string>();
                    auto dirPath = skyboxNode["ActiveCubemapDirPath"].as<std::string>();
                    auto fileFormat = skyboxNode["ActiveCubemapFileFormat"].as<std::string>();

                    if (DeserializedEnvironmentLightEntity)
                    {
                        auto& comp = DeserializedEnvironmentLightEntity->GetComponent<Component::LightComponent>();
                        comp.IsEnvironmentLight = true;
                        *CallbackData.EnvironmentLightComponent = &comp;
                        *CallbackData.EnvironmentLightEntity = DeserializedEnvironmentLightEntity;
                    }

                    *CallbackData.RenderMode = skyboxNode["Mode"].as<int>();
                    CallbackData.Skybox->AddCubemap(name, dirPath, fileFormat);
                    CallbackData.Skybox->SetActiveCubemap(skyboxNode["ActiveCubemapName"].as<std::string>());
                    CallbackData.Skybox->CalculateEnvironmentLightForCurrentCubemap();
                    break;
                }
            default:
                break;
            }

            *CallbackData.UseEnvironmentLighting = skyboxNode["UseEnvironmentLighting"].as<bool>();
        }

        LOG_CORE_INFO("Scene deserialized from '{0}'", filepath);
        return true;
    }

    void SceneSerializer::DeserializeEntityRecursive(const YAML::Node& entityNode, const Ref<Entity>& entity)
    {
        // Deserialize TagComponent Icon
        if (auto tagComponent = entityNode["TagComponent"])
        {
            auto& tc = entity->GetComponent<Component::TagComponent>();
            if (tagComponent["Icon"])
                tc.Icon = tagComponent["Icon"].as<std::string>();
        }

        // Deserialize TransformComponent
        if (auto transformComponent = entityNode["TransformComponent"])
        {
            auto& tc = entity->GetComponent<Component::TransformComponent>();
            tc.translation = transformComponent["Translation"].as<glm::vec3>();
            tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
            tc.scale = transformComponent["Scale"].as<glm::vec3>();
            const bool registerTF = transformComponent["IsRegistered"].as<bool>();

            tc.UpdateTransformMatrix();

            auto bboxNode = transformComponent["BBox"];
            tc.BBox.Center_ = bboxNode["Center"].as<glm::vec3>();
            tc.BBox.Radius_ = bboxNode["Radius"].as<float>();
            tc.BBox.Min_ = bboxNode["Min"].as<glm::vec3>();
            tc.BBox.Max_ = bboxNode["Max"].as<glm::vec3>();

            if (registerTF)
            {
                CurrentScene->RegisterTransformComponent(entity);
            }
        }

        // Deserialize CameraComponent
        if (auto cameraComponent = entityNode["CameraComponent"])
        {
            auto cameraNode = cameraComponent["Camera"];
            auto cameraType = (Renderer::Camera::Type)cameraNode["Projection"].as<int>();
            auto& cc = entity->AddComponents<Component::CameraComponent>(cameraType);

            cc.Camera->ProjectionType = cameraType;
            cc.Camera->ProjectionMat = cameraNode["ProjectionMatrix"].as<glm::mat4>();
            cc.Camera->Position = cameraNode["Position"].as<glm::vec3>();
            cc.Camera->Rotation = cameraNode["Rotation"].as<glm::vec3>();
            cc.Camera->WorldUp = cameraNode["WorldUp"].as<glm::vec3>();

            memset(cc.Camera->HasChangedField, true, sizeof(cc.Camera->HasChangedField));
            cc.Camera->UpdateViewMat();

            if (cameraNode["IsScenePrimary"].as<bool>())
            {
                CallbackData.ActiveCamera->emplace(cc.Camera);
                NewActiveCameraSet = true;
            }
        }

        // Deserialize MaterialComponent
        if (auto materialComponent = entityNode["MaterialComponent"])
        {
            auto selectedMaterial = materialComponent["SelectedMaterialProxy"].as<std::string>();
            auto& mc = entity->AddComponents<Component::MaterialComponent>(selectedMaterial);

            mc.parallaxFile = materialComponent["ParallaxPath"].as<std::string>();
            mc.normalFile = materialComponent["NormalPath"].as<std::string>();

            auto privateMaterial = materialComponent["PrivateMaterial"];
            if (privateMaterial && materialComponent["IsCustomMaterial"].as<bool>())
            {
                mc.isCustomMaterial = true;

                if (!mc.isPrivateMaterialCreated)
                {
                    mc.privateMaterial = MaterialCacheRegistryInstance.Get(MaterialCacheRegistryInstance.Create());
                    DeferredSamplerStoreInstance.AddCustomMaterial(mc.privateMaterial);
                    mc.isPrivateMaterialCreated = true;
                }

                mc.diffuseFile = privateMaterial["DiffusePath"].as<std::string>();
                mc.specularFile = privateMaterial["SpecularPath"].as<std::string>();

                if (privateMaterial["DiffuseLoaded"].as<bool>() && !mc.diffuseFile.empty())
                    mc.LoadDiffuseTexture(mc.diffuseFile);

                if (privateMaterial["SpecularLoaded"].as<bool>() && !mc.specularFile.empty())
                    mc.LoadSpecularTexture(mc.specularFile);

                mc.privateMaterial.shaderObject.diffuseColor = privateMaterial["Diffuse"].as<glm::vec3>();
                mc.privateMaterial.shaderObject.specularColor = privateMaterial["Specular"].as<glm::vec3>();
                mc.privateMaterial.shaderObject.emissiveColor = privateMaterial["Emissive"].as<glm::vec3>();

                mc.privateMaterial.shaderObject.metallic = privateMaterial["Metallic"].as<float>();
                mc.privateMaterial.shaderObject.roughness = privateMaterial["Roughness"].as<float>();
                mc.privateMaterial.shaderObject.shininess = privateMaterial["Shininess"].as<float>();
                mc.privateMaterial.shaderObject.reflectance = privateMaterial["Reflectance"].as<float>();

                mc.currentMaterial = mc.privateMaterial;
                mc.currentMaterialInfo = DeferredSamplerStoreInstance.GetCustomMaterialStoreId(mc.currentMaterial.id);
                mc.UpdateSamplerMapping();

                MaterialCacheRegistryInstance.Edit(mc.privateMaterial.id, mc.privateMaterial);
                DeferredSamplerStoreInstance.EditCustomMaterial(mc.currentMaterialInfo, mc.privateMaterial);
            }

            if (materialComponent["ParallaxLoaded"].as<bool>() && !mc.parallaxFile.empty())
                mc.LoadParallaxTexture(mc.parallaxFile);

            if (materialComponent["NormalLoaded"].as<bool>() && !mc.normalFile.empty())
                mc.LoadNormalTexture(mc.normalFile);
        }

        // Deserialize CustomMeshComponent
        if (auto customMeshComponent = entityNode["CustomMeshComponent"])
        {
            bool isIndexed = customMeshComponent["IsIndexed"].as<bool>();
            auto vertexDataSize = customMeshComponent["VertexDataSize"].as<size_t>();
            auto indexDataSize = customMeshComponent["IndexDataSize"].as<size_t>();

            if (!entity->HasComponent<Component::TransformComponent>())
            {
                LOG_CORE_ERROR("In order to add CustomMeshData, a registered TransformComponent is required.");
                return;
            }

            auto& tc = entity->GetComponent<Component::TransformComponent>();
            if (!tc.isRegistered)
            {
                CurrentScene->RegisterTransformComponent(entity);
                LOG_CORE_INFO("Registered transform component during CustomMeshComponent deserialization.");
            }

            auto& mc = entity->AddComponents<Component::CustomMeshComponent>();
            int samplerIndex = entity->HasComponent<Component::MaterialComponent>() ?
                               entity->GetComponent<Component::MaterialComponent>().samplerIndex : 0;

            if (vertexDataSize > 0)
            {
                auto encodedVertexData = customMeshComponent["VertexData"].as<std::string>();
                if (!encodedVertexData.empty()) {
                    mc.SetVertexBuffer((float*)Base64Decode(encodedVertexData, vertexDataSize), vertexDataSize, true);
                    mc.PatchRegistryData(samplerIndex, tc.transformRegistryIndex);
                }
            }

            if (isIndexed && indexDataSize > 0)
            {
                auto encodedIndexData = customMeshComponent["IndexData"].as<std::string>();
                if (!encodedIndexData.empty()) {
                    mc.SetIndexBuffer((unsigned int*)Base64Decode(encodedIndexData, indexDataSize), indexDataSize, true);
                }
            }

            mc.AddToBatchMemory();
        }

        // Deserialize LightComponent
        if (auto lightComponent = entityNode["LightComponent"])
        {
            const bool isEnvironmentLight = lightComponent["IsEnvironmentLight"].as<bool>();
            auto type = (Component::LightComponent::Type)lightComponent["Type"].as<int>();

            auto& lc = entity->AddComponents<Component::LightComponent>(type, CurrentScene);
            lc.IsEnvironmentLight = isEnvironmentLight;

            if (isEnvironmentLight)
                DeserializedEnvironmentLightEntity = entity.get();

            switch (type)
            {
            case Component::LightComponent::Type::Directional:
            {
                auto dirLight = reinterpret_cast<DirectionalLightShaderObject*>(lc.Light);
                dirLight->direction = lightComponent["Direction"].as<glm::vec3>();
                dirLight->ambient = lightComponent["Ambient"].as<glm::vec3>();
                dirLight->diffuse = lightComponent["Diffuse"].as<glm::vec3>();
                dirLight->specular = lightComponent["Specular"].as<glm::vec3>();
                break;
            }
            case Component::LightComponent::Type::Point:
            {
                auto pointLight = reinterpret_cast<PointLightShaderObject*>(lc.Light);
                pointLight->position = lightComponent["Position"].as<glm::vec3>();
                pointLight->ambient = lightComponent["Ambient"].as<glm::vec3>();
                pointLight->diffuse = lightComponent["Diffuse"].as<glm::vec3>();
                pointLight->specular = lightComponent["Specular"].as<glm::vec3>();
                pointLight->constant = lightComponent["Constant"].as<float>();
                pointLight->linear = lightComponent["Linear"].as<float>();
                pointLight->quadratic = lightComponent["Quadratic"].as<float>();
                break;
            }
            case Component::LightComponent::Type::Spot:
            {
                auto spotLight = reinterpret_cast<SpotLightShaderObject*>(lc.Light);
                spotLight->position = lightComponent["Position"].as<glm::vec3>();
                spotLight->direction = lightComponent["Direction"].as<glm::vec3>();
                spotLight->ambient = lightComponent["Ambient"].as<glm::vec3>();
                spotLight->diffuse = lightComponent["Diffuse"].as<glm::vec3>();
                spotLight->specular = lightComponent["Specular"].as<glm::vec3>();
                spotLight->constant = lightComponent["Constant"].as<float>();
                spotLight->cutOff = lightComponent["Cutoff"].as<float>();
                spotLight->outerCutOff = lightComponent["OuterCutoff"].as<float>();
                break;
            }
            }

            lc.UpdateLightData();
        }

        // Deserialize MeshComponent
        if (auto meshComponent = entityNode["MeshComponent"])
        {
            auto path = meshComponent["ModelPath"].as<std::string>();
            //std::string filename = meshComponent["Filename"].as<std::string>();
            // Use deferred loading (true) so the model loads after EntityNode is set
            auto& mc = entity->AddComponents<Component::MeshComponent>(path, true);
        }

        // Deserialize RasterizableComponent
        if (auto rasterizableComponent = entityNode["RasterizableComponent"])
        {
            auto& rc = entity->AddComponents<Component::RasterizableComponent>();
            rc.Renderable = rasterizableComponent["Renderable"].as<bool>();
        }

        // Deserialize PBRComponent
        if (auto pbrComponent = entityNode["PBRComponent"])
        {
            auto& pc = entity->AddComponents<Component::PBRComponent>();
            pc.Renderable = pbrComponent["Renderable"].as<bool>();
        }

        // Deserialize ShaderComponent
        if (auto shaderComponent = entityNode["ShaderComponent"])
        {
            auto& sc = entity->AddComponents<Component::ShaderComponent>();
            sc.Identifier = shaderComponent["Proxy"].as<std::string>();
        }

        // // Deserialize children
        // if (auto children = entityNode["Children"])
        // {
        //     for (auto childNode : children)
        //     {
        //         std::string name = "Untagged";
        //         if (auto tagComponent = entityNode["TagComponent"])
        //         {
        //             if (tagComponent["Tag"])
        //                 name = tagComponent["Tag"].as<std::string>();
        //         }
        //
        //         auto child = CurrentScene->CreateEntity(name);
        //         DeserializeEntityRecursive(entityNode, child);
        //         child->SetParent(entity);
        //         entity->AddChild(child);
        //     }
        // }
    }

    void SceneSerializer::PrepareNewSerialization()
    {
        DeserializedEnvironmentLightEntity = nullptr;
        NewActiveCameraSet = false;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
    {
        // Runtime deserialization - could be binary or have different format
        // For now, just use the same format
        return Deserialize(filepath);
    }
}

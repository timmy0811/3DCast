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
    SceneSerializer::SceneSerializer(Cast::Scene* scene)
        : CurrentScene(scene)
    {
    }

    void SceneSerializer::Serialize(const std::string& filepath) const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        auto& entityMap = CurrentScene->EntityDescriptorPool;
        for (auto& [entityHandle, entity] : entityMap)
        {
            if (entity && !entity->IsChild())
            {
                SerializeEntity(out, entity);
            }
        }

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

    void SceneSerializer::SerializeRuntime(const std::string& filepath) const
    {
        // Runtime serialization - could be binary or have different format
        // For now, just use the same format
        Serialize(filepath);
    }

    void SceneSerializer::SerializeEntity(YAML::Emitter& out, const Ref<Entity>& entity)
    {
        if (entity->HasComponent<Component::MeshComponent>() && !entity->GetComponent<Component::MeshComponent>().IsRootNode)
            return;

        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << static_cast<uint32_t>(*entity);

        // Tag Component
        if (entity->HasComponent<Component::TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent

            const auto& tag = entity->GetComponent<Component::TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;

            out << YAML::EndMap; // TagComponent
        }

        // Transform Component
        if (entity->HasComponent<Component::TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            auto& tc = entity->GetComponent<Component::TransformComponent>();
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
            out << YAML::Key << "Projection" << YAML::Value << (int)cc.Camera.ProjectionType;
            out << YAML::Key << "Position" << YAML::Value << cc.Camera.Position;
            out << YAML::Key << "Rotation" << YAML::Value << cc.Camera.Rotation;
            out << YAML::Key << "WorldUp" << YAML::Value << cc.Camera.WorldUp;
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
                    const auto dirLight = static_cast<DirectionalLightShaderObject*>(lc.Light);
                    out << YAML::Key << "Direction" << YAML::Value << dirLight->direction;
                    out << YAML::Key << "Ambient" << YAML::Value << dirLight->ambient;
                    out << YAML::Key << "Diffuse" << YAML::Value << dirLight->diffuse;
                    out << YAML::Key << "Specular" << YAML::Value << dirLight->specular;
                    break;
                }
                case Component::LightComponent::Type::Point:
                {
                    const auto pointLight = static_cast<PointLightShaderObject*>(lc.Light);
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
                    const auto spotLight = static_cast<SpotLightShaderObject*>(lc.Light);
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
            out << YAML::Key << "DiffusePath" << YAML::Value << mc.diffuseFile;
            out << YAML::Key << "SpecularPath" << YAML::Value << mc.specularFile;
            out << YAML::Key << "ParallaxPath" << YAML::Value << mc.parallaxFile;
            out << YAML::Key << "NormalPath" << YAML::Value << mc.normalFile;

            if (mc.isPrivateMaterialCreated)
            {
                out << YAML::Key << "PrivateMaterial" << YAML::Value;
                out << YAML::BeginMap; // PrivateMaterial
                out << YAML::Key << "Diffuse" << YAML::Value << mc.privateMaterial.shaderObject.diffuseColor;
                out << YAML::Key << "Specular" << YAML::Value << mc.privateMaterial.shaderObject.specularColor;
                out << YAML::Key << "Emissive" << YAML::Value << mc.privateMaterial.shaderObject.emissiveColor;
                out << YAML::Key << "Metallic" << YAML::Value << mc.privateMaterial.shaderObject.metallic;
                out << YAML::Key << "Roughness" << YAML::Value << mc.privateMaterial.shaderObject.roughness;
                out << YAML::Key << "Shininess" << YAML::Value << mc.privateMaterial.shaderObject.shininess;
                out << YAML::Key << "Reflectance" << YAML::Value << mc.privateMaterial.shaderObject.reflectance;
                out << YAML::EndMap; // PrivateMaterial
            }

            out << YAML::Key << "IsCustomMaterial" << YAML::Value << mc.isCustomMaterial;
            out << YAML::Key << "SelectedMaterialProxy" << YAML::Value << mc.selectedItem;

            out << YAML::EndMap; // MaterialComponent
        }

        // Mesh Component
        if (entity->HasComponent<Component::MeshComponent>())
        {
            const auto& mc = entity->GetComponent<Component::MeshComponent>();
            if (mc.IsRootNode)
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
#if 0
        // Check if file exists
        if (!std::filesystem::exists(filepath))
        {
            LOG_CORE_ERROR("Scene file does not exist: {0}", filepath);
            return false;
        }

        // Clear existing entities before deserializing
        auto& registry = CurrentScene->Registry;
        registry.clear();
        CurrentScene->GetEntityDescriptors().clear();

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath);
        }
        catch (const YAML::ParserException& e)
        {
            LOG_CORE_ERROR("Failed to load scene file '{0}': {1}", filepath, e.what());
            return false;
        }

        if (!data["Scene"])
        {
            LOG_CORE_ERROR("Scene file is invalid: {0}", filepath);
            return false;
        }

        std::string sceneName = data["Scene"].as<std::string>();
        LOG_CORE_INFO("Deserializing scene: {0}", sceneName);

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entityNode : entities)
            {
                DeserializeEntity(entityNode);
            }
        }

        return true;
#endif
        // Add a return statement
        return false;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
    {
        // Runtime deserialization - could be binary or have different format
        // For now, just use the same format
        return Deserialize(filepath);
    }

    Cast::Ref<Cast::Entity> SceneSerializer::DeserializeEntity(const YAML::Node& entityNode)
    {
        // Extract entity name from TagComponent
        std::string name = "Untagged";
        if (auto tagComponent = entityNode["TagComponent"])
        {
            if (tagComponent["Tag"])
                name = tagComponent["Tag"].as<std::string>();
        }

        // Create the entity
        auto entity = CurrentScene->CreateEntity(name);

        // No need to manually add the TagComponent since CreateEntity already does that

        // Deserialize children
        if (auto children = entityNode["Children"])
        {
            for (auto childNode : children)
            {
                auto childEntity = DeserializeEntity(childNode);
                childEntity->SetParent(entity);
                entity->AddChild(childEntity);
            }
        }

        return entity;
    }
}

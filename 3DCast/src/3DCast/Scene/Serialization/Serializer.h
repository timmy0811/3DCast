#pragma once

#include "3DCast/Scene/Scene.h"
#include <yaml-cpp/yaml.h>
#include <string>

#include "3DCast/Renderer/Skybox.h"
#include "3DCast/Renderer/Camera/Camera.h"
#include "3DCast/Scene/Component/LightComponent.h"

namespace Cast::Serialization
{
    class SceneSerializer
    {
    public:
        explicit SceneSerializer(Scene* scene);

        void SetScene(Scene* scene) { CurrentScene = scene; }

        // Serialization
        void Serialize(const std::string& filepath);
        void SerializeRuntime(const std::string& filepath);

        // Deserialization
        bool Deserialize(const std::string& filepath);
        bool DeserializeRuntime(const std::string& filepath);

        // Callback data
        inline void SetSkyboxCallback(Renderer::Skybox* skybox) { CallbackData.Skybox = skybox; }
        inline void SetActiveCameraCallback(Optional<Renderer::Camera*>* camera) { CallbackData.ActiveCamera = camera; }
        inline void SetEnvironmentLightEntityCallback(Entity** entity) { CallbackData.EnvironmentLightEntity = entity; }
        inline void SetEnvironmentLightComponentCallback(Component::LightComponent** component) { CallbackData.EnvironmentLightComponent = component; }
        inline void SetUseEnvironmentLightingCallback(bool* useEnvironmentLighting) { CallbackData.UseEnvironmentLighting = useEnvironmentLighting; }
        inline void SetRenderModeCallback(int* renderMode) { CallbackData.RenderMode = renderMode; }

        // Additional data

    private:
        void SerializeEntity(YAML::Emitter& out, const Ref<Entity>& entity);
        void DeserializeEntityRecursive(const YAML::Node& entityNode, const Ref<Entity>& entity);
        void PrepareNewSerialization();

    private:
        struct CallbackObjectsData
        {
            Optional<Renderer::Camera*>* ActiveCamera = nullptr;

            // Skybox
            Renderer::Skybox* Skybox = nullptr;
            Entity** EnvironmentLightEntity = nullptr;
            Component::LightComponent** EnvironmentLightComponent = nullptr;
            bool* UseEnvironmentLighting = nullptr;
            int* RenderMode = nullptr;
        } CallbackData{};

        struct AdditionalSerializableData
        {
            // Data here
        } SerializableData{};

        Entity* DeserializedEnvironmentLightEntity = nullptr;
        Scene* CurrentScene = nullptr;

        bool NewActiveCameraSet = false;
        UID PopupID = UID::None();
    };
}

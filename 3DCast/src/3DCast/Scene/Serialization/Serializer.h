#pragma once

#include "3DCast/Scene/Scene.h"
#include <yaml-cpp/yaml.h>
#include <string>

#include "3DCast/Renderer/Skybox.h"
#include "3DCast/Renderer/Camera/Camera.h"

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
        inline void SetEnvironmentLightEntityCallback(Entity* entity) { CallbackData.EnvironmentLightEntity = entity; }

        // Additional data
        inline void AddDataUseEnvironmentLighting(const bool useEnvironmentLighting) { SerializableData.UseEnvironmentLighting = useEnvironmentLighting; }

    private:
        void SerializeEntity(YAML::Emitter& out, const Ref<Entity>& entity);
        void DeserializeEntityRecursive(const YAML::Node& entityNode, const Ref<Entity>& entity);

    private:
        struct CallbackObjectsData
        {
            Optional<Renderer::Camera*>* ActiveCamera;
            Entity* EnvironmentLightEntity = nullptr;
            Renderer::Skybox* Skybox = nullptr;
        } CallbackData{};

        struct AdditionalSerializableData
        {
            bool UseEnvironmentLighting;
        } SerializableData{};

        Entity* DeserializedEnvironmentLightEntity ;
        Scene* CurrentScene = nullptr;

        bool NewActiveCameraSet = false;
    };
}

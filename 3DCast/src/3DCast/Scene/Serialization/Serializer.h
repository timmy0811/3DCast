#pragma once

#include "3DCast/Scene/Scene.h"
#include <yaml-cpp/yaml.h>
#include <string>

namespace Cast::Serialization
{
    class SceneSerializer
    {
    public:
        explicit SceneSerializer(Scene* scene);

        // Serialization
        void Serialize(const std::string& filepath) const;
        void SerializeRuntime(const std::string& filepath) const;

        // Deserialization
        bool Deserialize(const std::string& filepath);
        bool DeserializeRuntime(const std::string& filepath);

    private:
        static void SerializeEntity(YAML::Emitter& out, const Ref<Entity>& entity);
        Ref<Entity> DeserializeEntity(const YAML::Node& entityNode);

        Scene* CurrentScene = nullptr;
    };
}
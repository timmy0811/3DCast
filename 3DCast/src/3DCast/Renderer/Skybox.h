#pragma once

#include "3DCast/Core.h"
#include "API/texture/Cubemap.h"

namespace Cast
{
    namespace Serialization
    {
        class SceneSerializer;
    }

    namespace Renderer {
        class Skybox {
        public:
            enum class RenderMode {
                ClearColor,
                Cubemap,
                Procedural
            };

            struct EnvironmentLight
            {
                glm::vec3 direction{0.0f, 1.0f, 0.0f};
                glm::vec3 ambient{0.1f, 0.1f, 0.1f};
                glm::vec3 diffuse{0.5f, 0.5f, 0.5f};
                glm::vec3 specular{1.0f, 1.0f, 1.0f};
            };

        public:
            explicit Skybox() = default;
            ~Skybox() = default;

            bool AddCubemap(const std::string& name, const std::string& dirPath, const std::string& fileFormat = ".png");
            void RemoveCubemap(const std::string& name);

            void SetActiveCubemap(const std::string& name);

            void BindCurrentCubemap(unsigned int slot) const;
            void UnbindCurrentCubemap() const;

            void CalculateEnvironmentLightForCurrentCubemap();

            void SetCubemapShaderCache(Ref<API::Core::Shader> shader) {
                for (auto& [name, cubemap] : Cubemaps) {
                    cubemap->SetCacheShader(shader.get());
                }
            }

            void SetCubemapViewProjectionMatrix(const glm::mat4& view, const glm::mat4& projection) const {
                for (auto& [name, cubemap] : Cubemaps) {
                    cubemap->SetViewProjectionMatrix(view, projection);
                }
            }

            void SetActiveCubemapShaderCache(Ref<API::Core::Shader> shader) const
            {
                if (ActiveCubemap) {
                    ActiveCubemap->SetCacheShader(shader.get());
                }
            }

            void SetActiveCubemapViewProjectionMatrix(const glm::mat4& view, const glm::mat4& projection) const {
                if (ActiveCubemap) {
                    ActiveCubemap->SetViewProjectionMatrix(view, projection);
                }
            }

            void SetClearColor(const glm::vec4& color) { ClearColor = color; }
            void UseRenderMode(const RenderMode mode) { Mode = mode; }

            inline std::string GetActiveCubemapName() const {
                if (ActiveCubemap) {
                    return ActiveCubemapName;
                }
                return {};
            }

            [[nodiscard]] std::vector<std::string> GetAvailableCubemapNames() const {
                std::vector<std::string> names;
                names.reserve(Cubemaps.size());
                for (const auto& [name, _] : Cubemaps) {
                    names.push_back(name);
                }
                return names;
            }

            [[nodiscard]] inline RenderMode GetRenderMode() const { return Mode; }
            [[nodiscard]] inline glm::vec4& GetClearColor() { return ClearColor; }

            [[nodiscard]] inline glm::vec3 GetLightDirection() const { return CubemapLightCache.at(ActiveCubemapName).direction; }
            [[nodiscard]] inline glm::vec3 GetLightSpecularColor() const { return CubemapLightCache.at(ActiveCubemapName).specular; }
            [[nodiscard]] inline glm::vec3 GetLightDiffuseColor() const { return CubemapLightCache.at(ActiveCubemapName).diffuse; }
            [[nodiscard]] inline glm::vec3 GetLightAmbientColor() const { return CubemapLightCache.at(ActiveCubemapName).ambient; }

            void Render() const;

        private:
            inline bool IsCubemapLoaded(const std::string& name) const {
                return Cubemaps.find(name) != Cubemaps.end();
            }

        private:
            std::unordered_map<std::string, Ref<API::Texture::Cubemap>> Cubemaps{};
            std::unordered_map<std::string, EnvironmentLight> CubemapLightCache{};
            Ref<API::Texture::Cubemap> ActiveCubemap{};
            std::string ActiveCubemapName{};

            RenderMode Mode = RenderMode::Cubemap;
            glm::vec4 ClearColor{0.1, 0.1, 0.1, 1.0};

            friend class Cast::Serialization::SceneSerializer;
        };
    }
}

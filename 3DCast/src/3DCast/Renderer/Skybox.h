#pragma once

#include "3DCast.h"
#include "3DCast.h"
#include "3DCast/Core.h"
#include "API/texture/Cubemap.h"

namespace Cast::Renderer {
    class Skybox {
    public:
        enum class RenderMode {
            ClearColor,
            Cubemap,
            Procedural
        };

    public:
        explicit Skybox() = default;
        ~Skybox() = default;

        bool AddCubemap(const std::string& name, const std::string& dirPath, const std::string& fileFormat = ".png");
        void RemoveCubemap(const std::string& name);

        void SetActiveCubemap(const std::string& name);

        void BindCurrentCubemap(unsigned int slot) const;
        void UnbindCurrentCubemap() const;

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

        [[nodiscard]] inline RenderMode GetRenderMode() const { return Mode; }
        [[nodiscard]] inline glm::vec4& GetClearColor() { return ClearColor; }

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

        void Render() const;

    private:
        inline bool IsCubemapLoaded(const std::string& name) const {
            return Cubemaps.find(name) != Cubemaps.end();
        }

    private:
        std::unordered_map<std::string, Ref<API::Texture::Cubemap>> Cubemaps{};
        Ref<API::Texture::Cubemap> ActiveCubemap{};
        std::string ActiveCubemapName{};

        RenderMode Mode = RenderMode::Cubemap;
        glm::vec4 ClearColor{0.1, 0.1, 0.1, 1.0};
    };
}

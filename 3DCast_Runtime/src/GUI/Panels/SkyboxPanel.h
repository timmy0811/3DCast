#pragma once

#include "3DCast/Renderer/Skybox.h"
#include "3DCast/Scene/Scene.h"
#include "3DCast/Scene/Component/Component.h"

namespace Runtime::GUI
{
    class SkyboxPanel
    {
    public:
        SkyboxPanel() = default;
        explicit SkyboxPanel(Cast::Renderer::Skybox* skybox) : Skybox(skybox) {}

        void Open();

        void SetSkybox(Cast::Renderer::Skybox* skybox);
        void UpdateEnvironmentLight() const;

        void OnImGuiRender();

        [[nodiscard]] inline bool IsUsingEnvironmentMapping() const { return UseEnvironmentLighting; }
        [[nodiscard]] inline bool* GetIsUsingEnvironmentMappingRef() { return &UseEnvironmentLighting; }
        [[nodiscard]] inline int* GetRenderModeRef() { return &CurrentRenderMode; }
        Cast::Entity** GetEnvironmentLightEntityRef() { return &EnvironmentLightEntity; }
        Cast::Component::LightComponent** GetEnvironmentLightComponentRef() { return &EnvironmentLightComponent; }

    private:
        static std::string OpenCubemapDirectoryDialogue();

    private:
        bool IsOpen = true;
        Cast::Renderer::Skybox* Skybox = nullptr;

        Cast::Entity* EnvironmentLightEntity = nullptr;
        Cast::Component::LightComponent* EnvironmentLightComponent = nullptr;

        // Render mode state
        int CurrentRenderMode = 1;

        // Cubemap mode state
        std::string SelectedCubemap;
        std::vector<std::string> AvailableCubemaps;
        bool UseEnvironmentLighting = false;
    };
}

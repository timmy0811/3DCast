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

        inline void SetContext(const Cast::Ref<Cast::Scene>& scene) {Context = scene; }
        void SetSkybox(Cast::Renderer::Skybox* skybox);

        void OnImGuiRender();

    private:
        static std::string OpenCubemapDirectoryDialogue();
        void UpdateEnvironmentLight() const;

    private:
        bool IsOpen = true;
        Cast::Renderer::Skybox* Skybox = nullptr;

        Cast::Ref<Cast::Scene> Context;
        Cast::WeakRef<Cast::Entity> EnvironmentLightEntity;
        Cast::Component::LightComponent* EnvironmentLightComponent = nullptr;

        // Render mode state
        int CurrentRenderMode = 1;

        // Cubemap mode state
        std::string SelectedCubemap;
        std::vector<std::string> AvailableCubemaps;
        bool UseEnvironmentLighting = false;
    };
}

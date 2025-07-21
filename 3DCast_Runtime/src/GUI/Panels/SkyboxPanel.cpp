#include "SkyboxPanel.h"

#include "imgui.h"
#include <nfd.h>

#include "Data/SharedEditorData.h"
#include "vendor/glm/gtc/type_ptr.hpp"

void Runtime::GUI::SkyboxPanel::Open()
{
    IsOpen = true;
}

void Runtime::GUI::SkyboxPanel::SetSkybox(Cast::Renderer::Skybox* skybox)
{
    Skybox = skybox;
    AvailableCubemaps = Skybox->GetAvailableCubemapNames();
    SelectedCubemap = Skybox->GetActiveCubemapName();
}

void Runtime::GUI::SkyboxPanel::OnImGuiRender()
{
    if (!IsOpen || !Skybox)
        return;

    ImGui::Begin("Skybox", &IsOpen);

    const char* renderModes[] = { "Clear Color", "Cubemap", "Procedural" };
    if (ImGui::Combo("Render Mode", &CurrentRenderMode, renderModes, IM_ARRAYSIZE(renderModes)))
    {
        Skybox->UseRenderMode(static_cast<Cast::Renderer::Skybox::RenderMode>(CurrentRenderMode));
    }

    ImGui::Separator();

    switch (CurrentRenderMode)
    {
        case 0: // RenderMode::ClearColor
        {
            ImGui::ColorEdit4("Clear Color", &(Skybox->GetClearColor().x));
            break;
        }

        case 1: // RenderMode::Cubemap
        {
            if (ImGui::BeginCombo("Cubemap", SelectedCubemap.c_str()))
            {
                for (const auto& cubemap : AvailableCubemaps)
                {
                    const bool isSelected = SelectedCubemap == cubemap;
                    if (ImGui::Selectable(cubemap.c_str(), isSelected))
                    {
                        SelectedCubemap = cubemap;
                        Skybox->SetActiveCubemap(cubemap);
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Load from Directory"))
            {
                const std::string dirPath = OpenCubemapDirectoryDialogue();
                if (!dirPath.empty())
                {
                    // Todo: Filetype must be selectable
                    const size_t lastSlash = dirPath.find_last_of("/\\");
                    const std::string cubemapName = lastSlash != std::string::npos ? dirPath.substr(lastSlash + 1) : dirPath;

                    if (Skybox->AddCubemap(cubemapName, dirPath))
                    {
                        AvailableCubemaps = Skybox->GetAvailableCubemapNames();
                        SelectedCubemap = cubemapName;
                        Skybox->SetActiveCubemap(cubemapName);
                        EditorContext.Skybox.SetActiveCubemapShaderCache(Cast::AssetCache.GetShaderHandle("cubemap"));
                    }
                }
            }

            ImGui::Separator();

            if (ImGui::Checkbox("Use Environment Lighting", &UseEnvironmentLighting))
            {
                if (UseEnvironmentLighting)
                {
                    const auto entity = EnvironmentLightEntity.lock();
                    if (!entity)
                    {
                        EnvironmentLightEntity = Context->CreateEntity("Environment Light", true);
                        if (const auto newEntity = EnvironmentLightEntity.lock())
                        {
                            newEntity->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLight(), Cast::Shared.ActiveScene);
                        }
                    }
                }
                else
                {
                    if (auto entity = EnvironmentLightEntity.lock())
                    {
                        LOG_CORE_WARN("Cannot delete light yet. FIX THIS BUG");
                        //Context->RemoveEntity(*entity);
                    }
                }
            }

            if (UseEnvironmentLighting)
            {
                ImGui::Text("Alter \'Env.Light\' to adjust lighting");
                ImGui::Text("Hint: Existing lights can influence the environment lighting");
            }
            break;
        }

        case 2:
        {
            ImGui::TextWrapped("Coming soon!");
            break;
        }
    default: ;
    }

    ImGui::End();
}

std::string Runtime::GUI::SkyboxPanel::OpenCubemapDirectoryDialogue()
{
    nfdu8char_t* outPath;
    const nfdu8char_t* defaultPath = nullptr;
    const nfdresult_t result = NFD_PickFolderU8(&outPath, defaultPath);

    if (result == NFD_OKAY)
    {
        std::string outPathStr(outPath);
        NFD_FreePathU8(outPath);
        return outPathStr;
    }
    else if (result == NFD_CANCEL)
    {
        // User canceled
    }
    else
    {
        // Error
    }

    return "";
}

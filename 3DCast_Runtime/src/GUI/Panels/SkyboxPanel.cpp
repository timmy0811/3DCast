#include "SkyboxPanel.h"

#include "imgui.h"
#include <nfd.h>

#include "Data/SharedEditorData.h"
#include "vendor/glm/gtc/type_ptr.hpp"

#include <3DCast/Misc/Icon.h>

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

    ImGui::Begin(ICON_FA_CLOUD_SUN_RAIN " Skybox", &IsOpen);
    ImGui::BeginDisabled(!Cast::Shared.ActiveScene);

    static const char* renderModes[] = { "Clear Color", "Cubemap", "Procedural" };
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

                static auto format = ".png";

                static const char* formatList[] = { ".jpg", ".png", ".hdr", ".exr", ".tga", ".bmp", ".dds" };
                static int CurrentFormat = 1;

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.4f);
                if (ImGui::Combo("Format", &CurrentFormat, formatList, IM_ARRAYSIZE(formatList)))
                {
                    format = formatList[CurrentFormat];
                }

                ImGui::SameLine();

            if (ImGui::Button("Load from Directory"))
            {
                const std::string dirPath = OpenCubemapDirectoryDialogue();
                if (!dirPath.empty())
                {
                    // Todo: Filetype must be selectable
                    const size_t lastSlash = dirPath.find_last_of("/\\");
                    const std::string cubemapName = lastSlash != std::string::npos ? dirPath.substr(lastSlash + 1) : dirPath;

                    if (Skybox->AddCubemap(cubemapName, dirPath, std::string(format)))
                    {
                        AvailableCubemaps = Skybox->GetAvailableCubemapNames();
                        SelectedCubemap = cubemapName;
                        Skybox->SetActiveCubemap(cubemapName);
                        Skybox->SetActiveCubemapShaderCache(Cast::ShaderCacheRegistryInstance.GetHandle("cubemap"));
                    }
                }
            }

                ImGui::Text("Images in directory must follow naming convention:\npx, py, pz, nx, ny, nz");

            ImGui::Separator();

            if (ImGui::Checkbox("Use Environment Lighting", &UseEnvironmentLighting))
            {
                if (UseEnvironmentLighting)
                {
                    Skybox->CalculateEnvironmentLightForCurrentCubemap();

                    if (!EnvironmentLightEntity)
                    {
                        EnvironmentLightEntity = Cast::Shared.ActiveScene->CreateEntity("Environment Light", true).get();

                        EnvironmentLightEntity->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLightShaderObject(), &Cast::Shared.ActiveScene.value());
                        EnvironmentLightComponent = &EnvironmentLightEntity->GetComponent<Cast::Component::LightComponent>();
                        EnvironmentLightComponent->IsEnvironmentLight = true;
                    }

                    UpdateEnvironmentLight();
                }
                else
                {
                    Cast::Shared.ActiveScene->RemoveEntity(*EnvironmentLightEntity);
                }
            }

                ImGui::SameLine();
                ImGui::Text("(?)");

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Lower Cutoff is set to 40%%");
                    ImGui::Text("Only the upper 45%% of pixels will be used for calculation.");
                    ImGui::Text("");
                    ImGui::Text("Results for light direction can be off.");
                    ImGui::Text("Manually adjust it on the entity.");
                    ImGui::EndTooltip();
                }

            if (UseEnvironmentLighting)
            {
                ImGui::Text("Alter \'EnvironmentLight\' to adjust lighting");
                ImGui::Text("Hint: Existing lights can influence the environment lighting");

                static std::string lastActiveCubemap = SelectedCubemap;
                if (SelectedCubemap != lastActiveCubemap)
                {
                    Skybox->CalculateEnvironmentLightForCurrentCubemap();
                    UpdateEnvironmentLight();
                }

                lastActiveCubemap = SelectedCubemap;
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

    ImGui::EndDisabled();
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
        LOG_ERROR("NFD_PickFolderU8 failed");
    }

    return "";
}

void Runtime::GUI::SkyboxPanel::UpdateEnvironmentLight() const
{
    if (EnvironmentLightComponent)
    {
        const auto dirLight = (Cast::DirectionalLightShaderObject*)(EnvironmentLightComponent->Light);
        dirLight->ambient = Skybox->GetLightAmbientColor();
        dirLight->diffuse = Skybox->GetLightDiffuseColor();
        dirLight->specular = Skybox->GetLightSpecularColor();
        dirLight->direction = Skybox->GetLightDirection();

        EnvironmentLightComponent->UpdateLightData();
    }
}

#pragma once

#include "imgui.h"

#include "Data/SharedEditorData.h"
#include "Layer/Blocks/RasterizationViewport.h"

#include <functional>

namespace Runtime::GUI
{
    inline void PopupRasterSettings(bool open)
    {
        if (open)
        {
            ImGui::OpenPopup(ICON_FA_GEARS " Raster Settings");
            ImGui::SetNextWindowSizeConstraints(ImVec2(750.0f, 0.0f), ImVec2(1100.0f, FLT_MAX));
            open = false;
        }

        if (ImGui::BeginPopupModal(ICON_FA_GEARS " Raster Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::SeparatorText("Texture");

            // Parallax Scale
            ImGui::TextUnformatted("Parallax Scale");
            HALF_INPUT
            ImGui::DragFloat("##ParallaxScale", &Runtime::EditorContext.ViewSettings.ParallaxScale, 0.002f, 0.0f, 1.5f);

            // SSAO
            ImGui::SeparatorText("Ambient Occlusion");

            ImGui::TextUnformatted("Enable SSAO");
            HALF_INPUT
            ImGui::Checkbox("##SSAOEnabled", &Runtime::EditorContext.ViewSettings.SSAOEnabled);

            ImGui::TextUnformatted("SSAO Strength");
            HALF_INPUT
            ImGui::SliderFloat("##SSAOAffectness", &Runtime::EditorContext.ViewSettings.SSAOAffectness, 0.0f, 1.0f, "%.2f");

            // Retro effects
            ImGui::SeparatorText("Retro Effects");

            bool ditheringChanged = false;

            // Dithering
            ImGui::TextUnformatted("Enable Dithering");
            HALF_INPUT
            if (ImGui::Checkbox("##DitheringEnabled", &Runtime::EditorContext.ViewSettings.DitheringEnabled))
                ditheringChanged = true;

            ImGui::TextUnformatted("Dithering Strength");
            HALF_INPUT
            if (ImGui::SliderFloat("##DitheringStrength", &Runtime::EditorContext.ViewSettings.DitheringStrength, 0.0f, 2.0f, "%.2f"))
                ditheringChanged = true;

            ImGui::TextUnformatted("Color Depth");
            HALF_INPUT
            if (ImGui::SliderInt("##DitheringColorDepth", &Runtime::EditorContext.ViewSettings.DitheringColorDepth, 4, 64, "%d levels"))
                ditheringChanged = true;

            ImGui::TextUnformatted("Pattern Scale");
            HALF_INPUT
            if (ImGui::SliderFloat("##DitheringScale", &Runtime::EditorContext.ViewSettings.DitheringScale, 1.0f, 8.0f, "%.1f"))
                ditheringChanged = true;

            if (ditheringChanged)
                Runtime::RasterizationViewport::UpdateDitheringUniforms();

            bool retroSettingsChanged = false;

            ImGui::Separator();
            // Vertex Snapping (Wobble)
            ImGui::TextUnformatted("Enable Vertex Snapping");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Snaps vertices to a low-resolution grid,\ncausing a retro vertex wobble effect.");
            HALF_INPUT
            if (ImGui::Checkbox("##VertexSnappingEnabled", &Runtime::EditorContext.ViewSettings.VertexSnappingEnabled))
                retroSettingsChanged = true;

            ImGui::TextUnformatted("Snapping Resolution");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Lower values = more wobble.");
            HALF_INPUT
            if (ImGui::SliderFloat("##VertexSnappingResolution", &Runtime::EditorContext.ViewSettings.VertexSnappingResolution, 32.0f, 512.0f, "%.0f"))
                retroSettingsChanged = true;

            ImGui::Separator();
            // Affine Texture Mapping
            ImGui::TextUnformatted("Affine Texture Mapping");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Disables perspective-correct texture mapping,\ncausing a texture warping effect.");
            HALF_INPUT
            if (ImGui::Checkbox("##AffineTextureMappingEnabled", &Runtime::EditorContext.ViewSettings.AffineTextureMappingEnabled))
                retroSettingsChanged = true;

            ImGui::TextUnformatted("Affine Strength");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("0.0 = perspective-correct (no effect)\n1.0 = fully affine (maximum warping)\nLower values give a more subtle effect.");
            HALF_INPUT
            if (ImGui::SliderFloat("##AffineTextureStrength", &Runtime::EditorContext.ViewSettings.AffineTextureStrength, 0.0f, 1.0f, "%.2f"))
                retroSettingsChanged = true;


            if (retroSettingsChanged)
                Runtime::RasterizationViewport::UpdateRetroUniforms();

            // Shadow Mapping
            ImGui::SeparatorText("Shadow Mapping");
            // Shadow Bias Factors
            for (int i = 0; i < 3; ++i)
            {
                ImGui::Text("Shadow Bias Cascade %d", i);
                HALF_INPUT
                std::string id = "##ShadowBiasFactor" + std::to_string(i);
                ImGui::DragFloat(id.c_str(), &Runtime::EditorContext.ViewSettings.ShadowBiasFactors[i], 0.1f, 0.0f, 100.0f, "%.1f");
            }

            ImGui::Separator();
            {
                const ImGuiStyle& style = ImGui::GetStyle();
                const float btnWidth = ImGui::CalcTextSize("Close").x + style.FramePadding.x * 2.0f;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - btnWidth);
                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    inline void PopupViewportSettings(bool open, const std::function<void()>& onResizeCallback)
    {
        if (open)
        {
            ImGui::OpenPopup(ICON_FA_GEARS " Viewport Resolution");
            open = false;
        }

        if (ImGui::BeginPopupModal(ICON_FA_GEARS " Viewport Resolution", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginTable("ViewportSettingsTable", 2, ImGuiTableFlags_SizingStretchProp))
            {
                constexpr float labelWidth = 250.0f;
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
                ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);

                // Adjust to window size
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Adjust to window size");
                ImGui::TableSetColumnIndex(1);
                ImGui::Checkbox("##AdjustToWindowSize", &Runtime::EditorContext.ViewSettings.AdjustToWindowSize);

                // Width
                ImGui::BeginDisabled(Runtime::EditorContext.ViewSettings.AdjustToWindowSize);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Width");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(200.f);
                ImGui::InputInt("##ViewportWidth", &Runtime::EditorContext.ViewSettings.ViewportWidth);

                // Height
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Height");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(200.f);
                ImGui::InputInt("##ViewportHeight", &Runtime::EditorContext.ViewSettings.ViewportHeight);
                ImGui::EndDisabled();

                // Filter Mode
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Texture Filter");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(200.f);

                const char* filterModes[] = { "Nearest (Pixelated)", "Linear (Smooth)" };
                int currentFilter = static_cast<int>(Runtime::EditorContext.ViewSettings.FilterMode);
                if (ImGui::Combo("##FilterMode", &currentFilter, filterModes, IM_ARRAYSIZE(filterModes)))
                {
                    Runtime::EditorContext.ViewSettings.FilterMode = static_cast<Runtime::ViewportFilterMode>(currentFilter);
                }

                ImGui::EndTable();
            }

            ImGui::Separator();
            {
                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();
                const ImGuiStyle& style = ImGui::GetStyle();
                const float btnWidth = ImGui::CalcTextSize("Apply").x + style.FramePadding.x * 2.0f;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - btnWidth);

                if (ImGui::Button("Apply"))
                {
                    if (onResizeCallback)
                    {
                        onResizeCallback();
                    }
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }
}

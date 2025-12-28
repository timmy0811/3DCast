#pragma once

#include "imgui.h"

#include "Data/SharedEditorData.h"

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
            constexpr float labelWidth = 350.0f;

            // Parallax Scale
            ImGui::SeparatorText("Texture");
            if (ImGui::BeginTable("TextureSettingsTable", 2, ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
                ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthFixed, 210.f);

                // Parallax Scale
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                ImGui::TextUnformatted("Parallax Scale");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("##ParallaxScale", &Runtime::EditorContext.ViewSettings.ParallaxScale, 0.002f, 0.0f, 1.5f);

                ImGui::EndTable();
            }

            // SSAO
            ImGui::SeparatorText("Ambient Occlusion");
            if (ImGui::BeginTable("AOSettingsTable", 2, ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
                ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthFixed, 210.f);

                // Enable SSAO
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Enable SSAO");
                ImGui::TableSetColumnIndex(1);
                ImGui::Checkbox("##SSAOEnabled", &Runtime::EditorContext.ViewSettings.SSAOEnabled);

                // SSAO Strength
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("SSAO Strength");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(200.f);
                ImGui::SliderFloat("##SSAOAffectness", &Runtime::EditorContext.ViewSettings.SSAOAffectness, 0.0f, 1.0f, "%.2f");

                ImGui::EndTable();
            }

            // Shadow Mapping
            ImGui::SeparatorText("Shadow Mapping");
            if (ImGui::BeginTable("ShadowSettingsTable", 2, ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
                ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthFixed, 210.f);

                // Shadow Bias Factors
                for (int i = 0; i < 3; ++i)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Shadow Bias Cascade %d", i);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(200.f);
                    std::string id = "##ShadowBiasFactor" + std::to_string(i);
                    ImGui::DragFloat(id.c_str(), &Runtime::EditorContext.ViewSettings.ShadowBiasFactors[i], 0.1f, 0.0f, 100.0f, "%.1f");
                }

                ImGui::EndTable();
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

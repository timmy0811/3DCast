#pragma once

#include "imgui.h"

#include "Data/SharedEditorData.h"

namespace Runtime::GUI
{
    inline void PopupRasterSettings(bool open)
    {
        if (open)
        {
            ImGui::OpenPopup(ICON_FA_GEARS " Raster Settings");
            ImGui::SetNextWindowSizeConstraints(ImVec2(360.0f, 0.0f), ImVec2(560.0f, FLT_MAX));
            open = false;
        }

        if (ImGui::BeginPopupModal(ICON_FA_GEARS " Raster Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginTable("RasterSettingsTable", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);

                // Parallax Scale
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Parallax Scale");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                ImGui::DragFloat("##ParallaxScale", &Runtime::EditorContext.ViewSettings.ParallaxScale, 0.002f, 0.0f, 1.5f);

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
                ImGui::SetNextItemWidth(-1);
                ImGui::SliderFloat("##SSAOAffectness", &Runtime::EditorContext.ViewSettings.SSAOAffectness, 0.0f, 1.0f, "%.2f");

                ImGui::EndTable();
            }

            ImGui::Separator();
            // Right-align the Close button
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
}

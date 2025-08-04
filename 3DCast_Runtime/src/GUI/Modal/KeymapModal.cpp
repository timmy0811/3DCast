#include "runtimepch.h"
#include "KeymapModal.h"

#include <imgui.h>

#include "Application/KeymapLayout.h"

#include <3DCast/Misc/Icon.h>

void Runtime::GUI::Keymap::OnImGuiRender()
{
    if (g_ShowKeymapModal)
    {
        ImGui::OpenPopup(ICON_FA_KEYBOARD " Edit Keymap");
        g_ShowKeymapModal = false;
    }

    ImGui::SetNextWindowSize(ImVec2(650, 0), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopupModal(ICON_FA_KEYBOARD " Edit Keymap", nullptr, ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::BeginChild("TableContainer", ImVec2(0, 450), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        if (ImGui::BeginTable("KeymapTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 200.f);
            ImGui::TableSetupColumn("Key Binding", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (const auto& [action, description] : Runtime::Application::Keymap::ActionToString)
            {
                ImGui::TableNextRow(0.f, 25.f);

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(std::get<0>(description).c_str());

                ImGui::TableSetColumnIndex(1);

                const bool isEditable = std::get<1>(description);
                auto& keys = Runtime::Application::Keymap::Layout[action];

                if (!isEditable)
                {
                    ImGui::BeginDisabled();
                    std::string keyString;
                    bool first = true;
                    for (const auto& key : keys)
                    {
                        if (!first) keyString += " + ";
                        first = false;
                        keyString += Application::Keymap::KeyToString(key);
                    }
                    ImGui::TextUnformatted(keyString.c_str());
                    ImGui::EndDisabled();
                }
                else
                {
                    int idx = 0;
                    for (auto it = keys.begin(); it != keys.end(); ++it, ++idx)
                    {
                        char label[32];
                        snprintf(label, sizeof(label), "##key%d_%d", static_cast<int>(action), idx);

                        ImGui::SetNextItemWidth(140.0f);
                        if (ImGui::BeginCombo(label, Application::Keymap::KeyToString(*it)))
                        {
                            for (const int k : Application::Keymap::AvailableKeys)
                            {
                                const bool isSelected = (*it == k);
                                if (ImGui::Selectable(Application::Keymap::KeyToString(k), isSelected))
                                {
                                    *it = k;
                                }

                                if (isSelected)
                                    ImGui::SetItemDefaultFocus();
                            }

                            ImGui::EndCombo();
                        }

                        if (std::next(it) != keys.end())
                        {
                            ImGui::SameLine();
                            ImGui::Text("+");
                            ImGui::SameLine();
                        }
                    }

                    ImGui::SameLine();
                    char addButtonLabel[32];
                    snprintf(addButtonLabel, sizeof(addButtonLabel), "+##add_%d", static_cast<int>(action));
                    if (ImGui::Button(addButtonLabel))
                    {
                        keys.push_back(CAST_KEY_W);
                    }

                    if (keys.size() > 1)
                    {
                        ImGui::SameLine();
                        char removeButtonLabel[32];
                        snprintf(removeButtonLabel, sizeof(removeButtonLabel), "-##remove_%d", static_cast<int>(action));
                        if (ImGui::Button(removeButtonLabel))
                        {
                            keys.pop_back();
                        }
                    }
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar(2);

        const float windowWidth = ImGui::GetContentRegionAvail().x;
        constexpr float buttonWidth = 120;
        const float buttonX = windowWidth - buttonWidth;

        ImGui::SetCursorPosX(buttonX);
        if (ImGui::Button("Close", ImVec2(buttonWidth, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

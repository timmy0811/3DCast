#pragma once

#include "3DCast/Gui/TempElements/TempGuiElement.h"
#include "3DCast/Data/GlobalShared.h"

#include <imgui.h>

#include <utility>

#include "3DCast/Gui/Font.h"

namespace Cast::GUI {
	class NotificationModal final : public TempGuiElement {
	public:
		NotificationModal(std::string title, std::string message, ICON_NAME  icon = NO_ICON, const bool okButton = false, const bool closable = true, const ImVec2 size = ImVec2(0, 0))
			: title(std::move(title)), message(std::move(message)), icon(std::move(icon)), size(size), firstRender(true), okButton(okButton), closable(closable)
		{
		}

		void SetTextAttrib(const char* text) override
		{
			message = text;
		}

		bool Render() override {
			if (firstRender) {
				ImGui::OpenPopup(title.c_str());
				firstRender = false;
			}

			constexpr auto minSize = ImVec2(500.0f, 100.0f);
			constexpr auto maxSize = ImVec2(900.f, 400.f);
			ImGui::SetNextWindowSizeConstraints(minSize, maxSize);

			ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Appearing);

			if (ImGui::BeginPopupModal(title.c_str(), closable ? &isOpen : nullptr,
		    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
		    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
			{
			    const ImVec2 winSize = ImGui::GetWindowSize();
			    const float verticalPadding = winSize.y * 0.05f;
			    const float horizontalPadding = winSize.x * 0.08f;

			    ImGui::Dummy(ImVec2(0.0f, verticalPadding));

			    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + horizontalPadding);
			    ImGui::BeginGroup();

			    if (icon != NO_ICON) {
				    constexpr float iconSize = 32.f;
			        ImGui::BeginGroup();

			        const float textHeight = ImGui::CalcTextSize(message.c_str(), nullptr, false,
			                          ImGui::GetContentRegionAvail().x - iconSize - 30.0f).y;
			        const float iconPadding = (textHeight - iconSize) * 0.5f;

			        if (iconPadding > 0) {
			            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + iconPadding);
			        }

			    	ImGui::PushFont(Font::fa900_64px);
			    	ImGui::TextUnformatted(icon.c_str());
			    	ImGui::PopFont();
			        ImGui::EndGroup();

			        ImGui::SameLine(iconSize + 40.0f);

			        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - horizontalPadding));
			        ImGui::Text("%s", message.c_str());
			        ImGui::PopTextWrapPos();
			    }
			    else {
			        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - horizontalPadding));
			        ImGui::Text("%s", message.c_str());
			        ImGui::PopTextWrapPos();
			    }

				if (okButton && closable)
				{
					ImGui::Dummy(ImVec2(0.0f, 20.0f));

					constexpr float buttonWidth = 120.0f;
					const float availableWidth = winSize.x - (2.0f * horizontalPadding);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
										 (availableWidth - buttonWidth) * 0.5f);
					if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
						isOpen = false;
						ImGui::CloseCurrentPopup();
					}
				}

			    ImGui::EndGroup();

			    ImGui::Dummy(ImVec2(0.0f, verticalPadding));

			    ImGui::EndPopup();
			}

			return !isOpen;
		}

	private:
		std::string  title;
		std::string message;
		ICON_NAME icon;
		ImVec2 size;

		bool firstRender;
		bool isOpen = true;
		bool okButton;
		bool closable = true;
	};
}

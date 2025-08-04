#pragma once

#include "3DCast/ImGui/TempElements/TempGuiElement.h"
#include "3DCast/Data/GlobalShared.h"

#include <imgui.h>

namespace Cast::GUI {
	class NotificationModal final : public TempGuiElement {
	public:
		NotificationModal(const char* title, const char* message, const ImVec2 size = ImVec2(0, 0))
			: title(title), message(message), size(size) {}

		bool Render() override {
			ImGui::SetNextWindowPos(ImVec2(Shared.WindowCenter.x, Shared.WindowCenter.y), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			ImGui::SetNextWindowSize(size);
			ImGui::Begin((ICON_FA_HOURGLASS_HALF + std::string(title)).c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::Text("%s", message);

			bool res = false;
			if (ImGui::Button("OK")) {
				ImGui::CloseCurrentPopup();
				res = true;
			}

			ImGui::End();
			return res;
		}

	private:
		const char* title;
		const char* message;
		ImVec2 size;
	};
}
#pragma once

#include "3DCast/Data/GlobalShared.h"

#include <imgui.h>
#include <string>

#define GUIWIN_WSIXTH (ImGui::GetWindowWidth() * 0.16667f)
#define GUIWIN_WQUARTER (ImGui::GetWindowWidth() * 0.25f)
#define GUIWIN_WTHIRD (ImGui::GetWindowWidth() * 0.33333f)
#define GUIWIN_WHalf (ImGui::GetWindowWidth() * 0.5f)

#define TWOTHIRD_INPUT ImGui::SetNextItemWidth(GUIWIN_WTHIRD * 2.f - GUIWIN_ENDELEMENT_PADDING);
#define HALF_INPUT ImGui::SameLine(GUIWIN_WHalf); ImGui::SetNextItemWidth(GUIWIN_WHalf - GUIWIN_ENDELEMENT_PADDING);

#define GUIWIN_ELEMENT_PADDING 15.f
#define GUIWIN_ENDELEMENT_PADDING 25.f
#define DUMMYSPACE_AFTER_COMPONENT 8.f

namespace Cast::UI {
	static void ModalImportInProgress(const std::string& path, const bool end = false) {
		ImGui::SetNextWindowPos(ImVec2(Shared.WindowCenter.x, Shared.WindowCenter.y), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::OpenPopup("ModelImporting");

		if (ImGui::BeginPopupModal("ModelImporting", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Model import running for:");
			ImGui::Text(path.c_str());
			ImGui::Text("Please wait :)");

			if (end)
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
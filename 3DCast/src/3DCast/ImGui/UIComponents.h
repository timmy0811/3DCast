#pragma once

#include "3DCast/Data/GlobalShared.h"

#include <imgui.h>
#include <string>

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
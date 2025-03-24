#pragma once

#include <imgui.h>
#include <string>

namespace Cast::UI {
	void ModalImportInProgress(const std::string& path, bool end = false) {
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f,
			ImGui::GetIO().DisplaySize.y * 0.5f),
			ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		static bool show = false;

		if (!show) {
			ImGui::OpenPopup("ModelImporting");
			show = true;
		}

		if (ImGui::BeginPopupModal("ModelImporting", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Model import running for %s", path.c_str());
			ImGui::Text("Please wait :)");

			if (end)
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
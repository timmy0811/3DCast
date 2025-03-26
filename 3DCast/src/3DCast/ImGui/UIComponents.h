#pragma once

#include <imgui.h>
#include <string>

namespace Cast::UI {
	static void ModalImportInProgress(const std::string& path, bool end = false) {
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
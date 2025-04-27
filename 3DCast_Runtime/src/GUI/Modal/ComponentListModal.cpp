#include "ComponentListModal.h"

#include <string>
#include <algorithm>
#include <imgui.h>

#include <3DCast/Scene/Component/Component.h>

bool MatchesSearch(const std::string& text, const char* search)
{
	if (!search || !*search)
		return true;

	std::string lowerText, lowerSearch;
	lowerText.resize(text.size());
	std::transform(text.begin(), text.end(), lowerText.begin(), ::tolower);

	lowerSearch.resize(strlen(search));
	std::transform(search, search + strlen(search), lowerSearch.begin(), ::tolower);

	return (lowerText.find(lowerSearch) != std::string::npos);
}

Runtime::GUI::ComponentList::ModalResult Runtime::GUI::ComponentList::OnImGuiRender()
{
	if (g_ShowAddComponentModal)
	{
		ImGui::OpenPopup("Add Component Modal");
		g_ShowAddComponentModal = false;
	}

	ModalResult res = ModalResult::None;

	if (ImGui::BeginPopupModal("Add Component Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::InputText("Search", g_SearchBuffer, IM_ARRAYSIZE(g_SearchBuffer));

		ImGui::BeginChild("ComponentList", ImVec2(300, 200), true);
		for (size_t i = 0; i < Cast::Component::AddableComponentIds.size(); i++)
		{
			if (MatchesSearch(Cast::Component::AddableComponentIds[i], g_SearchBuffer))
			{
				ImGui::Checkbox(Cast::Component::AddableComponentIds[i].c_str(), &g_Selections[i]);
			}
		}

		ImGui::EndChild();

		if (ImGui::Button("Add Selected"))
		{
			std::vector<std::string> selectedComponents;
			for (size_t i = 0; i < Cast::Component::AddableComponentIds.size(); i++)
			{
				if (g_Selections[i])
				{
					selectedComponents.push_back(Cast::Component::AddableComponentIds[i]);
				}
			}

			ImGui::CloseCurrentPopup();
			res = ModalResult::Success;
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			res = ModalResult::Cancel;
		}

		ImGui::EndPopup();
		return res;
	}

	return ModalResult::None;
}

void Runtime::GUI::ComponentList::Reset()
{
	g_ShowAddComponentModal = true;
	g_SearchBuffer[0] = '\0';
	std::fill(std::begin(g_Selections), std::end(g_Selections), false);
}
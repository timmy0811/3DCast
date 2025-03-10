#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

namespace Cast::Component {
	struct MeshComponent : public Component
	{
		std::string MeshPath;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path)
			: MeshPath(path) {}

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Path:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);

				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), MeshPath.c_str());

				if (ImGui::InputText("##MeshPath", buffer, sizeof(buffer)))
				{
					MeshPath = std::string(buffer);
				}
			}
		}
	};
}
#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

namespace Cast::Component
{
	struct TagComponent : public Component
	{
		std::string Tag;

		TagComponent() { Tag.reserve(MAX_TAG_SIZE); };
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) {
			Tag = tag.empty() ? "Untagged" : tag;
			Tag.reserve(MAX_TAG_SIZE);
		}

		virtual void OnImGuiRender() override
		{
			ImGui::Text("Name:");
			ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
			ImGui::InputText("##Tag", Tag.data(), MAX_TAG_SIZE);
		}

	private:
		constexpr static size_t MAX_TAG_SIZE = 96;
	};
}
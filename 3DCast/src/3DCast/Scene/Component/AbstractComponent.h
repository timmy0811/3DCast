#pragma once

#include "3DCast/Core.h"
#include "3DCast/Scene/Component/Typedefinition.h"

#include <string>

#define SAMELINE_WIDGET_OFFSET ImGui::GetWindowWidth() / 3.0f

namespace Cast {
	class Entity;
}

namespace Cast::Component {
	struct Component
	{
		virtual ~Component() = default;

		virtual UIResponse OnImGuiRender() { return {}; };
		virtual void Print() {};
		virtual void OnAfterEntitySetBehaviour() {};

		void SetEntity(Entity* entity) { EntityNode = entity; }

		virtual std::string GetName() const
		{
			return "Component";
		}

	protected:
		Entity* EntityNode = nullptr;

		friend class Entity;
	};
}
#pragma once

#include "3DCast/Core.h"
#include "3DCast/Scene/Component/Typedefinition.h"

#include <string>

#define SAMELINE_WIDGET_OFFSET ImGui::GetWindowWidth() / 3.0f

namespace Cast
{
	class Entity;
}

namespace Cast::Component
{
	struct Component
	{
		virtual ~Component()
		{
		}

		virtual UIResponse OnImGuiRender() { return {}; };

		virtual void Print()
		{
		};

		virtual void OnAfterEntitySetBehaviour()
		{
		};

		void SetEntity(Ref<Entity> entity) { EntityNode = entity; }

		static std::string GetName()
		{
			return "Component";
		}

	protected:
		Ref<Entity> EntityNode = nullptr;

		friend class Entity;
	};
}

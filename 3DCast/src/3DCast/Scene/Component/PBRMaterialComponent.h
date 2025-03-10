#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

namespace Cast::Component {
	struct PBRMaterialComponent : public Component
	{
		bool temp;

		PBRMaterialComponent() = default;
		PBRMaterialComponent(const PBRMaterialComponent&) = default;
	};
}
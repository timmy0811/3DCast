#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

namespace Cast::Component {
	struct RasterizableComponent : public Component
	{
		bool Renderable{ true };

		RasterizableComponent() = default;
		RasterizableComponent(const RasterizableComponent&) = default;
		RasterizableComponent(bool enable)
			: Renderable(enable) {}
	};

	struct PBRComponent : public Component
	{
		bool Renderable{ true };

		PBRComponent() = default;
		PBRComponent(const PBRComponent&) = default;
		PBRComponent(bool enable)
			: Renderable(enable) {}
	};
}
#pragma once

#include "3DCast/Core.h"
#include "3DCast/Scene/Scene.h"

#include <Vendor/glm/glm.hpp>

namespace Cast {
	struct SharedClass
	{
		Ref<Scene> ActiveScene;
		glm::vec2 WindowCenter;
	};

	extern SharedClass Shared;
}
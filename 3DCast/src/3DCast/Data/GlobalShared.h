#pragma once

#include "3DCast/Core.h"
#include "3DCast/Scene/Scene.h"

namespace Cast {
	struct SharedClass
	{
		Ref<Scene> ActiveScene;
	};

	extern SharedClass Shared;
}
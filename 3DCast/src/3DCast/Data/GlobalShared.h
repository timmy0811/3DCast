#pragma once

#include "3DCast/Core.h"
#include "3DCast/Scene/Scene.h"
#include "3DCast/Misc/UID.h"
#include "3DCast/Model/IVertexEntity.h"

#include <vendor/glm/glm.hpp>

namespace Cast
{
	struct SharedClass
	{
#pragma region SCENE
		Optional<Scene> ActiveScene;
		std::map<uid, IVertexEntity*> VertexEntities;
#pragma endregion

#pragma region WINDOW
		glm::vec2 WindowCenter{};
#pragma endregion
	};

	extern SharedClass Shared;
}

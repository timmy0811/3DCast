#pragma once

#include "3DCast/Core.h"
#include "3DCast/Scene/Scene.h"
#include "3DCast/Misc/UID.h"
#include "3DCast/Model/IVertexEntity.h"

#include <vendor/glm/glm.hpp>

#include "3DCast/Core/Window.h"

namespace Cast
{
	struct SharedClass
	{
#pragma region SCENE
		Optional<Scene> ActiveScene;
		std::map<uid, IVertexEntity*> VertexEntities;
#pragma endregion

#pragma region WINDOW
		Window* AppWindow{ nullptr };
		glm::vec2 WindowCenter{};
#pragma endregion
	};

	extern SharedClass Shared;
}

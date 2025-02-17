#pragma once

#include "Vendor/glm/glm.hpp"

namespace Cast::Memory {
	struct BatchVertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		float TransformIndex;
	};
}
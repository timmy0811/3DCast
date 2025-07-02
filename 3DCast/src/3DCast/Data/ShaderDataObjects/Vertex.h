#pragma once

#include "vendor/glm/glm.hpp"

namespace Cast::Memory
{
	struct BatchVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		glm::vec2 TexCoords;
		float SamplerIndex;
		float TransformIndex;
	};
}

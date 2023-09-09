#pragma once

#include "../vendor/glm/glm.hpp"

namespace GL::material {
	struct Material {
		int texture;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shine;
	};
}
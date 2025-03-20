#pragma once

#include <Vendor/glm/glm.hpp>

#include "Scene.h"

namespace Cast::Create {
	Ref<Cast::Entity> Cube(const std::string& name, Cast::Scene* scene);
	Ref<Cast::Entity> Plane(const std::string& name, Cast::Scene* scene);

	std::pair<glm::vec3, glm::vec3> GetTangentAndBitangent(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2);
}
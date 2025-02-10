#pragma once

#include "Scene.h"

namespace Cast::Create {
	Cast::Entity Cube(const std::string& name, Cast::Scene* scene, int texIndex = 0, float shine = 32.f, float reflectance = 0.5f);

	Cast::Entity Plane(const std::string& name, Cast::Scene* scene);
}
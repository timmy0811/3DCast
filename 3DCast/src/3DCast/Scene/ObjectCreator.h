#pragma once

#include "Scene.h"

namespace Cast::Create {
	Cast::Entity Cube(const std::string& name, Cast::Scene* scene);
	Cast::Entity Plane(const std::string& name, Cast::Scene* scene);
}
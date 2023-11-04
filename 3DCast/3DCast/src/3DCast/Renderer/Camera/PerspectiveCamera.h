#pragma once

#include "Camera.h"

namespace Cast::Renderer {
	class PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera(float fov, float aspect, float nearPlane = 0.1f, float farPlane = 100.f);
	};
}
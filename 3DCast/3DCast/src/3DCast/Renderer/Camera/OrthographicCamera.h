#pragma once

#include "Camera.h"

namespace Cast::Renderer {
	class OrthographicCamera : public Camera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float nearPlane = -1.f, float farPlane = 1.f);
	};
}
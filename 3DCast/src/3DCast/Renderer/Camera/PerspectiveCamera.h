#pragma once

#include "Camera.h"

namespace Cast::Renderer {
	class PerspectiveCamera final : public Camera {
	public:
		PerspectiveCamera() = default;
		PerspectiveCamera(float fov, float aspect, float nearPlane = 0.1f, float farPlane = 100.f);

		void SetAspectRatio(float aspectRatio);

	private:
		float Fov = glm::radians(90.f);
		float NearPlane = 0.1f;
		float FarPlane = 100.f;
	};
}
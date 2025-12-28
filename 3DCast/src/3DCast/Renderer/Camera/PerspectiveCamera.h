#pragma once

#include "Camera.h"

namespace Cast::Renderer {
	class PerspectiveCamera final : public Camera {
	public:
		PerspectiveCamera()
		{
			ProjectionType = Type::Perspective;
			UpdateProjectionMat();
		}
		PerspectiveCamera(float fov, float aspect, float nearPlane = 0.1f, float farPlane = 100.f);

		[[nodiscard]] inline float GetFOV() const { return Fov; }
		[[nodiscard]] inline float GetAspectRatio() const { return AspectRatio; }
		[[nodiscard]] inline float GetNearPlane() const { return NearPlane; }
		[[nodiscard]] inline float GetFarPlane() const { return FarPlane; }

		inline void SetFOV(const float fov)
		{
			if (Fov == fov) return;
			Fov = fov;
			UpdateProjectionMat();
		}

		inline void SetAspectRatio(const float aspectRatio)
		{
			if (AspectRatio == aspectRatio) return;
			AspectRatio = aspectRatio;
			UpdateProjectionMat();
		}

	private:
		void UpdateProjectionMat()
		{
			ProjectionMat = glm::perspective(glm::radians(Fov), AspectRatio, NearPlane, FarPlane);
			ViewProjectionMat = ProjectionMat * ViewMat;
			MarkAsChanged();
		}

		float Fov = 85.0f;
		float AspectRatio = 16.0f / 9.0f;
		float NearPlane = 0.1f;
		float FarPlane = 100.0f;
	};
}
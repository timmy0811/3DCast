#pragma once

#include <vendor/glm/glm.hpp>

namespace Cast::Renderer {
	class Camera {
	public:
		Camera();
		virtual ~Camera() = default;

		virtual void LookAt(const glm::vec3& target, const glm::vec3& up = { 0.f, 1.f, 0.f });

		virtual inline const glm::vec3& GetPosition() const { return position; }
		virtual inline void SetPosition(const glm::vec3& position) { this->position = position; UpdateViewMat(); }

		virtual inline float GetZRotation() const { return rotation.z; }
		virtual inline const glm::vec3& GetRotation() const { return rotation; }
		virtual inline void SetRotation(const glm::vec3& rotation) { this->rotation = rotation; UpdateViewMat(); }
		virtual inline void SetZRotation(float rotation) { this->rotation.z = rotation; UpdateViewMat(); }

		virtual inline const glm::mat4& GetViewMat() const { return viewMat; }
		virtual inline const glm::mat4& GetViewProjectionMat() const { return viewProjectionMat; }
		virtual inline const glm::mat4& GetProjectionMat() const { return projectionMat; }

	protected:
		void UpdateViewMat();

	protected:
		glm::mat4 projectionMat{ 1.f };
		glm::mat4 viewMat;
		glm::mat4 viewProjectionMat{ 1.f };

		glm::vec3 position;
		glm::vec3 rotation = { 0.f, 0.f, 0.f };
	};
}
#pragma once

#include <vendor/glm/glm.hpp>

namespace Cast::Renderer {
	class Camera {
	public:
		Camera();
		virtual ~Camera() = default;

		virtual void LookAt(const glm::vec3& target, const glm::vec3& up = { 0.f, 1.f, 0.f });

		virtual const glm::vec3& GetPosition() const { return m_Rotation; }
		virtual void SetPosition(const glm::vec3& position) { m_Position = position; UpdateViewMat(); }

		virtual float GetZRotation() const { return m_Rotation.z; }
		virtual const glm::vec3& GetRotation() const { return m_Rotation; }
		virtual void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; UpdateViewMat(); }
		virtual void SetZRotation(float rotation) { m_Rotation.z = rotation; UpdateViewMat(); }

		virtual const glm::mat4& GetProjectionMat() const { return m_ProjectionMat; }
		virtual const glm::mat4& GetViewMat() const { return m_ViewMat; }
		virtual const glm::mat4& GetViewProjectionMat() const { return m_ViewProjectionMat; }

	protected:
		void UpdateViewMat();

	protected:
		glm::mat4 m_ProjectionMat;
		glm::mat4 m_ViewMat;
		glm::mat4 m_ViewProjectionMat;

		glm::vec3 m_Position;
		glm::vec3 m_Rotation = { 0.f, 0.f, 0.f };
	};
}
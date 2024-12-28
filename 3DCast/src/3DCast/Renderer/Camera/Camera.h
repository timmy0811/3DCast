#pragma once

#include <vendor/glm/glm.hpp>

namespace Cast::Renderer {
	class Camera {
	public:
		enum class Type {
			Perspective,
			Orthographic
		};

		Camera();
		virtual ~Camera() = default;

		inline Type GetType() const { return ProjectionType; }
		virtual void LookAt(const glm::vec3& target, const glm::vec3& up = { 0.f, 1.f, 0.f });

		virtual inline const glm::vec3& GetPosition() const { return Position; }
		virtual inline void SetPosition(const glm::vec3& position) { this->Position = position; UpdateViewMat(); }

		virtual inline const float GetRoll() const { return Rotation.z; }
		virtual inline const float GetPitch() const { return Rotation.x; }
		virtual inline const float GetYaw() const { return Rotation.y; }

		virtual inline const glm::vec3& GetRotation() const { return Rotation; }

		virtual inline void SetRoll(float rotation) { this->Rotation.z = rotation; UpdateViewMat(); }
		virtual inline void SetPitch(float rotation) { this->Rotation.x = glm::clamp(rotation, -89.99f, 89.99f); UpdateViewMat(); }
		virtual inline void SetYaw(float rotation) { this->Rotation.y = rotation; UpdateViewMat(); }
		virtual inline void SetRotation(const glm::vec3& rotation) { this->Rotation = rotation; this->Rotation.x = glm::clamp(rotation.x, -89.99f, 89.99f); UpdateViewMat(); }

		virtual inline const glm::vec3& GetForward() const { return Forward; }
		virtual inline const glm::vec3& GetRight() const { return Right; }
		virtual inline const glm::vec3& GetWorldUp() const { return WorldUp; }

		virtual inline const glm::mat4& GetViewMat() const { return ViewMat; }
		virtual inline const glm::mat4& GetViewProjectionMat() const { return ViewProjectionMat; }
		virtual inline const glm::mat4& GetProjectionMat() const { return ProjectionMat; }

	protected:
		void UpdateViewMat();

	protected:
		Type ProjectionType;
		glm::mat4 ProjectionMat{ 1.f };
		glm::mat4 ViewMat;
		glm::mat4 ViewProjectionMat{ 1.f };

		glm::vec3 Position;
		glm::vec3 Rotation = { 0.f, -90.f, 0.f }; // pitch, yaw, roll

		glm::vec3 WorldUp = { 0.f, 1.f, 0.f };

		glm::vec3 Up = { 0.f, 0.f, 0.f };
		glm::vec3 Forward = { 0.f, 0.f, -1.f };
		glm::vec3 Right = { 0.f, 0.f, 0.f };
	};
}
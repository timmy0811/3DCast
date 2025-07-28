#pragma once

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/type_ptr.hpp>

namespace Cast::Renderer
{
	class Camera
	{
	public:
		enum class Type
		{
			Perspective,
			Orthographic
		};

		Camera();
		virtual ~Camera() = default;

		virtual void LookAt(const glm::vec3& target, const glm::vec3& up = {0.f, 1.f, 0.f});

		virtual void MakeConsistentViewMatBase();

		virtual inline void SetPosition(const glm::vec3& position)
		{
			if (Position == position) return;

			this->Position = position;
			UpdateViewMat();
		}

		virtual inline void SetRoll(const float rotation)
		{
			if (this->Rotation.z == rotation) return;

			this->Rotation.z = rotation;
			UpdateViewMat();
		}

		virtual inline void SetPitch(const float rotation)
		{
			const float pitch = glm::clamp(rotation, -89.99f, 89.99f);
			if (this->Rotation.x == pitch) return;

			this->Rotation.x = glm::clamp(rotation, -89.99f, 89.99f);
			UpdateViewMat();
		}

		virtual inline void SetYaw(const float rotation)
		{
			if (this->Rotation.y == rotation) return;
			this->Rotation.y = rotation;
			UpdateViewMat();
		}

		virtual inline void SetRotation(const glm::vec3& rotation)
		{
			if (this->Rotation == rotation) return;
			this->Rotation = rotation;
			this->Rotation.x = glm::clamp(rotation.x, -89.99f, 89.99f);
			UpdateViewMat();
		}

		inline bool HasChanged(const int consumerChannel = 0)
		{
			const bool changed = HasChangedField[consumerChannel];
			HasChangedField[consumerChannel] = false;
			return changed;
		}

		[[nodiscard]] inline Type GetType() const { return ProjectionType; }
		[[nodiscard]] virtual inline const glm::vec3& GetPosition() const { return Position; }

		[[nodiscard]]virtual inline float* GetViewMatValuePtr() { return glm::value_ptr(ViewMat); }

		[[nodiscard]] virtual inline float GetRoll() const { return Rotation.z; }
		[[nodiscard]] virtual inline float GetPitch() const { return Rotation.x; }
		[[nodiscard]] virtual inline float GetYaw() const { return Rotation.y; }

		[[nodiscard]] virtual inline const glm::vec3& GetRotation() const { return Rotation; }

		[[nodiscard]] virtual inline const glm::vec3& GetForward() const { return Forward; }
		[[nodiscard]] virtual inline const glm::vec3& GetRight() const { return Right; }
		[[nodiscard]] virtual inline const glm::vec3& GetWorldUp() const { return WorldUp; }

		[[nodiscard]] virtual inline const glm::mat4& GetViewMat() const { return ViewMat; }
		[[nodiscard]] virtual inline const glm::mat4& GetViewProjectionMat() const { return ViewProjectionMat; }
		[[nodiscard]] virtual inline const glm::mat4& GetProjectionMat() const { return ProjectionMat; }

		void UpdateViewMat();
	protected:
		void UpdateDirections();

	protected:
		Type ProjectionType;
		bool HasChangedField[10] = {true};

		glm::mat4 ProjectionMat{1.f};
		glm::mat4 ViewMat{1.f};
		glm::mat4 ViewProjectionMat{1.f};

		glm::vec3 Position;
		glm::vec3 Rotation = {0.f, 0.f, 0.f}; // pitch, yaw, roll
		glm::vec3 Scale = {1.f, 1.f, 1.f}; // Not used in Camera, but kept for consistency

		glm::vec3 WorldUp = {0.f, 1.f, 0.f};

		glm::vec3 Up = {0.f, 0.f, 0.f};
		glm::vec3 Forward = {1.f, 0.f, 0.f};
		glm::vec3 Right = {0.f, 0.f, 0.f};
	};
}

#include "castpch.h"
#include "Camera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>

Cast::Renderer::Camera::Camera()
	: ProjectionType(), Position(0.f, 0.f, 0.f) // near, far?
{
}

auto Cast::Renderer::Camera::LookAt(const glm::vec3& target, const glm::vec3& up) -> void
{
	const glm::vec3 targetVec = glm::normalize(target - Position);
	const glm::quat rotation = glm::quatLookAt(targetVec, up);

	Rotation = glm::degrees(glm::eulerAngles(rotation));
	UpdateViewMat();
}

void Cast::Renderer::Camera::UpdateViewMat()
{
	glm::vec3 fw;
	fw.x = (float)(cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x)));
	fw.y = (float)sin(glm::radians(Rotation.x));
	fw.z = (float)(sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x)));
	Forward = glm::normalize(fw);

	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Right, Forward));

	ViewMat = glm::lookAt(Position, Position + Forward, Up);
	ViewProjectionMat = ProjectionMat * ViewMat;

	memset(HasChangedField, true, sizeof(HasChangedField));
}

#include "castpch.h"
#include "Camera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>

Cast::Renderer::Camera::Camera()
	: ViewMat(1.f), Position(0.f, 0.f, 0.f) // near, far?
{
}

void Cast::Renderer::Camera::LookAt(const glm::vec3& target, const glm::vec3& up)
{
	glm::vec3 targetVec = glm::normalize(target - Position);
	glm::quat rotation = glm::quatLookAt(targetVec, up);

	rotation = glm::degrees(glm::eulerAngles(rotation));

	UpdateViewMat();
}

void Cast::Renderer::Camera::UpdateViewMat()
{
	glm::mat4 rotated = glm::mat4(glm::quat(glm::radians(glm::vec3(Rotation.x, Rotation.y, Rotation.z))));
	glm::mat4 transform = glm::translate(glm::mat4(1.f), Position) * rotated;

	ViewMat = glm::inverse(transform);
	ViewProjectionMat = ProjectionMat * ViewMat;
}
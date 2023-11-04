#include "castpch.h"
#include "Camera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtx/quaternion.hpp>

Cast::Renderer::Camera::Camera()
	: m_ViewMat(1.f), m_Position(0.f, 0.f, 0.f) // near, far?
{
}

void Cast::Renderer::Camera::LookAt(const glm::vec3& target, const glm::vec3& up)
{
	glm::vec3 targetVec = glm::normalize(target - m_Position);
	glm::quat rotation = glm::quatLookAt(targetVec, up);

	m_Rotation = glm::degrees(glm::eulerAngles(rotation));

	UpdateViewMat();
}

void Cast::Renderer::Camera::UpdateViewMat()
{
	glm::mat4 rotated = glm::mat4(glm::quat(glm::radians(glm::vec3(m_Rotation.x, m_Rotation.y, m_Rotation.z))));
	glm::mat4 transform = glm::translate(glm::mat4(1.f), m_Position) * rotated;

	m_ViewMat = glm::inverse(transform);
	m_ViewProjectionMat = m_ProjectionMat * m_ViewMat;
}

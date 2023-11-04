#include "castpch.h"
#include "PerspectiveCamera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>

Cast::Renderer::PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane)
	: Camera()
{
	m_ProjectionMat = glm::perspective(fov, aspect, nearPlane, farPlane);
	m_ViewProjectionMat = m_ProjectionMat * m_ViewMat;
}

#include "castpch.h"
#include "OrthographicCamera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>

Cast::Renderer::OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
	: Camera()
{
	m_ProjectionMat = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	m_ViewProjectionMat = m_ProjectionMat * m_ViewMat;
}

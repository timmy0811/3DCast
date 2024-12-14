#include "castpch.h"
#include "PerspectiveCamera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>

Cast::Renderer::PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane)
	: Camera()
{
	projectionMat = glm::perspective(fov, aspect, nearPlane, farPlane);
	viewProjectionMat = projectionMat * viewMat;
}

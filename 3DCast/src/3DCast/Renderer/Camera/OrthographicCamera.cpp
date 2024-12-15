#include "castpch.h"
#include "OrthographicCamera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>

Cast::Renderer::OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
	: Camera()
{
	ProjectionType = Type::Orthographic;
	ProjectionMat = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	ViewProjectionMat = ProjectionMat * ViewMat;
}

void Cast::Renderer::OrthographicCamera::SetFrustumOnResized(float width, float height)
{
	float aspect = width / height;
	ProjectionMat = glm::ortho(-aspect, aspect, -1.f, 1.f, -1.f, 1.f);
	ViewProjectionMat = ProjectionMat * ViewMat;
}
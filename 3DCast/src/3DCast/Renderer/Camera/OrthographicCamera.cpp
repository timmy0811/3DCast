#include "castpch.h"
#include "OrthographicCamera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>

Cast::Renderer::OrthographicCamera::OrthographicCamera(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane)
{
	ProjectionType = Type::Orthographic;
	ProjectionMat = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	ViewProjectionMat = ProjectionMat * ViewMat;

	memset(HasChangedField, true, sizeof(HasChangedField));
}

void Cast::Renderer::OrthographicCamera::SetFrustumOnResized(const float width, const float height)
{
	const float aspect = width / height;
	ProjectionMat = glm::ortho(-aspect, aspect, -1.f, 1.f, -1.f, 1.f);
	ViewProjectionMat = ProjectionMat * ViewMat;

	memset(HasChangedField, true, sizeof(HasChangedField));
}
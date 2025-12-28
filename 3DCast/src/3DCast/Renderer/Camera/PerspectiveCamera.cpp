#include "castpch.h"
#include "PerspectiveCamera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>

Cast::Renderer::PerspectiveCamera::PerspectiveCamera(const float fov, const float aspect, const float nearPlane, const float farPlane)
	: Fov(fov), NearPlane(nearPlane), FarPlane(farPlane)
{
	ProjectionType = Type::Perspective;
	ProjectionMat = glm::perspective(fov, aspect, nearPlane, farPlane);
	ViewProjectionMat = ProjectionMat * ViewMat;

	memset(HasChangedField, true, sizeof(HasChangedField));
}

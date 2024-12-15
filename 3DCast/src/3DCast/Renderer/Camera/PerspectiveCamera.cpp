#include "castpch.h"
#include "PerspectiveCamera.h"

#include <vendor/glm/gtc/matrix_transform.hpp>

Cast::Renderer::PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane)
	: Camera(), Fov(fov), NearPlane(nearPlane), FarPlane(farPlane)
{
	ProjectionType = Type::Perspective;
	ProjectionMat = glm::perspective(fov, aspect, nearPlane, farPlane);
	ViewProjectionMat = ProjectionMat * ViewMat;
}

void Cast::Renderer::PerspectiveCamera::SetAspectRatio(float aspectRatio)
{
	ProjectionMat = glm::perspective(Fov, aspectRatio, NearPlane, FarPlane);
	ViewProjectionMat = ProjectionMat * ViewMat;
}
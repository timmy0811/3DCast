#pragma once

#include <Vendor/glm/glm.hpp>

// Structures layout must match shader structs
namespace Cast {
	struct AbstractLight {};

	struct DirectionalLight : public AbstractLight
	{
		alignas(16) glm::vec3 direction = { -0.2f, -1.0f, -0.3f };
		alignas(16) glm::vec3 ambient = { 0.1f, 0.1f, 0.1f };
		alignas(16) glm::vec3 diffuse = { 0.5f, 0.5f, 0.5f };
		alignas(16) glm::vec3 specular = { 1.f, 1.0f, 1.0f };
	};

	struct PointLight : public AbstractLight
	{
		alignas(16) float affectedRadius = 1.f;

		alignas(16) glm::vec3 position = { 0.f, 2.f, 0.f };
		alignas(16) glm::vec3 ambient = { 0.1f, 0.1f, 0.1f };
		alignas(16) glm::vec3 diffuse = { 0.5f, 0.5f, 0.5f };
		alignas(16) glm::vec3 specular = { 1.f, 1.0f, 1.0f };

		float constant = 1.f;
		float linear = 0.09f;
		float quadratic = 0.032f;
	};

	struct SpotLight : public AbstractLight
	{
		alignas(16) glm::vec3 position = { 0.f, 2.f, 0.f };
		alignas(16) glm::vec3 direction = { -0.2f, -1.0f, -0.3f };

		alignas(16) glm::vec3 ambient = { 0.1f, 0.1f, 0.1f };
		alignas(16) glm::vec3 diffuse = { 0.8f, 0.8f, 0.8f };
		alignas(16) glm::vec3 specular = { 1.0f, 1.0f, 1.0f };

		float constant = 1.f;
		float linear = 0.09f;
		float quadratic = 0.032f;

		float cutOff = glm::cos(glm::radians(55.f));
		float outerCutOff = glm::cos(glm::radians(50.f));
	};
}
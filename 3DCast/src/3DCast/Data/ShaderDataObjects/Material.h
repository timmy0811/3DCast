#pragma once

#include <vendor/glm/glm.hpp>

// Structures layout must match shader structs
namespace Cast
{
	struct CustomMaterialShaderObject {
	    alignas(16) glm::vec3 diffuseColor = glm::vec3(0.9f, 0.0f, 0.8f);
	    alignas(16) glm::vec3 specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
	    alignas(16) glm::vec3 emissiveColor = glm::vec3(0.0f, 0.0f, 0.0f);

	    float metallic = 0.0f;
	    float roughness = 0.5f;
	    float shininess = 32.0f;
	    float reflectance = 0.5f;
    };
}

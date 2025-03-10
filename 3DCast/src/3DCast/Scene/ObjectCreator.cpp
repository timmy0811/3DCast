#include "castpch.h"
#include "ObjectCreator.h"

#include "Component/Component.h"
#include "Entity.h"

#include "SceneShaderCache.h"

Cast::Entity Cast::Create::Cube(const std::string& name, Cast::Scene* scene)
{
	// Cube
	Cast::Entity entity = scene->CreateEntity(name);
	entity.AddComponents<Cast::Component::CustomMeshComponent>();
	entity.AddComponents<Cast::Component::RasterizableComponent>();
	entity.AddComponents<Cast::Component::MaterialComponent>();

	float trIndex = (float)entity.GetComponent<Component::TransformComponent>().bufferIndex;
	float samplerIndex = (float)entity.GetComponent<Component::MaterialComponent>().samplerIndex;

	float vertices[] = {
		// Position				// Tangent				// Bitangent				// UVs			// SamplerIndex	// TransformIndex
		// back face
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-left
		1.0f,  1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-right
		1.0f,  1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-left
		-1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// top-left
		// front face
		-1.0f, -1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-left
		1.0f, -1.0f,  1.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-right
		1.0f,  1.0f,  1.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		1.0f,  1.0f,  1.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		-1.0f,  1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// top-left
		-1.0f, -1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-left
		// left face
		-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// top-right
		-1.0f,  1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-left
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// bottom-left
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// bottom-left
		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-right
		-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// top-right
		// right face
		1.0f,  1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// top-left
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// bottom-right
		1.0f,  1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// bottom-right
		1.0f,  1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// top-left
		1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-left
		// bottom face
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-left
		1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-left
		1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-left
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-right
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		// top face
		-1.0f,  1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// top-left
		1.0f,  1.0f,  1.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-right
		1.0f,  1.0f, -1.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		1.0f,  1.0f,  1.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-right
		-1.0f,  1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// top-left
		-1.0f,  1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		samplerIndex,	trIndex			// bottom-left
	};

	Cast::Component::CustomMeshComponent& cubeMesh = entity.GetComponent<Cast::Component::CustomMeshComponent>();
	cubeMesh.AddAndAllocVertexData(vertices, sizeof(vertices));
	cubeMesh.AddToBatchMemory();

	return entity;
}

Cast::Entity Cast::Create::Plane(const std::string& name, Cast::Scene* scene)
{
	Cast::Entity entity = scene->CreateEntity(name);
	entity.AddComponents<Cast::Component::CustomMeshComponent>();
	entity.AddComponents<Cast::Component::RasterizableComponent>();
	entity.AddComponents<Cast::Component::MaterialComponent>();

	float trIndex = (float)entity.GetComponent<Component::TransformComponent>().bufferIndex;
	float samplerIndex = (float)entity.GetComponent<Cast::Component::MaterialComponent>().samplerIndex;

	float vertices[] = {
		// Position				// Tangent				// Bitangent				// UVs			// SamplerIndex	// TransformIndex
		-1.0f,  0.0f, -1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// top-left
		1.0f,  0.0f,  1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-right
		1.0f,  0.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		samplerIndex,	trIndex,		// top-right
		1.0f,  0.0f,  1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		samplerIndex,	trIndex,		// bottom-right
		-1.0f,  0.0f, -1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,		samplerIndex,	trIndex,		// top-left
		-1.0f,  0.0f,  1.0f,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		samplerIndex,	trIndex			// bottom-left
	};

	Cast::Component::CustomMeshComponent& cubeMesh = entity.GetComponent<Cast::Component::CustomMeshComponent>();
	cubeMesh.AddAndAllocVertexData(vertices, sizeof(vertices));
	cubeMesh.AddToBatchMemory();

	return entity;
}
#include "castpch.h"
#include "ObjectCreator.h"

#include "Components.h"
#include "Entity.h"

#include "SceneShaderCache.h"

Cast::Entity Cast::Create::Cube(const std::string& name, Cast::Scene* scene, int texIndex, float shine, float reflectance)
{
	// Cube
	Cast::Entity entity = scene->CreateEntity(name);
	entity.AddComponents<Cast::Component::CustomMeshComponent>();
	entity.AddComponents<Cast::Component::RasterizableComponent>();
	entity.AddComponents<Cast::Component::MaterialComponent>(Cast::AssetCache.GetShaderId("shader_geometry_pass")); // Temporary

	float trIndex = (float)entity.GetComponent<Component::TransformComponent>().bufferIndex;
	float tIndex = (float)texIndex;

	float vertices[36 * 9] = {
		// Position				// Normals				// UVs          // TransformIndex
		// back face
		-1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,		trIndex,	// bottom-left
		1.0f,  1.0f, -1.0f,		0.0f,  0.0f, -1.0f,		1.0f, 1.0f,		trIndex,	// top-right
		1.0f, -1.0f, -1.0f,		0.0f,  0.0f, -1.0f,		1.0f, 0.0f,		trIndex,	// bottom-right
		1.0f,  1.0f, -1.0f,		0.0f,  0.0f, -1.0f,		1.0f, 1.0f,		trIndex,	// top-right
		-1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,		trIndex,	// bottom-left
		-1.0f,  1.0f, -1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 1.0f,		trIndex,	// top-left
		// front face
		-1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,		0.0f, 0.0f,		trIndex,	// bottom-left
		1.0f, -1.0f,  1.0f,		0.0f,  0.0f,  1.0f,		1.0f, 0.0f,		trIndex,	// bottom-right
		1.0f,  1.0f,  1.0f,		0.0f,  0.0f,  1.0f,		1.0f, 1.0f,		trIndex,	// top-right
		1.0f,  1.0f,  1.0f,		0.0f,  0.0f,  1.0f,		1.0f, 1.0f,		trIndex,	// top-right
		-1.0f,  1.0f,  1.0f,	0.0f,  0.0f,  1.0f,		0.0f, 1.0f,		trIndex,	// top-left
		-1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,		0.0f, 0.0f,		trIndex,	// bottom-left
		// left face
		-1.0f,  1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		trIndex,	// top-right
		-1.0f,  1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		trIndex,	// top-left
		-1.0f, -1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		trIndex,	// bottom-left
		-1.0f, -1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		trIndex,	// bottom-left
		-1.0f, -1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		trIndex,	// bottom-right
		-1.0f,  1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		trIndex,	// top-right
		// right face
		1.0f,  1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		trIndex,	// top-left
		1.0f, -1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		trIndex,	// bottom-right
		1.0f,  1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 1.0f,		trIndex,	// top-right
		1.0f, -1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		trIndex,	// bottom-right
		1.0f,  1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		trIndex,	// top-left
		1.0f, -1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 0.0f,		trIndex,	// bottom-left
		// bottom face
		-1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,		trIndex,	// top-right
		1.0f, -1.0f, -1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 1.0f,		trIndex,	// top-left
		1.0f, -1.0f,  1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,		trIndex,	// bottom-left
		1.0f, -1.0f,  1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,		trIndex,	// bottom-left
		-1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,		trIndex,	// bottom-right
		-1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,		trIndex,	// top-right
		// top face
		-1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		trIndex,	// top-left
		1.0f,  1.0f , 1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		trIndex,	// bottom-right
		1.0f,  1.0f, -1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f,		trIndex,	// top-right
		1.0f,  1.0f,  1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		trIndex,	// bottom-right
		-1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		trIndex,	// top-left
		-1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f, 	trIndex,	// bottom-left
	};

	Cast::Component::CustomMeshComponent& cubeMesh = entity.GetComponent<Cast::Component::CustomMeshComponent>();
	cubeMesh.AddAndAllocVertexData(vertices, sizeof(vertices));
	cubeMesh.AddToBatchMemory();

	return entity;
}

Cast::Entity Cast::Create::Plane(const std::string& name, Cast::Scene* scene)
{
	return Cast::Entity();
}
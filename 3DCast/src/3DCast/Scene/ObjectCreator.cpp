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

	float vertices[36 * 12] = {
		// Position				// Normals				// UVs          // TexIndex	& TransformIndex // Shine & Reflectance
		// back face
		-1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		1.0f,  1.0f, -1.0f,		0.0f,  0.0f, -1.0f,		1.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		1.0f, -1.0f, -1.0f,		0.0f,  0.0f, -1.0f,		1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-right
		1.0f,  1.0f, -1.0f,		0.0f,  0.0f, -1.0f,		1.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		-1.0f, -1.0f, -1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		-1.0f,  1.0f, -1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,		0.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		1.0f, -1.0f,  1.0f,		0.0f,  0.0f,  1.0f,		1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-right
		1.0f,  1.0f,  1.0f,		0.0f,  0.0f,  1.0f,		1.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		1.0f,  1.0f,  1.0f,		0.0f,  0.0f,  1.0f,		1.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		-1.0f,  1.0f,  1.0f,	0.0f,  0.0f,  1.0f,		0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-left
		-1.0f, -1.0f,  1.0f,	0.0f,  0.0f,  1.0f,		0.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		-1.0f,  1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-left
		-1.0f, -1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		-1.0f, -1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		-1.0f, -1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-right
		-1.0f,  1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		// right face
		1.0f,  1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // top-left
		1.0f, -1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // bottom-right
		1.0f,  1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		1.0f, -1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // bottom-right
		1.0f,  1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // top-left
		1.0f, -1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		// bottom face
		-1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		1.0f, -1.0f, -1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-left
		1.0f, -1.0f,  1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		1.0f, -1.0f,  1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-left
		-1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-right
		-1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-left
		1.0f,  1.0f , 1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-right
		1.0f,  1.0f, -1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-right
		1.0f,  1.0f,  1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		tIndex, trIndex,	shine, reflectance, // bottom-right
		-1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		tIndex, trIndex,	shine, reflectance, // top-left
		-1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f, 	tIndex, trIndex,	shine, reflectance// bottom-left
	};

	Cast::Component::CustomMeshComponent& cubeMesh = entity.GetComponent<Cast::Component::CustomMeshComponent>();

	cubeMesh.vb.reset(API::Core::VertexBuffer::Create(vertices, sizeof(vertices)));

	cubeMesh.vbLayout.reset(API::Core::VertexBufferLayout::Create());
	cubeMesh.vbLayout->Push(API::Core::ShaderDataType::Float3);
	cubeMesh.vbLayout->Push(API::Core::ShaderDataType::Float3);
	cubeMesh.vbLayout->Push(API::Core::ShaderDataType::Float2);
	cubeMesh.vbLayout->Push(API::Core::ShaderDataType::Float);
	cubeMesh.vbLayout->Push(API::Core::ShaderDataType::Float);
	cubeMesh.vbLayout->Push(API::Core::ShaderDataType::Float2);

	cubeMesh.va.reset(API::Core::VertexArray::Create());
	cubeMesh.va->AddBuffer(*(cubeMesh.vb), *(cubeMesh.vbLayout));
	cubeMesh.va->SetVBCount(36);

	return entity;
}

Cast::Entity Cast::Create::Plane(const std::string& name, Cast::Scene* scene)
{
	return Cast::Entity();
}
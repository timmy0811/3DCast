#include "castpch.h"
#include "ObjectCreator.h"

#include "Component/Component.h"
#include "Entity.h"
#include "3DCast/Data/ShaderDataObjects/Vertex.h"

Cast::Ref<Cast::Entity> Cast::Create::Cube(const std::string& name, Scene* scene)
{
	// Cube
	Ref<Entity> entity = scene->CreateEntity(name, true);
	entity->AddComponents<Component::CustomMeshComponent>();
	entity->AddComponents<Component::RasterizableComponent>();
	entity->AddComponents<Component::MaterialComponent>();

	auto& transformComp = entity->GetComponent<Component::TransformComponent>();
	auto trIndex = (float)transformComp.transformRegistryIndex;
	auto samplerIndex = (float)entity->GetComponent<Component::MaterialComponent>().samplerIndex;

	std::pair<glm::vec3, glm::vec3> t0Back = GetTangentAndBitangent(glm::vec3(-1.f, -1.f, -1.f),
	                                                                glm::vec3(1.f, 1.f, -1.f),
	                                                                glm::vec3(1.f, -1.f, -1.f), glm::vec2(0.f, 0.f),
	                                                                glm::vec2(1.f, 1.f), glm::vec2(1.f, 0.f));
	std::pair<glm::vec3, glm::vec3> t1Back = GetTangentAndBitangent(glm::vec3(1.f, 1.f, -1.f),
	                                                                glm::vec3(-1.f, -1.f, -1.f),
	                                                                glm::vec3(-1.f, 1.f, -1.f), glm::vec2(1.f, 1.f),
	                                                                glm::vec2(0.f, 0.f), glm::vec2(0.f, 1.f));

	std::pair<glm::vec3, glm::vec3> t0Front = GetTangentAndBitangent(glm::vec3(-1.f, -1.f, 1.f),
	                                                                 glm::vec3(1.f, 1.f, 1.f),
	                                                                 glm::vec3(1.f, -1.f, 1.f), glm::vec2(0.f, 0.f),
	                                                                 glm::vec2(1.f, 1.f), glm::vec2(1.f, 0.f));
	std::pair<glm::vec3, glm::vec3> t1Front = GetTangentAndBitangent(glm::vec3(1.f, 1.f, 1.f),
	                                                                 glm::vec3(-1.f, -1.f, 1.f),
	                                                                 glm::vec3(-1.f, 1.f, 1.f), glm::vec2(1.f, 1.f),
	                                                                 glm::vec2(0.f, 0.f), glm::vec2(0.f, 1.f));

	std::pair<glm::vec3, glm::vec3> t0Left = GetTangentAndBitangent(glm::vec3(-1.f, 1.f, 1.f),
	                                                                glm::vec3(-1.f, 1.f, -1.f),
	                                                                glm::vec3(-1.f, -1.f, -1.f), glm::vec2(1.f, 1.f),
	                                                                glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f));
	std::pair<glm::vec3, glm::vec3> t1Left = GetTangentAndBitangent(glm::vec3(-1.f, -1.f, -1.f),
	                                                                glm::vec3(-1.f, -1.f, 1.f),
	                                                                glm::vec3(-1.f, 1.f, 1.f), glm::vec2(0.f, 0.f),
	                                                                glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f));

	std::pair<glm::vec3, glm::vec3> t0Right = GetTangentAndBitangent(glm::vec3(1.f, 1.f, 1.f),
	                                                                 glm::vec3(1.f, -1.f, -1.f),
	                                                                 glm::vec3(1.f, 1.f, -1.f), glm::vec2(0.f, 1.f),
	                                                                 glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f));
	std::pair<glm::vec3, glm::vec3> t1Right = GetTangentAndBitangent(glm::vec3(1.f, -1.f, -1.f),
	                                                                 glm::vec3(1.f, 1.f, 1.f),
	                                                                 glm::vec3(1.f, -1.f, 1.f), glm::vec2(1.f, 0.f),
	                                                                 glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f));

	std::pair<glm::vec3, glm::vec3> t0Bottom = GetTangentAndBitangent(glm::vec3(-1.f, -1.f, -1.f),
	                                                                  glm::vec3(1.f, -1.f, -1.f),
	                                                                  glm::vec3(1.f, -1.f, 1.f), glm::vec2(0.f, 0.f),
	                                                                  glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f));
	std::pair<glm::vec3, glm::vec3> t1Bottom = GetTangentAndBitangent(glm::vec3(1.f, -1.f, 1.f),
	                                                                  glm::vec3(-1.f, -1.f, 1.f),
	                                                                  glm::vec3(-1.f, -1.f, -1.f), glm::vec2(1.f, 1.f),
	                                                                  glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f));

	std::pair<glm::vec3, glm::vec3> t0Top = GetTangentAndBitangent(glm::vec3(-1.f, 1.f, -1.f), glm::vec3(1.f, 1.f, 1.f),
	                                                               glm::vec3(1.f, 1.f, -1.f), glm::vec2(0.f, 1.f),
	                                                               glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f));
	std::pair<glm::vec3, glm::vec3> t1Top = GetTangentAndBitangent(glm::vec3(1.f, 1.f, 1.f), glm::vec3(-1.f, 1.f, -1.f),
	                                                               glm::vec3(-1.f, 1.f, 1.f), glm::vec2(1.f, 0.f),
	                                                               glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f));

	Memory::BatchVertex vertices[] = {
		// Position				// Normal			// Tangent			// Bitangent		// UVs			// SamplerIndex	// TransformIndex
		// back face
		{{-1.f, -1.f, -1.f}, {0.f, 0.f, -1.f}, t0Back.first, t0Back.second, {0.f, 0.f}, samplerIndex, trIndex},
		// bottom-left
		{{1.f, 1.f, -1.f}, {0.f, 0.f, -1.f}, t0Back.first, t0Back.second, {1.f, 1.f}, samplerIndex, trIndex},
		// top-right
		{{1.f, -1.f, -1.f}, {0.f, 0.f, -1.f}, t0Back.first, t0Back.second, {1.f, 0.f}, samplerIndex, trIndex},
		// bottom-right
		{{1.f, 1.f, -1.f}, {0.f, 0.f, -1.f}, t1Back.first, t1Back.second, {1.f, 1.f}, samplerIndex, trIndex},
		// top-right
		{{-1.f, -1.f, -1.f}, {0.f, 0.f, -1.f}, t1Back.first, t1Back.second, {0.f, 0.f}, samplerIndex, trIndex},
		// bottom-left
		{{-1.f, 1.f, -1.f}, {0.f, 0.f, -1.f}, t1Back.first, t1Back.second, {0.f, 1.f}, samplerIndex, trIndex},
		// top-left

		// front face
		{{-1.f, -1.f, 1.f}, {0.f, 0.f, 1.f}, t0Front.first, t0Front.second, {0.f, 0.f}, samplerIndex, trIndex},
		// bottom-left
		{{1.f, -1.f, 1.f}, {0.f, 0.f, 1.f}, t0Front.first, t0Front.second, {1.f, 0.f}, samplerIndex, trIndex},
		// top-right
		{{1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}, t0Front.first, t0Front.second, {1.f, 1.f}, samplerIndex, trIndex},
		// bottom-right
		{{1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}, t1Front.first, t1Front.second, {1.f, 1.f}, samplerIndex, trIndex},
		// top-right
		{{-1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}, t1Front.first, t1Front.second, {0.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{-1.f, -1.f, 1.f}, {0.f, 0.f, 1.f}, t1Front.first, t1Front.second, {0.f, 0.f}, samplerIndex, trIndex},
		// top-left

		// left face
		{{-1.f, 1.f, 1.f}, {-1.f, 0.f, 0.f}, t0Left.first, t0Left.second, {1.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{-1.f, 1.f, -1.f}, {-1.f, 0.f, 0.f}, t0Left.first, t0Left.second, {0.f, 1.f}, samplerIndex, trIndex},
		// top-right
		{{-1.f, -1.f, -1.f}, {-1.f, 0.f, 0.f}, t0Left.first, t0Left.second, {0.f, 0.f}, samplerIndex, trIndex},
		// bottom-right
		{{-1.f, -1.f, -1.f}, {-1.f, 0.f, 0.f}, t1Left.first, t1Left.second, {0.f, 0.f}, samplerIndex, trIndex},
		// top-right
		{{-1.f, -1.f, 1.f}, {-1.f, 0.f, 0.f}, t1Left.first, t1Left.second, {1.f, 0.f}, samplerIndex, trIndex},
		// bottom-left
		{{-1.f, 1.f, 1.f}, {-1.f, 0.f, 0.f}, t1Left.first, t1Left.second, {1.f, 1.f}, samplerIndex, trIndex},
		// top-left

		// right face
		{{1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}, t0Right.first, t0Right.second, {0.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{1.f, -1.f, -1.f}, {1.f, 0.f, 0.f}, t0Right.first, t0Right.second, {1.f, 0.f}, samplerIndex, trIndex},
		// top-right
		{{1.f, 1.f, -1.f}, {1.f, 0.f, 0.f}, t0Right.first, t0Right.second, {1.f, 1.f}, samplerIndex, trIndex},
		// bottom-right
		{{1.f, -1.f, -1.f}, {1.f, 0.f, 0.f}, t1Right.first, t1Right.second, {1.f, 0.f}, samplerIndex, trIndex},
		// top-right
		{{1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}, t1Right.first, t1Right.second, {0.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{1.f, -1.f, 1.f}, {1.f, 0.f, 0.f}, t1Right.first, t1Right.second, {0.f, 0.f}, samplerIndex, trIndex},
		// top-left

		// bottom face
		{{-1.f, -1.f, -1.f}, {0.f, -1.f, 0.f}, t0Bottom.first, t0Bottom.second, {0.f, 0.f}, samplerIndex, trIndex},
		// bottom-left
		{{1.f, -1.f, -1.f}, {0.f, -1.f, 0.f}, t0Bottom.first, t0Bottom.second, {1.f, 0.f}, samplerIndex, trIndex},
		// top-right
		{{1.f, -1.f, 1.f}, {0.f, -1.f, 0.f}, t0Bottom.first, t0Bottom.second, {1.f, 1.f}, samplerIndex, trIndex},
		// bottom-right
		{{1.f, -1.f, 1.f}, {0.f, -1.f, 0.f}, t1Bottom.first, t1Bottom.second, {1.f, 1.f}, samplerIndex, trIndex},
		// top-right
		{{-1.f, -1.f, 1.f}, {0.f, -1.f, 0.f}, t1Bottom.first, t1Bottom.second, {0.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{-1.f, -1.f, -1.f}, {0.f, -1.f, 0.f}, t1Bottom.first, t1Bottom.second, {0.f, 0.f}, samplerIndex, trIndex},
		// top-left

		// top face
		{{-1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}, t0Top.first, t0Top.second, {0.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{1.f, 1.f, 1.f}, {0.f, 1.f, 0.f}, t0Top.first, t0Top.second, {1.f, 0.f}, samplerIndex, trIndex}, // top-right
		{{1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}, t0Top.first, t0Top.second, {1.f, 1.f}, samplerIndex, trIndex},
		// bottom-right
		{{1.f, 1.f, 1.f}, {0.f, 1.f, 0.f}, t1Top.first, t1Top.second, {1.f, 0.f}, samplerIndex, trIndex}, // top-right
		{{-1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}, t1Top.first, t1Top.second, {0.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{-1.f, 1.f, 1.f}, {0.f, 1.f, 0.f}, t1Top.first, t1Top.second, {0.f, 0.f}, samplerIndex, trIndex}, // top-left
	};

	auto& cubeMesh = entity->GetComponent<Component::CustomMeshComponent>();
	cubeMesh.AddAndAllocVertexData(&vertices[0].Position.x, sizeof(vertices));
	cubeMesh.AddToBatchMemory();

	return entity;
}

Cast::Ref<Cast::Entity> Cast::Create::Plane(const std::string& name, Scene* scene)
{
	// Cube
	Ref<Entity> entity = scene->CreateEntity(name, true);
	entity->AddComponents<Component::CustomMeshComponent>();
	entity->AddComponents<Component::RasterizableComponent>();
	entity->AddComponents<Component::MaterialComponent>();

	const auto& transformComp = entity->GetComponent<Component::TransformComponent>();
	const auto trIndex = (float)transformComp.transformRegistryIndex;
	const auto samplerIndex = (float)entity->GetComponent<Component::MaterialComponent>().samplerIndex;

	std::pair<glm::vec3, glm::vec3> t0Top = GetTangentAndBitangent(glm::vec3(-3.f, -1.f, -3.f), glm::vec3(3.f, -1.f, 3.f),
	                                                               glm::vec3(3.f, -1.f, -3.f), glm::vec2(0.f, 3.f),
	                                                               glm::vec2(3.f, 0.f), glm::vec2(3.f, 3.f));
	std::pair<glm::vec3, glm::vec3> t1Top = GetTangentAndBitangent(glm::vec3(3.f, -1.f, 3.f), glm::vec3(-3.f, -1.f, -3.f),
	                                                               glm::vec3(-3.f, -1.f, 3.f), glm::vec2(3.f, 0.f),
	                                                               glm::vec2(0.f, 3.f), glm::vec2(0.f, 0.f));

	const Memory::BatchVertex vertices[] = {
		// Position				// Normal			// Tangent			// Bitangent		// UVs			// SamplerIndex	// TransformIndex
		{{-3.f, -1.f, -3.f}, {0.f, 3.f, 0.f}, t0Top.first, t0Top.second, {0.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{3.f, -1.f, 3.f}, {0.f, 3.f, 0.f}, t0Top.first, t0Top.second, {1.f, 0.f}, samplerIndex, trIndex}, // top-right
		{{3.f, -1.f, -3.f}, {0.f, 3.f, 0.f}, t0Top.first, t0Top.second, {1.f, 1.f}, samplerIndex, trIndex},
		// bottom-right
		{{3.f, -1.f, 3.f}, {0.f, 3.f, 0.f}, t1Top.first, t1Top.second, {1.f, 0.f}, samplerIndex, trIndex}, // top-right
		{{-3.f, -1.f, -3.f}, {0.f, 3.f, 0.f}, t1Top.first, t1Top.second, {0.f, 1.f}, samplerIndex, trIndex},
		// bottom-left
		{{-3.f, -1.f, 3.f}, {0.f, 3.f, 0.f}, t1Top.first, t1Top.second, {0.f, 0.f}, samplerIndex, trIndex}, // top-left
	};

	auto& cubeMesh = entity->GetComponent<Component::CustomMeshComponent>();
	cubeMesh.AddAndAllocVertexData(&vertices[0].Position.x, sizeof(vertices));
	cubeMesh.AddToBatchMemory();

	return entity;
}

std::pair<glm::vec3, glm::vec3> Cast::Create::GetTangentAndBitangent(const glm::vec3& v0, const glm::vec3& v1,
                                                                     const glm::vec3& v2, const glm::vec2& uv0,
                                                                     const glm::vec2& uv1, const glm::vec2& uv2)
{
	const glm::vec3 edge1 = v1 - v0;
	const glm::vec3 edge2 = v2 - v0;
	const glm::vec2 deltaUV1 = uv1 - uv0;
	const glm::vec2 deltaUV2 = uv2 - uv0;

	const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	glm::vec3 tangent;
	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	glm::vec3 bitangent;
	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = glm::normalize(bitangent);

	return std::make_pair(tangent, bitangent);
}

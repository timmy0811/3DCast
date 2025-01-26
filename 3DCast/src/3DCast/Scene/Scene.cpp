#include "castpch.h"
#include "Scene.h"

#include "3DCast/Log.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Scene/Components.h"

#include "3DCast/Renderer/Renderer.h"
#include "3DCast/Scene/SceneShaderCache.h"

#include <Vendor/glm/glm.hpp>

Cast::Scene::Scene()
{
	RegisterComponentImGuiRenderCallback<Component::TagComponent>();
	RegisterComponentImGuiRenderCallback<Component::TransformComponent>();
	RegisterComponentImGuiRenderCallback<Component::LightComponent>();
	RegisterComponentImGuiRenderCallback<Component::MeshComponent>();
	RegisterComponentImGuiRenderCallback<Component::CustomMeshComponent>();
	RegisterComponentImGuiRenderCallback<Component::MaterialComponent>();
	RegisterComponentImGuiRenderCallback<Component::CameraComponent>();
	RegisterComponentImGuiRenderCallback<Component::TextureComponent>();
	RegisterComponentImGuiRenderCallback<Component::ShaderComponent>();
}

Cast::Entity Cast::Scene::CreateEntity(const std::string& name)
{
	Entity entity = { Registry.create(), this };
	entity.AddComponents<Component::TransformComponent>(glm::mat4(1.0f));
	entity.AddComponents<Component::TagComponent>(name);

	return entity;
}

void Cast::Scene::OnUpdate()
{
	auto group = Registry.group<Component::TransformComponent>(entt::get<Component::RasterizableComponent>);
	for (auto entity : group) {
		auto& shader = Cast::AssetCache.GetShaderHandle(Registry.get<Component::MaterialComponent>(entity).Shader);
		auto& transform = Registry.get<Component::TransformComponent>(entity);
		auto& mesh = Registry.get<Component::CustomMeshComponent>(entity);

		Renderer::RendererContext::Submit(mesh.va, mesh.ib, shader, transform.GetTransform());
	}
}
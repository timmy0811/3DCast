#include "castpch.h"
#include "Scene.h"

#include "3DCast/Log.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Scene/Components.h"

#include "3DCast/Renderer/Renderer.h"

#include <Vendor/glm/glm.hpp>

Cast::Scene::Scene()
{
#ifdef sample
	struct TransformComponent {
		glm::mat4 transform;

		TransformComponent() = default;
		TransformComponent(const glm::mat4& transform)
			:transform(transform) {}
	};

	struct StringComponent {
		std::string name;

		StringComponent() = default;
		StringComponent(const std::string& name)
			:name(name) {}
	};

	entt::entity entity = registry.create();
	registry.emplace<StringComponent>(entity, "Test");

	auto& col = registry.get<StringComponent>(entity).name = "Hello!";

	registry.view<StringComponent>().each([](auto entity, auto& stringComponent) {
		CAST_CORE_TRACE("Entity {0} has a string component: {1}", entity, stringComponent.name);
		});

	auto group = registry.group<StringComponent>(entt::get<TransformComponent>);

	for (auto entity : group)
		auto& [string, transform] = group.get<StringComponent, TransformComponent>(entity);

	registry.remove<StringComponent>(entity);
#endif
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
		auto& shader = Registry.get<Component::ShaderComponent>(entity);
		auto& transform = Registry.get<Component::TransformComponent>(entity);
		auto& mesh = Registry.get<Component::CustomMeshComponent>(entity);

		Renderer::RendererContext::Submit(mesh.va, mesh.ib, shader.Shader, transform.GetTransform());
	}
}
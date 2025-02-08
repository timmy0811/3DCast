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

	constexpr unsigned int maxTransforms = 32;

	TransformSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, maxTransforms, sizeof(glm::mat4)));
	TransformSSBO->BindBase(0);
}

Cast::Entity Cast::Scene::CreateEntity(const std::string& name)
{
	Entity entity = { Registry.create(), this };
	entity.AddComponents<Component::TransformComponent>(glm::mat4(1.0f));
	if (!entity.GetComponent<Component::TransformComponent>().Register(TransformSSBO))
		LOG_CORE_ERROR("Could not register transform component in registry.");

	entity.AddComponents<Component::TagComponent>(name);

	return entity;
}

void Cast::Scene::OnUpdate()
{
	auto group = Registry.group<Component::TransformComponent>(entt::get<Component::RasterizableComponent>);
	for (auto entity : group) {
		auto& material = Registry.get<Component::MaterialComponent>(entity);
		auto& transform = Registry.get<Component::TransformComponent>(entity);
		auto& mesh = Registry.get<Component::CustomMeshComponent>(entity);

		Cast::Ref<API::Core::Shader> shader;
		if (material.isDeffered) {
			shader = Cast::AssetCache.GetShaderHandle("shader_geometry_pass");
		}
		else {
			shader = Cast::AssetCache.GetShaderHandle(material.Shader);
		}

		if (mesh.ib) {
			Renderer::RendererContext::Submit(mesh.va, mesh.ib, shader);
		}
		else {
			Renderer::RendererContext::Submit(mesh.va, shader);
		}
	}
}
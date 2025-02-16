#include "castpch.h"
#include "Scene.h"

#include "3DCast/Log.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Scene/Components.h"

#include "3DCast/Renderer/Renderer.h"
#include "3DCast/Scene/SceneShaderCache.h"

#include <Vendor/glm/glm.hpp>

Cast::Scene::Scene()
	:IconRenderer(Cast::IconRenderer("../3DCast/ressources/configuration/icon.yml", "../3DCast/ressources/icon/icon_pallete.png"))
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
	constexpr unsigned int maxLightsPerType = 8;

	TransformSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, maxTransforms, sizeof(glm::mat4)));
	TransformSSBO->BindBase(0);

	DirLightsSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, maxLightsPerType, sizeof(DirectionalLight)));
	DirLightsSSBO->BindBase(1);

	SpotLightsSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, maxLightsPerType, sizeof(SpotLight)));
	SpotLightsSSBO->BindBase(2);

	PointLightsSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, maxLightsPerType, sizeof(PointLight)));
	PointLightsSSBO->BindBase(3);

	IconRenderer.BindBufferBaseDefault();
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

void Cast::Scene::OnDeferredRender()
{
	RenderCustomMeshComponent();
}

void Cast::Scene::OnForwardRender()
{
	if (InRenderView) return;

	RenderLightComponent();
}

void Cast::Scene::OnUpdate()
{
	static Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shader_shading_pass");
	shader->Bind();
	shader->SetUniform1i("BufferCountDirectionalLight", (int)(DirLightsSSBO->GetSize() / sizeof(DirectionalLight)));
	shader->SetUniform1i("BufferCountPointLight", (int)(PointLightsSSBO->GetSize() / sizeof(PointLight)));
	shader->SetUniform1i("BufferCountSpotLight", (int)(SpotLightsSSBO->GetSize() / sizeof(SpotLight)));
}

void Cast::Scene::ReallocateLights(int type)
{
	switch (type) {
	case 0: DirLightsSSBO->Empty(); break;
	case 1: PointLightsSSBO->Empty(); break;
	case 2: SpotLightsSSBO->Empty(); break;
	}

	auto view = Registry.view<Component::LightComponent>();

	view.each([&](entt::entity entity, Component::LightComponent& light) {
		if ((int)light.LightType == type) {
			light.Reallocate();
		}
		});
}

inline void Cast::Scene::RenderCustomMeshComponent()
{
	static Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shader_shading_pass");

	auto group = Registry.group<Component::TransformComponent>(entt::get<Component::CustomMeshComponent>);
	for (auto entity : group) {
		auto& transform = Registry.get<Component::TransformComponent>(entity);
		auto& material = Registry.get<Component::MaterialComponent>(entity);
		auto& mesh = Registry.get<Component::CustomMeshComponent>(entity);

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

inline void Cast::Scene::RenderLightComponent()
{
	IconRenderer.Clear();

	auto view = Registry.view<Component::LightComponent>();

	for (auto entity : view) {
		Component::LightComponent& light = view.get<Component::LightComponent>(entity);
		Component::TransformComponent& transform = Registry.get<Component::TransformComponent>(entity);

		glm::vec3 position = transform.GetTranslation();
		light.EntityPosition = position;

		switch (light.LightType) {
		case Component::LightComponent::Type::Directional:
			IconRenderer.AddIcon(Icon::LightDirectional, position);
			// Render vector
			break;
		case Component::LightComponent::Type::Point:
			IconRenderer.AddIcon(Icon::LightPoint, position);
			break;
		case Component::LightComponent::Type::Spot:
			// Render vector
			IconRenderer.AddIcon(Icon::LightSpot, position);
			break;
		}
	}

	IconRenderer.RenderAll();
}
#include "castpch.h"
#include "Scene.h"

#include "3DCast/Core/Log.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Scene/Component/Component.h"

#include "3DCast/Scene/SceneShaderCache.h"
#include "3DCast/Memory/Batching/BatchManager.h"

#include <vendor/glm/glm.hpp>

Cast::Scene::Scene()
	: IconRenderer_(IconRenderer(std::string(ASSET_DIR) + "configuration/icon.yml",
	                             std::string(ASSET_DIR) + "img/icon/icon_pallete.png"))
{
	EntityDescriptorPool.reserve(1000);

	RegisterComponentImGuiRenderCallback<Component::TagComponent>();
	RegisterComponentImGuiRenderCallback<Component::TransformComponent>();
	RegisterComponentImGuiRenderCallback<Component::LightComponent>();
	RegisterComponentImGuiRenderCallback<Component::MeshComponent>();
	RegisterComponentImGuiRenderCallback<Component::CustomMeshComponent>();
	RegisterComponentImGuiRenderCallback<Component::MaterialComponent>();
	RegisterComponentImGuiRenderCallback<Component::CameraComponent>();
	RegisterComponentImGuiRenderCallback<Component::ShaderComponent>();

	constexpr unsigned int maxLightsPerType = 8;

	DirLightsSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
	                                              API::Core::Buffer::MemoryLayout::DYNAMIC, maxLightsPerType,
	                                              sizeof(DirectionalLight)));
	SpotLightsSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
	                                               API::Core::Buffer::MemoryLayout::DYNAMIC, maxLightsPerType,
	                                               sizeof(SpotLight)));
	PointLightsSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
	                                                API::Core::Buffer::MemoryLayout::DYNAMIC, maxLightsPerType,
	                                                sizeof(PointLight)));
}

Cast::Ref<Cast::Entity> Cast::Scene::CreateEntity(const std::string& name, const bool registerTransform)
{
	entt::entity handle = Registry.create();
	auto entity = CreateRef<Entity>(handle, this);
	entity->AddComponents<Component::TransformComponent>(glm::mat4(1.0f));

	if (registerTransform)
	{
		if (!entity->GetComponent<Component::TransformComponent>().Register(&TransRegistry))
			LOG_CORE_ERROR("Could not register transform component in registry.");
	}

	entity->AddComponents<Component::TagComponent>(name);
	EntityDescriptorPool.insert({handle, entity});

	return entity;
}

void Cast::Scene::RemoveEntity(Entity& entity)
{
	if (!Registry.valid(entity.GetEntityHandle()))
	{
		LOG_CORE_WARN("Attempted to remove an invalid entity.");
		return;
	}

	Registry.destroy(entity.GetEntityHandle());
	EntityDescriptorPool.erase(entity.GetEntityHandle());
}

bool Cast::Scene::RegisterTransformComponent(Ref<Entity> entity)
{
	auto& view = entity->GetComponent<Component::TransformComponent>();
	if (!view.isRegistered)
	{
		if (!view.Register(&TransRegistry))
		{
			LOG_CORE_ERROR("Could not register transform component in registry.");
			return false;
		}
		return true;
	}
	return false;
}

void Cast::Scene::OnDeferredRender() const
{
	BindTransformSSBO();
	SamplerRegistry.BindSamplerBuffersToShaderPoints();

	static Ref<API::Core::Shader> shader = AssetCache.GetShaderHandle("shader_geometry_pass");
	Memory::BatchMemoryHandler.Render(shader);
	Memory::BatchMemoryHandler.RenderIndexed(shader);
}

void Cast::Scene::OnForwardRender()
{
	if (InRenderView) return;

	RenderLightComponent();
}

void Cast::Scene::OnUpdate() const
{
	static Ref<API::Core::Shader> shader = AssetCache.GetShaderHandle("shader_shading_pass");
	shader->Bind();
	shader->SetUniform1i("BufferCountDirectionalLight", (int)(DirLightsSSBO->GetSize() / sizeof(DirectionalLight)));
	shader->SetUniform1i("BufferCountPointLight", (int)(PointLightsSSBO->GetSize() / sizeof(PointLight)));
	shader->SetUniform1i("BufferCountSpotLight", (int)(SpotLightsSSBO->GetSize() / sizeof(SpotLight)));
}

void Cast::Scene::ReallocateLights(const int type)
{
	switch (type)
	{
	case 0: DirLightsSSBO->Empty();
		break;
	case 1: PointLightsSSBO->Empty();
		break;
	case 2: SpotLightsSSBO->Empty();
		break;
	default: ;
	}

	const auto view = Registry.view<Component::LightComponent>();

	view.each([&](entt::entity entity, Component::LightComponent& light)
	{
		if ((int)light.LightType == type)
		{
			light.Reallocate();
		}
	});
}

void Cast::Scene::BindSSBOforShadingPass() const
{
	BindLightSSBOs();
}

Cast::Ref<Cast::Entity> Cast::Scene::GetEntityReferenceByHandle(const entt::entity ent)
{
	const auto it = EntityDescriptorPool.find(ent);
	if (it != EntityDescriptorPool.end())
	{
		return it->second;
	}

	else return nullptr;
}

inline void Cast::Scene::RenderLightComponent()
{
	IconRenderer_.Clear();

	const auto view = Registry.view<Component::LightComponent>();

	for (const auto entity : view)
	{
		Component::LightComponent& light = view.get<Component::LightComponent>(entity);
		Component::TransformComponent& transform = Registry.get<Component::TransformComponent>(entity);

		glm::vec3 position = transform.GetTranslation();
		light.EntityPosition = position;

		switch (light.LightType)
		{
		case Component::LightComponent::Type::Directional:
			IconRenderer_.AddIcon(LightDirectional, position);
			// Render vector
			break;
		case Component::LightComponent::Type::Point:
			IconRenderer_.AddIcon(LightPoint, position);
			break;
		case Component::LightComponent::Type::Spot:
			// Render vector
			IconRenderer_.AddIcon(LightSpot, position);
			break;
		}
	}

	BindSymbolSSBOs();
	IconRenderer_.RenderAll();
}

inline void Cast::Scene::BindLightSSBOs() const
{
	DirLightsSSBO->BindBase(1);
	SpotLightsSSBO->BindBase(2);
	PointLightsSSBO->BindBase(3);
}

inline void Cast::Scene::BindSymbolSSBOs() const
{
	IconRenderer_.BindBufferBaseDefault();
}

inline void Cast::Scene::BindTransformSSBO() const
{
	TransRegistry.BindBase(0);
}

#pragma once

#include <string>
#include <vector>
#include <entt/entt.hpp>

#include "3DCast/Core.h"
#include "3DCast/Renderer/IconRenderer.h"

#include "3DCast/Scene/Component/Typedefinition.h"
#include "3DCast/Scene/TransformRegistry.h"

#include <API/core/Buffer.h>

namespace Cast
{
	class Entity;

	class Scene
	{
		using ComponentHandler = std::function<Component::UIResponse(entt::registry&, entt::entity)>;

	public:
		Scene();
		~Scene() = default;

		Ref<Entity> CreateEntity(const std::string& name = "Untagged", bool registerTransform = false);
		void RemoveEntity(Entity& entity, bool recursive = true);
		void RemoveEntity(Ref<Entity> entity, bool recursive = true);
		void RemoveEntityBulkOptimized(Ref<Entity> entity);

		bool RegisterTransformComponent(Ref<Entity> entity);

		void OnDeferredRender() const;
		void OnForwardRender();
		void OnUpdate() const;
		void ReallocateLights(int type);

		void BindSSBOforShadingPass() const;

		inline std::unordered_map<entt::entity, Ref<Entity>>& GetEntityDescriptors() { return EntityDescriptorPool; }
		Ref<Entity> GetEntityReferenceByHandle(entt::entity ent);

		inline Ref<API::Core::Buffer> GetDirLightsBuffer() { return DirLightsSSBO; }
		inline Ref<API::Core::Buffer> GetSpotLightsBuffer() { return SpotLightsSSBO; }
		inline Ref<API::Core::Buffer> GetPointLightsBuffer() { return PointLightsSSBO; }

		inline TransformRegistry* GetTransformRegistry() { return &TransRegistry; }
		inline entt::registry& GetRegistry() { return Registry; }
		inline const std::vector<ComponentHandler>& GetComponentImGuiCallbacks() { return ComponentHandlers; }

		inline bool& GetInRenderView() { return InRenderView; }

		template <typename Comp>
		void RegisterComponentImGuiRenderCallback()
		{
			ComponentHandlers.push_back([](entt::registry& registry, const entt::entity entity) -> Component::UIResponse
			{
				if (registry.all_of<Comp>(entity))
				{
					auto& component = registry.get<Comp>(entity);
					return component.OnImGuiRender();
				}

				return {};
			});
		}

	private:
		inline void RenderLightComponent();
		inline void BindLightSSBOs() const;
		inline void BindSymbolSSBOs() const;
		inline void BindTransformSSBO() const;

	private:
		entt::registry Registry;
		std::vector<ComponentHandler> ComponentHandlers;
		std::unordered_map<entt::entity, Ref<Entity>> EntityDescriptorPool;

		IconRenderer IconRenderer_;

		bool InRenderView = false;

		TransformRegistry TransRegistry{};

		Ref<API::Core::Buffer> DirLightsSSBO;
		Ref<API::Core::Buffer> SpotLightsSSBO;
		Ref<API::Core::Buffer> PointLightsSSBO;

		friend class Entity;
	};
}

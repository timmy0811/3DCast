#pragma once

#include <string>
#include <vector>
#include <entt/entt.hpp>

#include "3DCast/Core.h"
#include "3DCast/Data/ShaderDataObjects/Light.h"
#include "3DCast/Renderer/IconRenderer.h"

#include "3DCast/Scene/Component/Typedefinition.h"

#include <API/core/Buffer.h>

namespace Cast {
	class Entity;

	class Scene {
		using ComponentHandler = std::function<Component::UIResponse(entt::registry&, entt::entity)>;

	public:
		Scene();
		~Scene() = default;

		Cast::Ref<Cast::Entity> CreateEntity(const std::string& name = "Untagged", bool registerTransform = false);
		void RemoveEntity(Entity& entity);

		bool RegisterTransformComponent(Ref<Entity> entity);

		void OnDeferredRender();
		void OnForwardRender();
		void OnUpdate();
		void ReallocateLights(int type);

		void BindSSBOforShadingPass();

		inline std::vector<Ref<Entity>>& GetEntityDescriptors() { return EntityDescriptorPool; }

		inline Cast::Ref<API::Core::Buffer> GetDirLightsBuffer() { return DirLightsSSBO; }
		inline Cast::Ref<API::Core::Buffer> GetSpotLightsBuffer() { return SpotLightsSSBO; }
		inline Cast::Ref<API::Core::Buffer> GetPointLightsBuffer() { return PointLightsSSBO; }

		inline Cast::Ref<API::Core::Buffer> GetTransformRegistry() { return TransformSSBO; }
		inline entt::registry& GetRegistry() { return Registry; }
		inline const std::vector<ComponentHandler>& GetComponentImGuiCallbacks() { return ComponentHandlers; }

		inline bool& GetInRenderView() { return InRenderView; }

		template<typename Comp>
		void RegisterComponentImGuiRenderCallback() {
			ComponentHandlers.push_back([](entt::registry& registry, entt::entity entity) -> Component::UIResponse {
				if (registry.all_of<Comp>(entity)) {
					auto& component = registry.get<Comp>(entity);
					return component.OnImGuiRender();
				}

				return {};
				});
		}

	private:
		inline void RenderLightComponent();
		inline void BindLightSSBOs();
		inline void BindSymbolSSBOs();
		inline void BindTransformSSBO();

	private:
		entt::registry Registry;
		std::vector<ComponentHandler> ComponentHandlers;
		std::vector<Ref<Entity>> EntityDescriptorPool;

		IconRenderer IconRenderer;

		bool InRenderView = false;

		Cast::Ref<API::Core::Buffer> TransformSSBO;
		Cast::Ref<API::Core::Buffer> DirLightsSSBO;
		Cast::Ref<API::Core::Buffer> SpotLightsSSBO;
		Cast::Ref<API::Core::Buffer> PointLightsSSBO;

		friend class Entity;
	};
}
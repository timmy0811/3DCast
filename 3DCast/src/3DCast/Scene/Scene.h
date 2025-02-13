#pragma once

#include <string>
#include <vector>
#include <entt/entt.hpp>

#include "3DCast/Core.h"
#include "3DCast/Scene/DataObjects/ShaderDataObjects.h"
#include "3DCast/Renderer/IconRenderer.h"

#include <API/core/Buffer.h>

namespace Cast {
	class Entity;

	class Scene {
		using ComponentHandler = std::function<void(entt::registry&, entt::entity)>;

	public:
		Scene();
		~Scene() = default;

		Entity CreateEntity(const std::string& name = "Untagged");

		void OnDeferredRender();
		void OnForwardRender();
		void OnUpdate();
		void ReallocateLights(int type);

		inline Cast::Ref<API::Core::Buffer> GetDirLightsBuffer() { return DirLightsSSBO; }
		inline Cast::Ref<API::Core::Buffer> GetSpotLightsBuffer() { return SpotLightsSSBO; }
		inline Cast::Ref<API::Core::Buffer> GetPointLightsBuffer() { return PointLightsSSBO; }

		inline Cast::Ref<API::Core::Buffer> GetTransformRegistry() { return TransformSSBO; }
		inline entt::registry& GetRegistry() { return Registry; }
		inline const std::vector<ComponentHandler>& GetComponentImGuiCallbacks() { return ComponentHandlers; }

		inline bool& GetInRenderView() { return InRenderView; }

		template<typename Component>
		void RegisterComponentImGuiRenderCallback() {
			ComponentHandlers.push_back([](entt::registry& registry, entt::entity entity) {
				if (registry.all_of<Component>(entity)) {
					auto& component = registry.get<Component>(entity);
					component.OnImGuiRender();
				}
				});
		}

	private:
		inline void RenderCustomMeshComponent();
		inline void RenderLightComponent();

	private:
		entt::registry Registry;
		std::vector<ComponentHandler> ComponentHandlers;

		IconRenderer IconRenderer;

		bool InRenderView = false;

		Cast::Ref<API::Core::Buffer> TransformSSBO;
		Cast::Ref<API::Core::Buffer> DirLightsSSBO;
		Cast::Ref<API::Core::Buffer> SpotLightsSSBO;
		Cast::Ref<API::Core::Buffer> PointLightsSSBO;

		friend class Entity;
	};
}
#pragma once

#include <string>
#include <vector>
#include <entt/entt.hpp>

#include "3DCast/Core.h"
#include "3DCast/Renderer/IconRenderer.h"

#include "3DCast/Scene/Component/Typedefinition.h"
#include "Registry/TransformRegistry.h"

#include <API/core/Buffer.h>

namespace Cast
{
	namespace Serialization
	{
		class SceneSerializer;
	}

	class Entity;

	class Scene
	{
		using ComponentHandler = std::function<Component::UIResponse(entt::registry&, entt::entity)>;

	public:
		Scene();
		~Scene();

		void Shutdown();

		Ref<Entity> CreateEntity(const std::string& name = "Untagged", bool registerTransform = false);
		void RemoveEntity(Entity& entity, bool recursive = true);
		void RemoveEntity(Ref<Entity> entity, bool recursive = true);
		void RemoveEntityBulkOptimized(Ref<Entity> entity);

		Cast::Ref<Entity> RaycastSelection(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const;

		bool RegisterTransformComponent(Ref<Entity> entity);

		void OnDeferredRender() const;
		void OnForwardRender();
		void OnUpdate() const;
		void ReallocateLights(int type);

		void BindSSBOForShadingPass() const;

		inline void SetEditorSelectionContext(Ref<Entity> entity) {EditorSelectionContext = entity; }

		inline std::unordered_map<entt::entity, Ref<Entity>>& GetEntityDescriptors() { return EntityDescriptorPool; }
		Ref<Entity> GetEntityReferenceByHandle(entt::entity ent);
		inline Ref<Entity> GetEditorSelectionContext() { return EditorSelectionContext; }
		inline bool IsEntitySelected() const { return EditorSelectionContext != nullptr; }

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

	protected:
		entt::registry Registry;
		std::vector<ComponentHandler> ComponentHandlers;
		std::unordered_map<entt::entity, Ref<Entity>> EntityDescriptorPool;
		Ref<Entity> EditorSelectionContext;

		IconRenderer IconRenderer_;

		bool InRenderView = false;
		bool IsShutdown = false;

		TransformRegistry TransRegistry{};

		Ref<API::Core::Buffer> DirLightsSSBO;
		Ref<API::Core::Buffer> SpotLightsSSBO;
		Ref<API::Core::Buffer> PointLightsSSBO;

		friend class Entity;
		friend class Serialization::SceneSerializer;
	};
}

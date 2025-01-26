#pragma once

#include <string>
#include <vector>
#include <entt/entt.hpp>

namespace Cast {
	class Entity;

	class Scene {
		using ComponentHandler = std::function<void(entt::registry&, entt::entity)>;

	public:
		Scene();
		~Scene() = default;

		Entity CreateEntity(const std::string& name = "Untagged");

		void OnUpdate();
		void SubmitShaderData();

		inline entt::registry& GetRegistry() { return Registry; }
		inline const std::vector<ComponentHandler>& GetComponentImGuiCallbacks() { return ComponentHandlers; }

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
		entt::registry Registry;
		std::vector<ComponentHandler> ComponentHandlers;

		friend class Entity;
	};
}
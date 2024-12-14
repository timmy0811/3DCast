#pragma once

#include <string>
#include <entt/entt.hpp>

namespace Cast {
	class Entity;

	class Scene {
	public:
		Scene();
		~Scene() = default;

		Entity CreateEntity(const std::string& name = "Untagged");

		void OnUpdate();

	private:
		entt::registry Registry;

		friend class Entity;
	};
}
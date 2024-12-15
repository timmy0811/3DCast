#pragma once

#include "Scene.h"
#include "entt/entt.hpp"
#include "3DCast/Core.h"

namespace Cast {
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		inline const entt::entity GetEntityHandle() const { return EntityHandle; }

		template<typename T, typename... Args>
		T& AddComponents(Args&&... args)
		{
			CAST_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return Scene->Registry.emplace<T>(EntityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		T& GetComponent()
		{
			CAST_ASSERT(!HasComponent<T>(), "Entity does not have component!");
			return Scene->Registry.get<T>(EntityHandle);
		}

		template <typename T>
		bool HasComponent()
		{
			// return Scene->Registry.has<T>(EntityHandle);
			return false;
		}

		template <typename T>
		void RemoveComponent()
		{
			CAST_ASSERT(!HasComponent<T>(), "Entity does not have component!");
			Scene->Registry.remove<T>(EntityHandle);
		}

		operator bool() const { return EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)EntityHandle; }

		bool operator==(const Entity& other) const
		{
			return EntityHandle == other.EntityHandle && Scene == other.Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		entt::entity EntityHandle{ entt::null };
		Scene* Scene = nullptr;
	};
}
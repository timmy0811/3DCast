#pragma once

#include "Scene.h"
#include "entt/entt.hpp"
#include "3DCast/Core.h"

namespace Cast {
	namespace Component {
		enum class Type;
	}

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		//Entity(const Entity& other) = default;

		inline const entt::entity GetEntityHandle() const { return EntityHandle; }

		template<typename T, typename... Args>
		T& AddComponents(Args&&... args)
		{
			if (HasComponent<T>()) {
				LOG_CORE_WARN("Trying to add a component even though it hase already been added. Ignoring.");
				return GetComponent<T>();
			}

			EntityHasRequiredComponents(T::GetType());

			auto& comp = Scene->Registry.emplace<T>(EntityHandle, std::forward<Args>(args)...);
			comp.SetEntity(this);
			comp.OnAfterEntitySetBehaviour();
			return comp;
		}

		template <typename T>
		T& GetComponent()
		{
			CAST_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return Scene->Registry.get<T>(EntityHandle);
		}

		template <typename T>
		bool HasComponent()
		{
			return Scene->Registry.all_of<T>(EntityHandle);
		}

		template <typename T>
		void RemoveComponent()
		{
			if (!HasComponent<T>()) {
				LOG_CORE_WARN("Trying to remove a component that has never been added. Ignoring.");
				return;
			}
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

		void SetScene(Scene* scene) { Scene = scene; }
		Scene* GetScene() { return Scene; }

		void AddChild(Ref<Entity> child) { Children.push_back(child); }
		void RemoveChild(Ref<Entity> child) { Children.erase(std::remove(Children.begin(), Children.end(), child), Children.end()); }

		void SetParent(Ref<Entity> parent) { Parent = parent; }
		Ref<Entity> GetParent() { return Parent; }

		std::vector<Ref<Entity>>& GetChildren() { return Children; }
		bool IsChild() { return Parent != nullptr; }
		bool HasChildren() { return !Children.empty(); }

	private:
		bool EntityHasRequiredComponents(Component::Type type);

	private:
		entt::entity EntityHandle{ entt::null };
		Scene* Scene = nullptr;

		Ref<Entity> Parent;
		std::vector<Ref<Entity>> Children;
	};
}
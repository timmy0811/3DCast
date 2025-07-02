#pragma once

#include "Scene.h"
#include "entt/entt.hpp"
#include "3DCast/Core.h"

namespace Cast
{
	namespace Component
	{
		enum class Type;
	}

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		//Entity(const Entity& other) = default;

		[[nodiscard]] inline entt::entity GetEntityHandle() const { return EntityHandle; }

		template <typename T, typename... Args>
		T& AddComponents(Args&&... args)
		{
			if (HasComponent<T>())
			{
				LOG_CORE_WARN("Trying to add a component even though it hase already been added. Ignoring.");
				return GetComponent<T>();
			}

			EntityHasRequiredComponents(T::GetType());

			auto& comp = Scene_->Registry.emplace<T>(EntityHandle, std::forward<Args>(args)...);
			comp.SetEntity(Scene_->GetEntityReferenceByHandle(EntityHandle));
			comp.OnAfterEntitySetBehaviour();
			return comp;
		}

		template <typename T>
		T& GetComponent()
		{
			CAST_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return Scene_->Registry.get<T>(EntityHandle);
		}

		template <typename T>
		[[nodiscard]] bool HasComponent() const
		{
			return Scene_->Registry.all_of<T>(EntityHandle);
		}

		template <typename T>
		void RemoveComponent() const
		{
			if (!HasComponent<T>())
			{
				LOG_CORE_WARN("Trying to remove a component that has never been added. Ignoring.");
				return;
			}
			Scene_->Registry.remove<T>(EntityHandle);
		}

		operator bool() const { return EntityHandle != entt::null; }
		operator uint32_t() const { return static_cast<uint32_t>(EntityHandle); }

		bool operator==(const Entity& other) const
		{
			return EntityHandle == other.EntityHandle && Scene_ == other.Scene_;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		void SetScene(Scene* scene) { Scene_ = scene; }
		[[nodiscard]] Scene* GetScene() const { return Scene_; }

		void AddChild(Ref<Entity> child) { Children.push_back(child); }

		void RemoveChild(Ref<Entity> child)
		{
			Children.erase(std::remove(Children.begin(), Children.end(), child), Children.end());
		}

		void SetParent(Ref<Entity> parent) { Parent = parent; }
		Ref<Entity> GetParent() { return Parent; }

		std::vector<Ref<Entity>>& GetChildren() { return Children; }
		[[nodiscard]] bool IsChild() const { return Parent != nullptr; }
		[[nodiscard]] bool HasChildren() const { return !Children.empty(); }

	private:
		bool EntityHasRequiredComponents(Component::Type type);

	private:
		entt::entity EntityHandle{entt::null};
		Scene* Scene_ = nullptr;

		Ref<Entity> Parent;
		std::vector<Ref<Entity>> Children;
	};
}

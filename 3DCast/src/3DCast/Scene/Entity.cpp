#include "castpch.h"
#include "Entity.h"

#include "3DCast/Scene/Component/Component.h"
#include "3DCast/ImGui/TempElements/TempGuiElementCollection.h"
#include "3DCast/ImGui/TempElements/Elements/NotificationModal.h"

Cast::Entity::Entity(const entt::entity handle, Scene* scene)
	: EntityHandle(handle), Scene_(scene)
{
}

bool Cast::Entity::EntityHasRequiredComponents(const Component::Type type)
{
	switch (type) {
	case Component::Type::Mesh:
		if (!HasComponent<Component::TransformComponent>()) {
			LOG_CORE_WARN("Entity does not have a TransformComponent [needed by MeshComponent]. Adding one.");
			AddComponents<Component::TransformComponent>();
			GUI::TempGuiElementCollection::AddElement(new GUI::NotificationModal("Missing Dependency", "The Mesh-Component needs a Transform-Component to work properly. It has been added."));
			return false;
		}
	default:
		return true;
	}
}
#include "castpch.h"
#include "Entity.h"

Cast::Entity::Entity(entt::entity handle, Cast::Scene* scene)
	: EntityHandle(handle), Scene(scene)
{
}
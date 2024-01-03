// Sam Collier 2023

#include "Entity.h"
#include "Components.h"

void goop::Entity::addCustomComponent(goop::CustomComponent* component)
{
	scene->registry.emplace<goop::CustomComponent*>(entity, component);
}


#include "pch.h"
#include "entity.h"





Entity::Entity(int id)
    : _id(id)
{
}

Entity::~Entity()
{
    clear();
}

EntityComponent * Entity::getComponent(const char * name)
{
    for (auto& comp : _components)
        if (comp.first == name)
            return comp.second;

    return NULL;
}

const EntityComponent * Entity::getComponent(const char * name) const
{
    for (const auto& comp : _components)
        if (comp.first == name)
            return comp.second;

    return NULL;
}

void Entity::clear()
{
    for (auto& comp : _components)
    {
        componentIsAboutToBeRemovedSignal(comp.first, comp.second);
        delete comp.second;
    }

    _components.clear();
}



//
// EntityComponent
//

EntityComponent::EntityComponent(Entity * entity)
    : _entity(entity)
{
}

EntityComponent::~EntityComponent()
{
}


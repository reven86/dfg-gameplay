#include "pch.h"
#include "entity_manager.h"
#include "entity.h"




EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
    clear();
}

Entity * EntityManager::addEntity(int id)
{
    Entity * res = getEntity(id);
    if (res)
        return res;

    res = new Entity(id);
    res->_entityManager = this;
    _entities.emplace(std::make_pair(id, res));

    entityAddedSignal(id, res);

    return res;
}

void EntityManager::removeEntity(int id)
{
    auto it = _entities.find(id);
    if (it == _entities.end())
        return;

    delete (*it).second;
    _entities.erase(it);

    entityRemovedSignal(id);
}

void EntityManager::clear()
{
    for (auto& it : _entities)
    {
        entityRemovedSignal(it.first);
        delete it.second;
    }
    _entities.clear();
}
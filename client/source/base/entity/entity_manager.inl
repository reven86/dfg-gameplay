#include "entity_manager.h"




inline Entity * EntityManager::getEntity(int id)
{
    auto it = _entities.find(id);
    if (it == _entities.end())
        return NULL;

    return (*it).second;
}

inline const Entity * EntityManager::getEntity(int id) const
{
    auto it = _entities.find(id);
    if (it == _entities.end())
        return NULL;

    return (*it).second;
}

inline unsigned EntityManager::getEntityCount() const
{
    return _entities.size();
}
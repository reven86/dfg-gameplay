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
    return static_cast<unsigned>(_entities.size());
}

inline std::unordered_map<int, Entity *>::iterator EntityManager::begin()
{
    return _entities.begin();
}

inline std::unordered_map<int, Entity *>::const_iterator EntityManager::begin() const
{
    return _entities.cbegin();
}

inline std::unordered_map<int, Entity *>::iterator EntityManager::end()
{
    return _entities.end();
}

inline std::unordered_map<int, Entity *>::const_iterator EntityManager::end() const
{
    return _entities.cend();
}

inline int EntityManager::getNextEntityID() const
{
    return _highestId + 1;
}

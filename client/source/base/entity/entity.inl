#include "entity.h"




//
// Entity
//

inline int Entity::getId() const
{
    return _id;
}

template<class _Component>
inline _Component * Entity::getComponent()
{
    return static_cast<_Component *>(getComponent(_Component::getTypeName()));
}

template<class _Component>
inline const _Component * Entity::getComponent() const
{
    return static_cast<const _Component *>(getComponent(_Component::getTypeName()));
}

template<class _Component, class..._Args>
inline _Component * Entity::addComponent(_Args...args)
{
    // check whether the component of same type is already attached.
    _Component * res = getComponent<_Component>();
    if (res)
        return res;

    res = _Component::create(this, args...);
    GP_ASSERT(res);
    if (!res)
        return NULL;

    _components.push_back(std::make_pair(_Component::getTypeName(), res));

    componentAddedSignal(_Component::getTypeName(), res);

    return res;
}

template<class _Component>
inline void Entity::removeComponent()
{
    for (auto it = _components.begin(), end_it = _components.end(); it != end_it; ++it)
        if ((*it).first == _Component::getTypeName())
        {
            componentIsAboutToBeRemovedSignal((*it).first, (*it).second);
            delete (*it).second;
            _components.erase(it);
            return;
        }
}

inline EntityManager * Entity::getEntityManager()
{
    return _entityManager;
}

inline const EntityManager * Entity::getEntityManager() const
{
    return _entityManager;
}




//
// EntityComponent
//

Entity * EntityComponent::getEntity()
{
    return _entity;
}

const Entity * EntityComponent::getEntity() const
{
    return _entity;
}
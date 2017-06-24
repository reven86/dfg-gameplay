#pragma once


#ifndef __DFG_ENTITY_MANAGER_H__
#define __DFG_ENTITY_MANAGER_H__



/**
 * Manages creation, destruction and lifetime of the Entities.
 * There are no public constructors available, the best way to use 
 * EntityManager is to derive from it.
 */
class EntityManager : Noncopyable
{
public:
    //
    // Signals
    //

    sigc::signal<void, int, class Entity *> entityAddedSignal;
    sigc::signal<void, int> entityRemovedSignal;



    /**
     * Creates Entity with given ID. No components are added.
     * EntityManager takes ownership of the entity. To remove
     * the entity, call removeEntity.
     *
     * If entity with the same ID alredy exists, it's returned
     * and no new Entities are created.
     *
     * @param id ID of the Entity to create.
     * @return Newly created entity.
     */
    virtual class Entity * addEntity(int id);

    /**
     * Get Entity by ID.
     *
     * @param id Entity ID.
     * @return Entity instance or NULL.
     */
    inline class Entity * getEntity(int id);

    /**
     * Get Entity by ID (const).
     *
     * @param id Entity ID.
     * @return Entity instance or NULL.
     */
    inline const class Entity * getEntity(int id) const;

    /**
     * Get entities count.
     */
    inline unsigned getEntityCount() const;

    /**
     * Remove Entity with given ID.
     *
     * @param id Entity ID.
     */
    virtual void removeEntity(int id);

    /**
     * Remove all entities.
     */
    virtual void clear();

protected:
    EntityManager();
    virtual ~EntityManager();

private:
    std::unordered_map<int, Entity *> _entities;
};


#include "entity_manager.inl"


#endif // __DFG_ENTITY_MANAGER_H__
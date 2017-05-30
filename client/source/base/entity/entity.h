#pragma once


#ifndef __DFG_ENTITY_H__
#define __DFG_ENTITY_H__



/**
 * Base class for all entities.
 * Entity basically consists of components, such as Transform, Renderer, etc.
 *
 * The only way to create entity is through EntityManager.
 */

class Entity : Noncopyable
{
    friend class EntityManager;

public:
    //
    // Signals
    //

    sigc::signal<void, const char *, class EntityComponent *> componentAddedSignal;
    sigc::signal<void, const char *, class EntityComponent *> componentIsAboutToBeRemovedSignal;



    /**
     * Get ID of the entity within EntityManager.
     */
    inline int getId() const;

    /**
     * Attach component of given type. If component of the same type
     * is already attached, it's returned. So, basically, it acts as
     * get or create.
     *
     * @return Newly added component, or the attached one if it was added previously.
     */
    template<class _Component, class..._Args>
    inline _Component * addComponent(_Args...args);

    /**
     * Remove component from an entity.
     */
    template<class _Component>
    inline void removeComponent();

    /**
     * Removes all components attached.
     */
    void clear();

    /**
     * Get component of specified type or NULL.
     */
    template<class _Component>
    inline _Component * getComponent();

    /**
     * Get component of specified type or NULL (const-version).
     */
    template<class _Component>
    inline const _Component * getComponent() const;

    /**
     * Get entity manager this entity belongs to.
     */
    inline class EntityManager * getEntityManager();

    /**
     * Get entity manager this entity belongs to (const).
     */
    inline const class EntityManager * getEntityManager() const;

private:
    Entity(int id);
    virtual ~Entity();

    class EntityComponent * getComponent(const char * type);
    const class EntityComponent * getComponent(const char * type) const;

    //
    // Variables
    //

    int _id;
    class EntityManager * _entityManager;

    // we don't expose raw access to components to derived classes
    // note: when looking up component we DON'T use string comparison,
    // instead we compare pointers to speed up search, 
    // pointers act most likely as components ids.
    std::list<std::pair<const char *, class EntityComponent *>> _components;
};



/**
 * Base class for entity components.
 *
 * Each derived component should implement static getTypeName method that returns the string
 * that uniquely identifies the component. This string is used as key to get component of specified
 * type.
 *
 * For now we don't support attaching several components of the same type to one entity.
 *
 * Components can be added and removed only through Entity methods addComponent and removeComponent.
 *
 * Derived classes should have a static 'create' method which first argument should be the entity
 * this component is being attached to. The 'create' method returns newly created component or 
 * NULL if component can't be initialized properly.
 */
class EntityComponent : Noncopyable
{
    friend class Entity;

public:
    /**
     * Get entity this component attached to.
     */
    inline Entity * getEntity();

    /**
     * Get entity this component attached to (const).
     */
    inline const Entity * getEntity() const;

protected:
    EntityComponent(Entity * entity);
    virtual ~EntityComponent();

private:
    // members automatically filed by Entity
    Entity * _entity;
};



#include "entity.inl"


#endif // __DFG_ENTITY_H__
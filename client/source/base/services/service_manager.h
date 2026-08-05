#ifndef __DFG_SERVICE_MANAGER__
#define __DFG_SERVICE_MANAGER__

#include "service.h"
#include "signals.h"
#include <cstdint>
#include <unordered_map>




/** FNV-1a 32-bit. Same type name string always yields the same id across TUs. */
constexpr uint32_t serviceTypeId(const char * name)
{
    uint32_t hash = 2166136261u;
    while (*name)
    {
        hash ^= static_cast<uint8_t>(*name++);
        hash *= 16777619u;
    }
    return hash;
}




/** @class ServiceManager service_manager.h "services/service_manager.h"
 *	@brief All services state processing goes here.
 *
 *	@author Andrew "RevEn" Karpushin
 */

class ServiceManager : public Singleton< ServiceManager >
{
    friend class Singleton<ServiceManager>;

public:
    /**
     * Signals registry.
     */
    mutable Signals signals;

    /** @brief Service registration.
     *
     *	Registers service by its typename.
     *
     *	@param[in]	dependencies	A NULL terminated array of service dependencies, or NULL.
     *	@return New service instance.
     */
    template< class _Service >
    _Service * registerService(Service ** dependencies)
    {
        _Service * newService = new _Service(this);
        registerService(_Service::getTypeName(), newService, dependencies);
        return newService;
    }

    /**
     * Helper function to find service, assuming it was registered by its class name.
     */
    template< class _Service >
    _Service * findService() const
    {
        // Cached per TU; value is identical everywhere because it hashes string content.
        static const uint32_t typeId = serviceTypeId(_Service::getTypeName());
        return static_cast<_Service *>(findServiceById(typeId));
    }

    /**
     * Get frame time in seconds.
     */
    float getFrameElapsedTime() const { return _elapsedTime; };

    /**
     * Shutdown all services.
     */
    void shutdown();

    /**
     * Update all services.
     */
    void update(float elapsedTime);

    /**
     * Get current state.
     */
    const Service::State& getState() const { return _state; }

protected:
    ServiceManager();
    virtual ~ServiceManager();

private:
    struct ServiceData
    {
        std::unique_ptr< Service > service;
        const char * name;
        uint32_t typeId;
    };

    // services, sorted by priority.
    typedef std::list< ServiceData * > ServicesType;

    //! Find service by type id (hash of getTypeName()).
    Service * findServiceById(uint32_t typeId) const;

    /*! \brief Service registration.
     *
     *	\param[in]	name			Name of the service, typically class name.
     *	\param[in]	service			Service instance.
     *	\param[in]	dependencies	A NULL terminated array of service dependencies, or NULL.
     */
    void registerService(const char * name, Service * service, Service ** dependencies);

    void insertService(ServiceData * data, ServicesType::iterator insertBefore);

    void cleanup();

    ServicesType _services;

    // O(1) lookup by hash of getTypeName(); content-stable across TUs.
    std::unordered_map<uint32_t, ServiceData *> _servicesById;

    // service manager also works in Service states.
    Service::State	_state;

    float _elapsedTime;
};




#endif

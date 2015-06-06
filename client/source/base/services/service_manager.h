#ifndef __DFG_SERVICE_MANAGER__
#define __DFG_SERVICE_MANAGER__

#include "service.h"
#include "signals.h"




/*! \class ServiceManager service_manager.h "services/service_manager.h"
 *	\brief All services state processing goes here.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class ServiceManager : public Singleton< ServiceManager >
{
    friend class Singleton< ServiceManager >;

    struct ServiceData
    {
        std::unique_ptr< Service > service;
        std::string name;
    };

    // services, sorted by priority.
    typedef std::list< ServiceData * > ServicesType;
    ServicesType _services;

    // service manager also works in Service states.
    Service::State	_state;

    float _elapsedTime;

protected:
    ServiceManager();
    virtual ~ServiceManager();

public:
    // signals registry
    mutable Signals signals;

    /*! \brief Service registration.
     *
     *	Registers service by its typename.
     *
     *	\param[in]	dependencies	A NULL terminated array of service dependencies, or NULL.
     *	\return New service instance.
     */
    template< class _Service >
    _Service * registerService(Service ** dependencies)
    {
        _Service * newService = new _Service(this);
        registerService(_Service::getTypeName(), newService, dependencies);
        return newService;
    }

    //! Helper function to find service, assuming it was registered by its class name.
    template< class _Service >
    _Service * findService() const { return static_cast<_Service *>(findService(_Service::getTypeName())); }

    //! Get frame time in seconds.
    float getFrameElapsedTime() const { return _elapsedTime; };

    //! Shutdown all services.
    void shutdown();

    //! Update all services.
    void update(float elapsedTime);

    //! Get current state.
    const Service::State& getState() const { return _state; }

private:
    //! Find service by name.
    Service * findService(const char * name) const;

    /*! \brief Service registration.
     *
     *	\param[in]	name			Name of the service, typically class name.
     *	\param[in]	service			Service instance.
     *	\param[in]	dependencies	A NULL terminated array of service dependencies, or NULL.
     */
    void registerService(const char * name, Service * service, Service ** dependencies);

    void cleanup();
};




#endif
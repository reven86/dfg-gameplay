#ifndef __DFG_SERVICE__
#define __DFG_SERVICE__





/*! \class Service service.h "services/service.h"
 *	\brief %Service base class.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class Service : Noncopyable
{
    friend class ServiceManager;

public:
    enum State
    {
        PRE_INIT,
        INITIALIZING,
        RUNNING,
        SHUTTING_DOWN,
        COMPLETE,
    };

protected:
    State _state;
    const class ServiceManager * _manager;

public:
    Service(const ServiceManager * manager);
    virtual ~Service();

    const State& getState() const { return _state; };
    const class ServiceManager * getManager() const { return _manager; };

    // state changing functions.
    // each function returns True if service is ready to transit to next state.

    /** Pre initializing state function.
     *
     * Use this function to make some lightweight initialization actions, e.g.
     * to store pointers to services this service depends on.
     */
    virtual bool onPreInit() { return true; };

    /** Initialization state function.
     *
     * Use this function to load all necessary resources and assets. This method
     * will be called every frame until true is returned (as any other state method).
     */
    virtual bool onInit() { return true; };

    /** Tick state function.
     *
     * Use this function if your service is needed to make some actions every frame.
     */
    virtual bool onTick() { return true; };

    /** Shutdown state function.
     *
     * Use this function to free all service's resources.
     */
    virtual bool onShutdown() { return true; };

protected:
    void setState(const State& state) { _state = state; };
};





#endif
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
		RefPtr< Service > service;
		std::string name;
	};

	// services, sorted by priority.
	typedef std::list< ServiceData * >	ServicesType;
	ServicesType	_services;

	// service manager is also works in Service states.
	Service::State	_state;

	float _elapsedTime;

protected:
	ServiceManager			( );
	virtual ~ServiceManager	( );

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
	_Service * RegisterService	( Service ** dependencies )
	{ 
		_Service * new_service = new _Service( this );
		RegisterService( _Service::GetName( ), new_service, dependencies ); 
		return new_service;
	}

	//! Helper function to find service, assuming it was registered by its class name.
	template< class _Service >
	_Service * FindService	( ) const { return static_cast< _Service * >( FindService( _Service::GetName( ) ) ); }

	//! Get frame time.
	float GetElapsedTime( ) const { return _elapsedTime; };

	//! Shutdown all services.
	void Shutdown ( );

	//! Update all services.
	void Update( float elapsedTime );

    //! Get current state.
    const Service::State& GetState( ) const { return _state; }

private:
	//! Find service by name.
	Service * FindService	( const char * name ) const;

	/*! \brief Service registration.
	 *
	 *	\param[in]	name			Name of the service, typically class name.
	 *	\param[in]	service			Service instance.
	 *	\param[in]	dependencies	A NULL terminated array of service dependencies, or NULL.
	 */
	void RegisterService	( const char * name, Service * service, Service ** dependencies );

	void Cleanup( );
};




#endif
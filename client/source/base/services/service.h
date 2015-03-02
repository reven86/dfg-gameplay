#ifndef __DFG_SERVICE__
#define __DFG_SERVICE__







/*! \class Service service.h "services/service.h"
 *	\brief %Service base class.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class Service : Noncopyable, public gameplay::Ref
{
public:
	enum State
	{
		ES_PRE_INIT,
		ES_INITIALIZING,
		ES_RUNNING,
		ES_SHUTTING_DOWN,
		ES_COMPLETE,
	};

protected:
	State _state;
	const class ServiceManager * _manager;

public:
	Service				( const ServiceManager * manager );
	virtual ~Service	( );

	const State& GetState( ) const { return _state; };
	void SetState( const State& state ) { _state = state; };

	const class ServiceManager * GetManager( ) const { return _manager; };

	// state changing functions.
	// each function returns True if service is ready to transit to next state.

	virtual bool OnPreInit( ) { return true; };
	virtual bool OnInit( ) { return true; };
	virtual bool OnTick( ) { return true; };
	virtual bool OnShutdown( ) { return true; };
};





#endif
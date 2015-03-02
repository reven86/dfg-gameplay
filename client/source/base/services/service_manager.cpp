#include "pch.h"
#include "service_manager.h"
#include "service.h"





ServiceManager::ServiceManager( )
	: _state( Service::ES_PRE_INIT )
	, _elapsedTime( 0.0f )
{
}

ServiceManager::~ServiceManager( )
{
}

void ServiceManager::Cleanup( )
{
	for( ServicesType::iterator it = _services.begin( ), end_it = _services.end( ); it != end_it; it++ )
		delete ( *it );

	_services.clear( );
}

void ServiceManager::RegisterService( const char * name, Service * service, Service ** dependencies )
{
	ServiceData * new_data = new ServiceData( );
	new_data->name = name;
	new_data->service = RefPtr< Service >( service );

	if( !dependencies )
	{
		_services.push_front( new_data );
		return;
	}

	int dependencies_count = 0;
	Service ** temp = dependencies;
	while( *temp )
	{
		dependencies_count++;
		temp++;
	}

	ServicesType::iterator it = _services.begin( ), end_it = _services.end( );
	while( it != end_it && dependencies_count > 0 )
	{
		temp = dependencies;
		while( *temp )
		{
			if( ( *it )->service.get( ) == *temp )
			{
				dependencies_count--;
				break;
			}

			temp++;
		}

		it++;
	}

	if( it != end_it )
		it++;

	if( dependencies_count > 0 )
	{
		GP_ERROR( "Trying to register service %s which have dependency on unregistered service", name );
		return;
	}

	_services.insert( it, new_data );
}

Service * ServiceManager::FindService( const char * name ) const
{
	for( ServicesType::const_iterator it = _services.begin( ), end_it = _services.end( ); it != end_it; it++ )
		if( ( *it )->name == name )
			return ( *it )->service.get( );
	return NULL;
}

void ServiceManager::Shutdown( )
{
	for( ServicesType::iterator it = _services.begin( ), end_it = _services.end( ); it != end_it; it++ )
		if( ( *it )->service->GetState( ) != Service::ES_COMPLETE )
			( *it )->service->SetState( Service::ES_SHUTTING_DOWN );

	_state = Service::ES_SHUTTING_DOWN;
	while( _state != Service::ES_COMPLETE )
		Update( _elapsedTime );

	Cleanup( );
}

void ServiceManager::Update( float elapsedTime )
{
	_elapsedTime = elapsedTime;
	if( _state == Service::ES_COMPLETE )
		return;

	static bool ( Service::* state_funcs[] ) ( ) =
	{
		&Service::OnPreInit,
		&Service::OnInit,
		&Service::OnTick,
		&Service::OnShutdown
	};

	bool all_completed = true;
	for( ServicesType::iterator it = _services.begin( ), end_it = _services.end( ); it != end_it; it++ )
	{
		const Service::State& service_state = ( *it )->service->GetState( );
		if( service_state <= _state )
		{
			bool res = (( *it )->service.get( )->*state_funcs[ service_state ])( );
			if( service_state == _state )
				all_completed &= res;

			if( res )
				( *it )->service->SetState( static_cast< Service::State >( service_state + 1 ) );
		}
	}

	if( all_completed )
		_state = static_cast< Service::State >( _state + 1 );
}
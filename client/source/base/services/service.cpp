#include "pch.h"
#include "service.h"







Service::Service( const class ServiceManager * manager ) :
	_state( ES_PRE_INIT ), _manager( manager )
{
}

Service::~Service( )
{
}
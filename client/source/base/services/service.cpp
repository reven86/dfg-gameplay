#include "pch.h"
#include "service.h"







Service::Service(const class ServiceManager * manager)
    : _state(PRE_INIT)
    , _manager(manager)
{
}

Service::~Service()
{
}
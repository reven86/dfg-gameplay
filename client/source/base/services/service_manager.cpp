#include "pch.h"
#include "service_manager.h"
#include "service.h"





ServiceManager::ServiceManager()
    : _state(Service::PRE_INIT)
    , _elapsedTime(0.0f)
{
}

ServiceManager::~ServiceManager()
{
}

void ServiceManager::cleanup()
{
    for (ServicesType::reverse_iterator it = _services.rbegin(), end_it = _services.rend(); it != end_it; it++)
        delete (*it);

    _services.clear();
}

void ServiceManager::registerService(const char * name, Service * service, Service ** dependencies)
{
    ServiceData * newData = new ServiceData();
    newData->name = name;
    newData->service.reset(service);

    if (!dependencies)
    {
        _services.push_front(newData);
        return;
    }

    int dependenciesCount = 0;
    Service ** temp = dependencies;
    while (*temp)
    {
        dependenciesCount++;
        temp++;
    }

    ServicesType::iterator it = _services.begin(), end_it = _services.end();
    while (it != end_it && dependenciesCount> 0)
    {
        temp = dependencies;
        while (*temp)
        {
            if ((*it)->service.get() == *temp)
            {
                dependenciesCount--;
                break;
            }

            temp++;
        }

        it++;
    }

    if (it != end_it)
        it++;

    if (dependenciesCount > 0)
    {
        GP_ERROR("Trying to register service %s which has dependency on unregistered service", name);
        return;
    }

    _services.insert(it, newData);
}

Service * ServiceManager::findService(const char * name) const
{
    for (ServicesType::const_iterator it = _services.begin(), end_it = _services.end(); it != end_it; it++)
        if ((*it)->name == name)
            return (*it)->service.get();
    return NULL;
}

void ServiceManager::shutdown()
{
    for (ServicesType::iterator it = _services.begin(), end_it = _services.end(); it != end_it; it++)
        if ((*it)->service->getState() != Service::COMPLETED)
            (*it)->service->setState(Service::SHUTTING_DOWN);

    _state = Service::SHUTTING_DOWN;
    signals.serviceManagerStateChangedEvent(_state);
    while (_state != Service::COMPLETED)
        update(_elapsedTime);

    cleanup();
}

void ServiceManager::update(float elapsedTime)
{
    _elapsedTime = elapsedTime;
    if (_state == Service::COMPLETED)
        return;

    static bool (Service::* state_funcs[]) () =
    {
        &Service::onPreInit,
        &Service::onInit,
        &Service::onTick,
        &Service::onShutdown
    };

    bool allCompleted = true;

    if (_state == Service::SHUTTING_DOWN)
    {
        // process all destruction of services in a reverse order
        for (auto it = _services.rbegin(), end_it = _services.rend(); it != end_it; it++)
        {
            const Service::State& serviceState = (*it)->service->getState();
            GP_ASSERT(serviceState >= Service::SHUTTING_DOWN);
            if (serviceState == Service::SHUTTING_DOWN)
            {
                bool res = ((*it)->service.get()->*state_funcs[serviceState])();
                if (serviceState == _state)
                    allCompleted &= res;

                if (res)
                    (*it)->service->setState(static_cast<Service::State>(serviceState + 1));
            }
        }
    }
    else
    {
        for (ServicesType::iterator it = _services.begin(), end_it = _services.end(); it != end_it; it++)
        {
            const Service::State& serviceState = (*it)->service->getState();
            if (serviceState <= _state)
            {
                bool res = ((*it)->service.get()->*state_funcs[serviceState])();
                if (serviceState == _state)
                    allCompleted &= res;

                if (res)
                    (*it)->service->setState(static_cast<Service::State>(serviceState + 1));
            }
        }
    }

    if (allCompleted)
    {
        _state = static_cast<Service::State>(_state + 1);
        signals.serviceManagerStateChangedEvent(_state);
    }
}
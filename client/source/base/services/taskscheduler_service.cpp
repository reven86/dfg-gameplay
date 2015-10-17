#include "pch.h"
#include "taskscheduler_service.h"




TaskSchedulerService::TaskSchedulerService(const ServiceManager * manager)
    : Service(manager)
    , _nextTaskHandle(0)
{
}

TaskSchedulerService::~TaskSchedulerService()
{
}

bool TaskSchedulerService::onInit()
{
    return true;
}

bool TaskSchedulerService::onShutdown()
{
    return true;
}

bool TaskSchedulerService::onTick()
{
    float time = static_cast<float>(gameplay::Game::getGameTime());
    while (!_taskQueue.empty() && _taskQueue.top().time <= time)
    {
        if (_removedTasks.find(_taskQueue.top().handle) == _removedTasks.end())
            _taskQueue.top().functor();
        else
            _removedTasks.erase(_taskQueue.top().handle);

        _taskQueue.pop();
    }

    return false;
}

unsigned TaskSchedulerService::scheduleTask(float time, const std::function<void()>& func)
{
    GP_ASSERT(time >= gameplay::Game::getGameTime());

    _nextTaskHandle++;
    _taskQueue.emplace(TaskType(time, _nextTaskHandle, func));

    return _nextTaskHandle;
}

void TaskSchedulerService::removeTask(unsigned handle)
{
    _removedTasks.insert(handle);
}
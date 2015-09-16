#pragma once

#ifndef __DFG_TASKSCHEDULER_SERVICE_H__
#define __DFG_TASKSCHEDULER_SERVICE_H__

#include "service.h"




/**
 * TaskSchedulerService is used to schedule task to run in specified time in future.
 *
 * The scheduled task is carried out in main thread. If you want to 
 * execute task on another thread, use TaskQueueService
 *
 * @see TaskQueueService
 */
class TaskSchedulerService : public Service
{
    friend class ServiceManager;

public:
    static const int INVALID_TASK_HANDLE = 0;

    static const char * getTypeName() { return "TaskSchedulerService"; };

    /**
     * Schedule task to run on specific time.
     *
     * @param time Game time to run task at.
     * @param func Task functor.
     * @return Task handle.
     * @see gameplay::Game::getGameTime
     */
    int scheduleTask(float time, const std::function<void()>& func);

    /**
     * Remove task from queue.
     *
     * @param handle Task's handle.
     */
    void removeTask(int handle);

protected:
    TaskSchedulerService(const ServiceManager * manager);
    virtual ~TaskSchedulerService();

    bool onInit();
    bool onShutdown();
    bool onTick();

private:
    struct TaskType
    {
        float time;
        int handle;
        std::function<void()> functor;

        TaskType(float t, int h, const std::function<void()>& f) : time(t), handle(h), functor(f) {};
        bool operator < (const TaskType& a) const
        {
            return time > a.time;
        }
    };

    int _nextTaskHandle;
    std::priority_queue<TaskType> _taskQueue;
    std::set<int> _removedTasks;
};



#endif // __DFG_TASKSCHEDULER_SERVICE_H__
#pragma once

#ifndef __DFG_TASKQUEUE_SERVICE_H__
#define __DFG_TASKQUEUE_SERVICE_H__

#include "service.h"
#include <condition_variable>



class TaskQueue;


/** TaskQueueService manages queues of work items.
 *
 *  Work item is a amount of work that may take some time to be executed and it's better to 
 *  run it from separate thread just to not halt the UI. For example, doing HTTP requests, 
 *  processing large amount of data are good examples of work items.
 *
 *  Each queue is carried out in separate thread.
 *
 *  The taskQueueWorkItemLoadedEvent signal is fired when the work item is about to be executed.
 *  The taskQueueWorkItemProcessedEvent signal is fired after work item has been processed.
 */
class TaskQueueService : public Service
{
    std::unordered_map<std::string, std::shared_ptr<TaskQueue> > _queues;

    // unnamed queue
    std::mutex _queueMutex;
    std::deque<std::function<void()> > _queue;

public:
    TaskQueueService(const ServiceManager * manager);
    virtual ~TaskQueueService();

    static const char * getTypeName() { return "TaskQueueService"; };

    bool onInit();
    bool onTick();
    bool onShutdown();

    /** Create named task queue. Queue runs on separate thread.
     */
    void createQueue(const char * name);

    /** Remove task queue.
     */
    void removeQueue(const char * name);

    /** Add work item to named queue.
     *
     *  Work items are executed in order they are added. The method returns
     *  work item's handle which can later be used to remove the work item
     *  from the queue in case it's not started yet.
     *
     * \param[in] queue Task queue name.
     * \param[in] func Work item functor.
     * \return Work item handle.
     */
    int addWorkItem(const char * queue, const std::function<void()>& func);

    /** Remove work item from the task queue.
     *
     * \param[in] queue Task queue name.
     * \param[in] itemHandle Work item's handle.
     */
    void removeWorkItem(const char * queue, int itemHandle);

    /** Schedule a work item to run on main thread.
     *
     *  Work item is added to internal unnamed queue which is processed
     *  during service onTick method. This method is useful when it's
     *  needed to send an event or callback from separate thread that
     *  should be dispatched on main (UI) thread. If there are several
     *  items in the queue, only one of them processed in onTick.
     *
     *  \param[in] func Work item functor.
     */
    void runOnMainThread(const std::function<void()>& func);
};




#endif // __DFG_TASKQUEUE_SERVICE_H__
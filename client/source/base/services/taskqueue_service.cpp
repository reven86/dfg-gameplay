#include "pch.h"
#include "taskqueue_service.h"
#include "service_manager.h"
#include <atomic>





class TaskQueue : Noncopyable
{
public:
    TaskQueue(const char * name, TaskQueueService * taskService);
    virtual ~TaskQueue();

    void start();
    void stop();

    int addWorkItem(const std::function<void()>& func);
    void removeWorkItem(int itemHandle);
    int getWorkItemsCount() const { return static_cast<int>(_queue.size()); }

private:
    static void threadProc(void * cookie);


    std::unique_ptr<std::thread> _workThread;
    bool _workThreadIsActive;
    std::string _name;
    TaskQueueService * _service;

    std::mutex _queueMutex;
    std::mutex _queueItemRemoveMutex;       // needed to prevent work item deletion after it started processing
    std::condition_variable _queueIsNotEmpty;
    std::deque<std::pair<int, std::function<void()> > > _queue;
    static std::atomic_int _itemCounter;
};







std::atomic_int TaskQueueService::_itemCounter;

TaskQueueService::TaskQueueService(const ServiceManager * manager)
    : Service(manager)
{
}

TaskQueueService::~TaskQueueService()
{
}

bool TaskQueueService::onInit()
{
    return true;
}

bool TaskQueueService::onTick()
{
    PROFILE("TaskQueueService::onTick", "Application");
    if (!_queue.empty())
    {
        _queueMutex.lock();
        if (!_queue.empty())
        {
            // copy functor
            std::unique_lock<std::recursive_mutex> itemRemovelock(_queueItemRemoveMutex);
            auto fn = _queue.front().second;
            _queue.pop_front();
            _queueMutex.unlock();

            // make sure functor is executed while mutex is not acquired
            fn();
        }
        else
        {
            _queueMutex.unlock();
        }
    }

    return false;
}

bool TaskQueueService::onShutdown()
{
    // first release all queues since they can invoke events on main thread
    _queues.clear();

    // now process all events on main thread
    if (!_queue.empty())
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        std::unique_lock<std::recursive_mutex> itemRemovelock(_queueItemRemoveMutex);
        while(!_queue.empty())
        {
            _queue.front().second();
            _queue.pop_front();
        }
    }

    return true;
}

void TaskQueueService::createQueue(const char * name)
{
#if !defined(__EMSCRIPTEN__)
    if (_queues.find(name) != _queues.end())
        return;

    _queues.insert(std::make_pair(std::string(name), std::shared_ptr<TaskQueue>(new TaskQueue(name, this))));
#endif
}

void TaskQueueService::removeQueue(const char * name)
{
    _queues.erase(name);
}

int TaskQueueService::addWorkItem(const char * queue, const std::function<void()>& func)
{
    if (!queue)
        return runOnMainThread(func);

#if defined(__EMSCRIPTEN__)
    // emscripten does not fully support multithreading and conditional variables
    runOnMainThread(func);
    return -1;
#else
    auto it = _queues.find(queue);
    if (it == _queues.end())
        return -1;

    return (*it).second->addWorkItem(func);
#endif
}

void TaskQueueService::removeWorkItem(const char * queue, int itemHandle)
{
    if (queue)
    {
        auto it = _queues.find(queue);
        if (it == _queues.end())
            return;

        (*it).second->removeWorkItem(itemHandle);
        return;
    }

    // remove item from main thread queue
    std::unique_lock<std::mutex> lock(_queueMutex);
    std::unique_lock<std::recursive_mutex> itemRemoveLock(_queueItemRemoveMutex);

    auto it = std::find_if(_queue.begin(), _queue.end(), [&itemHandle](const std::pair<int, std::function<void()> >& a) {return a.first == itemHandle; });
    if (it == _queue.end())
        return;

    _queue.erase(it);
}

int TaskQueueService::runOnMainThread(const std::function<void()>& func)
{
    std::unique_lock<std::mutex> lock(_queueMutex);

    _itemCounter++;
    _queue.push_back(std::make_pair(_itemCounter.load(), func));

    return _itemCounter;
}

int TaskQueueService::getWorkItemsCount(const char * queue) const 
{
    // no need to make synchronization here
    // result may be not accurate by design
    if (queue)
    {
        auto it = _queues.find(queue);
        if (it == _queues.end())
            return 0;
        return (*it).second->getWorkItemsCount();
    }

    return static_cast<int>(_queue.size());
}






//
// TaskQueue
//

std::atomic_int TaskQueue::_itemCounter;

TaskQueue::TaskQueue(const char * name, TaskQueueService * taskService)
    : _workThreadIsActive(true)
    , _name(name)
    , _service(taskService)
{
    start();
}

TaskQueue::~TaskQueue()
{
    stop();
}

void TaskQueue::start()
{
    GP_ASSERT(_workThread.get() == nullptr);

    _workThreadIsActive = true;
    _workThread.reset(new std::thread(&TaskQueue::threadProc, this));
}

void TaskQueue::stop()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        _workThreadIsActive = false;
        _queueIsNotEmpty.notify_one();
    }

    if (_workThread && _workThread->joinable())
        _workThread->join();
    _workThread.reset();
}

int TaskQueue::addWorkItem(const std::function<void()>& func)
{
    std::unique_lock<std::mutex> lock(_queueMutex);

    _itemCounter++;
    _queue.push_back(std::make_pair(_itemCounter.load(), func));
    _queueIsNotEmpty.notify_one();

    return _itemCounter;
}

void TaskQueue::removeWorkItem(int itemHandle)
{
    std::unique_lock<std::mutex> lock(_queueMutex);
    std::unique_lock<std::mutex> itemRemoveLock(_queueItemRemoveMutex);

    auto it = std::find_if(_queue.begin(), _queue.end(), [&itemHandle](const std::pair<int, std::function<void()> >& a){return a.first == itemHandle; });
    if (it == _queue.end())
        return;

    _queue.erase(it);
}

void TaskQueue::threadProc(void * cookie)
{
    TaskQueue * _this = reinterpret_cast<TaskQueue *>(cookie);

    _this->_service->runOnMainThread([_this](){ ServiceManager::getInstance()->signals.taskQueueStartedEvent(_this->_name.c_str()); });    
    while (_this->_workThreadIsActive)
    {
        std::pair<int, std::function<void()> > item;

        {
            std::unique_lock<std::mutex> lock(_this->_queueMutex);
            while (_this->_queue.empty() && _this->_workThreadIsActive)
                _this->_queueIsNotEmpty.wait(lock);
            if (!_this->_workThreadIsActive)
                break;
            _this->_queueItemRemoveMutex.lock();
            item = _this->_queue.front();
            _this->_queue.pop_front();
        }

        _this->_service->runOnMainThread([&item, _this]() { ServiceManager::getInstance()->signals.taskQueueWorkItemLoadedEvent(_this->_name.c_str(), item.first); });
        item.second();
        _this->_queueItemRemoveMutex.unlock();
        _this->_service->runOnMainThread([&item, _this](){ ServiceManager::getInstance()->signals.taskQueueWorkItemProcessedEvent(_this->_name.c_str(), item.first); });
    }
    _this->_service->runOnMainThread([_this](){ ServiceManager::getInstance()->signals.taskQueueStoppedEvent(_this->_name.c_str()); });
}
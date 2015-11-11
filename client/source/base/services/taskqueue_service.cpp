#include "pch.h"
#include "taskqueue_service.h"
#include "service_manager.h"
#include <atomic>





class TaskQueue : Noncopyable
{
private:
    std::unique_ptr<std::thread> _workThread;
    bool _workThreadIsActive;
    std::string _name;
    TaskQueueService * _service;

    std::mutex _queueMutex;
    std::condition_variable _queueIsNotEmpty;
    std::deque<std::pair<int, std::function<void()> > > _queue;
    static std::atomic_int _itemCounter;

public:
    TaskQueue(const char * name, TaskQueueService * taskService);
    virtual ~TaskQueue();

    void start();
    void stop();

    int addWorkItem(const std::function<void()>& func);
    void removeWorkItem(int itemHandle);

private:
    static void threadProc(void * cookie);
};







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
    if (!_queue.empty())
    {
        _queueMutex.lock();
        if (!_queue.empty())
        {
            // copy functor
            auto fn = _queue.front();
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
        while(!_queue.empty())
        {
            _queue.front()();
            _queue.pop_front();
        }
    }

    return true;
}

void TaskQueueService::createQueue(const char * name)
{
    if (_queues.find(name) != _queues.end())
        return;

    _queues.insert(std::make_pair(std::string(name), std::shared_ptr<TaskQueue>(new TaskQueue(name, this))));
}

void TaskQueueService::removeQueue(const char * name)
{
    _queues.erase(name);
}

int TaskQueueService::addWorkItem(const char * queue, const std::function<void()>& func)
{
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
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
    auto it = _queues.find(queue);
    if (it == _queues.end())
        return;

    (*it).second->removeWorkItem(itemHandle);
}

void TaskQueueService::runOnMainThread(const std::function<void()>& func)
{
    std::unique_lock<std::mutex> lock(_queueMutex);
    _queue.push_back(func);
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
            item = _this->_queue.front();
            _this->_queue.pop_front();
        }

        _this->_service->runOnMainThread([&item](){ ServiceManager::getInstance()->signals.taskQueueWorkItemLoadedEvent(item.first); });
        item.second();
        _this->_service->runOnMainThread([&item](){ ServiceManager::getInstance()->signals.taskQueueWorkItemProcessedEvent(item.first); });
    }
    _this->_service->runOnMainThread([_this](){ ServiceManager::getInstance()->signals.taskQueueStoppedEvent(_this->_name.c_str()); });
}
#pragma once

#ifndef __DFG_HTTP_REQUEST_SERVICE_H__
#define __DFG_HTTP_REQUEST_SERVICE_H__

#include "services/service.h"
#include "services/taskqueue_service.h"





/** 
 * HTTRPrequestService performs GET or POST request and returns
 * response as well as CURL error code in case of any error.
 * This service works on top of TaskQueueService, it creates
 * a new dedicated queue and send request asynchorniously.
 */
class HTTPRequestService : public Service
{
    friend class ServiceManager;

public:
    /**
     * Request structure used to send HTTP requests.
     */
    struct Request
    {
        typedef std::vector<std::pair<std::string, std::string> > HeadersList;

        std::string url;
        std::string postPayload;
        HeadersList headers;
        std::function<void(int, std::vector<uint8_t>)> responseCallback;
    };

    static const char * getTypeName() { return "HTTPRequestService"; };

    /** 
     * Get queue name for HTTPRequestService.
     */
    const char * getTaskQueueName() const;

    /**
     * Create HTTP request and add it to the outgoing queue.
     *
     * Note: Response callback is called from the main thread, so
     * there is no need to use synchonization primitives.
     *
     * @param[in] request Request data.
     * @return Work item handle.
     */
    int makeRequestAsync(const Request& request);

    /** 
     * Create HTTP request and execute it immediately from calling thread.
     *
     * @param[in] request Request data.
     * @param[in] responseCallback Callback functor when response is received.
     */
    void makeRequestSync(const Request& request);

    /**
     * Get error string for error codes.
     *
     * @param[in] code Error code.
     * @return Error string.
     */
    static const char * getErrorString(int code);

protected:
    HTTPRequestService(const ServiceManager * manager);
    virtual ~HTTPRequestService();

    virtual bool onInit();
    virtual bool onTick();
    virtual bool onShutdown();

private:
    void sendRequest(const Request& request);
    static size_t writeFunction(void *contents, size_t size, size_t nmemb, void *userp);

    void * _curl;

    TaskQueueService * _taskQueueService;
    std::mutex _requestProcessingMutex;
};



#endif // __DFG_HTTP_REQUEST_SERVICE_H__
#pragma once

#ifndef __DFG_HTTP_REQUEST_SERVICE_H__
#define __DFG_HTTP_REQUEST_SERVICE_H__

#include "services/service.h"
#include "services/taskqueue_service.h"





/** HTTRPrequestService performs GET or POST request and returns
 *  response as well as CURL error code in case of any error.
 *  This service works on top of TaskQueueService, it creates
 *  a new dedicated queue and send request asynchorniously.
 */
class HTTPRequestService : public Service
{
    void * _curl;

    struct Request
    {
        std::string url;
        std::string postPayload;
        std::function<void(int, const std::string&)> responseCallback;
    };

    TaskQueueService * _taskQueueService;
    std::string _response;

public:
    HTTPRequestService(const ServiceManager * manager);
    virtual ~HTTPRequestService();

    static const char * getTypeName() { return "HTTPRequestService"; };

    bool onInit();
    bool onTick();
    bool onShutdown();

    /** Get queue name for HTTPRequestService.
     */
    const char * getTaskQueueName() const;

    /** Create HTTP request and add it to the outgoing queue.
     *
     *  Note: Response callback is called from the main thread, so
     *  there is no need to use synchonization primitives.
     *
     *  \param[in] url URL to send request to.
     *  \param[in] payload Payload for POST requests or NULL.
     *  \param[in] responseCallback Callback functor when response is received.
     *  \return Work item handle.
     */
    int makeRequestAsync(const char * url, const char * payload, const std::function<void(int, const std::string&)>& responseCallback);

    /** Create HTTP request and execute it immediately from calling thread.
     *
     *  \param[in] url URL to send request to.
     *  \param[in] payload Payload for POST requests or NULL.
     *  \param[in] responseCallback Callback functor when response is received.
     */
    void makeRequestSync(const char * url, const char * payload, const std::function<void(int, const std::string&)>& responseCallback);

private:
    void sendRequest(const Request& request);
    static size_t writeFunction(void *contents, size_t size, size_t nmemb, void *userp);
};



#endif // __DFG_HTTP_REQUEST_SERVICE_H__
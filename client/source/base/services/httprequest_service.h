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
     * Response is returned via responseCallback which accepts error code, a newly created Stream with response data and a error string.
     * Client application is responsible to deallocate a stream with SAFE_DELETE macro.
     */
    struct Request
    {
        typedef std::vector<std::pair<std::string, std::string> > HeadersList;

        std::string url;
        std::string postPayload;
        HeadersList headers;
        std::function<void(int, class MemoryStream *, const char *, long)> responseCallback;

        // progress callback, can be invoked from separate thread
        // agruments and return value match the ones of CURLOPT_XFERINFOFUNCTION
        std::function<bool(uint64_t, uint64_t, uint64_t, uint64_t)> progressCallback;
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
     * @param[in] headOnly Make only HEAD request.
     * @return Work item handle.
     */
    int makeRequestAsync(const Request& request, bool headOnly = false);

    /** 
     * Create HTTP request and execute it immediately from calling thread.
     *
     * @param[in] request Request data.
     * @param[in] headOnly Make only HEAD request.
     * @param[in] responseCallback Callback functor when response is received.
     */
    void makeRequestSync(const Request& request, bool headOnly = false);

protected:
    HTTPRequestService(const ServiceManager * manager);
    virtual ~HTTPRequestService();

    virtual bool onInit();
    virtual bool onTick();
    virtual bool onShutdown();

private:
    void sendRequest(const Request& request, bool headOnly);
    static size_t writeFunction(void *contents, size_t size, size_t nmemb, void *userp);
    static void requestLoadCallback(unsigned, void * arg, void *buf, unsigned length);
    static void requestErrorCallback(unsigned, void * arg, int errorCode, const char * status);
    static void requestProgressCallback(unsigned, void * arg, int dlnow, int dltotal);

    void * _curl;

    TaskQueueService * _taskQueueService;
    std::mutex _requestProcessingMutex;
};



#endif // __DFG_HTTP_REQUEST_SERVICE_H__
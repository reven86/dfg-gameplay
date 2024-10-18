#pragma once

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
     * Response is returned via responseCallback which accepts error code, a Stream with response data and a error string.
     */
    struct Request
    {
        typedef std::vector<std::pair<std::string, std::string> > HeadersList;

        std::string url;
        std::string postPayload;
        HeadersList headers;
        std::function<void(int, class MemoryStream *, const char *, long)> responseCallback;    // error code, response, error, http response

        // progress callback, can be invoked from separate thread
        // agruments and return value match the ones of CURLOPT_XFERINFOFUNCTION (dltotal, dlnow, ultotal, ulnow)
        // return True from callback to abort the downloading or uploading
        std::function<bool(uint64_t, uint64_t, uint64_t, uint64_t)> progressCallback;
    };

    static const char * getTypeName() { return "HTTPRequestService"; };

    /** 
     * Get queue name for HTTPRequestService.
     */
    static const char * getTaskQueueName();

    /**
     * Create HTTP request and add it to the outgoing queue.
     *
     * Note: Response callback is called from the main thread, so
     * there is no need to use synchonization primitives.
     *
     * @param[in] request Request data.
     * @param[in] customRequest Custom request type (HEAD, PATCH, DELETE)
     * @param[in] withCredentials Send credentials (cookies) on emscripten.
     * @return Work item handle.
     */
    int makeRequestAsync(const Request& request, const char * customRequest = NULL, bool withCredentials = false);

    /** 
     * Create HTTP request and execute it immediately from calling thread.
     *
     * @param[in] request Request data.
     * @param[in] customRequest Custom request type (PATCH, DELETE)
     * @param[in] withCredentials Send credentials (cookies) on emscripten.
     */
    void makeRequestSync(const Request& request, const char * customRequest = NULL, bool withCredentials = false);

    /**
     * Get whether or not any of HTTP requests is currently in process.
     */
    bool hasActiveEmscriptenHTTPRequest() const;

protected:
    HTTPRequestService(const ServiceManager * manager);
    virtual ~HTTPRequestService();

    virtual bool onPreInit() override;
    virtual bool onInit() override;
    virtual bool onTick() override;
    virtual bool onShutdown() override;

private:
    void sendRequest(const Request& request, bool syncCall = false, std::string customRequest = "", bool withCredentials = false);
    static size_t writeFunction(void *contents, size_t size, size_t nmemb, void *userp);
    static void requestLoadCallback(unsigned, void * arg, void *buf, unsigned length, int statusCode, const char * status);
    static void requestErrorCallback(unsigned, void * arg, int statusCode, const char * status);
    static void requestProgressCallback(unsigned, void * arg, int dlnow, int dltotal);

    TaskQueueService * _taskQueueService;
};

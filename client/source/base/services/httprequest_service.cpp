#include "pch.h"
#include "httprequest_service.h"
#include "service_manager.h"
#include "main/memory_stream.h"
#include "curl/curl.h"





#ifdef __EMSCRIPTEN__
extern "C"
{
    extern int emscripten_async_wget3_data(const char* url, const char* requesttype, const char* param, const char* additionalHeader, void *arg, int free, em_async_wget2_data_onload_func onload, em_async_wget2_data_onerror_func onerror, em_async_wget2_data_onprogress_func onprogress);
}
#endif

static char errorBuffer[CURL_ERROR_SIZE];
const char * HTTP_REQUEST_SERVICE_QUEUE = "HTTPRequestServiceQueue";

HTTPRequestService::HTTPRequestService(const ServiceManager * manager)
    : Service(manager)
    , _curl(NULL)
    , _taskQueueService(NULL)
{
}

HTTPRequestService::~HTTPRequestService()
{
}

bool HTTPRequestService::onInit()
{
    _taskQueueService = _manager->findService<TaskQueueService>();
    _taskQueueService->createQueue(HTTP_REQUEST_SERVICE_QUEUE);

#ifndef __EMSCRIPTEN__
    _curl = curl_easy_init();
    if (_curl)
    {
        curl_easy_setopt(_curl, CURLOPT_USERAGENT, gameplay::Game::getInstance()->getUserAgentString());
        //curl_easy_setopt( _curl, CURLOPT_DNS_CACHE_TIMEOUT, -1 );
        curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 20);
        curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 5);
        curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(_curl, CURLOPT_TCP_NODELAY, 1);  // make sure packets are sent immediately
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &writeFunction);
        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0);
    }
#endif

    return true;
}

bool HTTPRequestService::onTick()
{
    // service has no work to do every frame
    return true;
}

bool HTTPRequestService::onShutdown()
{
    if (_taskQueueService)
        _taskQueueService->removeQueue(HTTP_REQUEST_SERVICE_QUEUE);

#ifndef __EMSCRIPTEN__
    if (_curl)
        curl_easy_cleanup(_curl);
#endif

    return true;
}

int HTTPRequestService::makeRequestAsync(const Request& request)
{
    // note: request is copied by value
    return _taskQueueService->addWorkItem(HTTP_REQUEST_SERVICE_QUEUE, std::bind(&HTTPRequestService::sendRequest, this, request));
}

void HTTPRequestService::makeRequestSync(const Request& request)
{
    sendRequest(request);
}

void HTTPRequestService::requestLoadCallback(unsigned, void * arg, void *buf, unsigned length)
{
    Request * request = reinterpret_cast<Request *>(arg);
    request->responseCallback(CURLE_OK, MemoryStream::create(buf, length), NULL);
    delete request;
}

void HTTPRequestService::requestErrorCallback(unsigned, void *arg, int errorCode, const char * status)
{
    Request * request = reinterpret_cast<Request *>(arg);
    GP_LOG("Failed to perform HTTP request to %s: error %d %s", request->url.c_str(), errorCode, status);
    request->responseCallback(errorCode, NULL, status);
    delete request;
}

void HTTPRequestService::sendRequest(const Request& request)
{
    // make sure curl is used only for one thread in any moment
    std::unique_lock<std::mutex> lock(_requestProcessingMutex);

#ifndef __EMSCRIPTEN__
    MemoryStream * response = MemoryStream::create();

    curl_easy_setopt(_curl, CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, request.postPayload.c_str());
    curl_easy_setopt(_curl, CURLOPT_POST, request.postPayload.empty() ? 0 : 1);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, response);

    struct curl_slist *list = NULL;
    if (!request.headers.empty())
    {
        for (auto& h : request.headers)
            list = curl_slist_append(list, (h.first + ": " + h.second).c_str());
    }
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, list);

    CURLcode res = curl_easy_perform(_curl);
    curl_slist_free_all(list);

    if (res != CURLE_OK)
        GP_LOG("Failed to perform HTTP request: error %d - %s", res, request.url.c_str());

    // response is copied by value since callback is invoked on main thread
    _taskQueueService->runOnMainThread(std::bind(request.responseCallback, res, response, curl_easy_strerror(res)));

#else

    std::string additionalHeaders;  // headers in JSON format
    if (!request.headers.empty())
    {
        additionalHeaders += "{";
        for (auto& h : request.headers)
        {
            additionalHeaders += "\"";
            additionalHeaders += h.first;
            additionalHeaders += "\":\"";
            additionalHeaders += h.second;
            additionalHeaders += "\",";
        }
        additionalHeaders.pop_back();
        additionalHeaders += "}";
    }

    Request * newRequest = new Request(request);

    emscripten_async_wget3_data(request.url.c_str(), request.postPayload.empty() ? "GET" : "POST", request.postPayload.c_str(),
        additionalHeaders.c_str(), newRequest, true, &HTTPRequestService::requestLoadCallback, &HTTPRequestService::requestErrorCallback, NULL);

#endif
}

size_t HTTPRequestService::writeFunction(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realSize = size * nmemb;
    gameplay::Stream * response = reinterpret_cast<gameplay::Stream *>(userp);

    response->write(contents, size, nmemb);

    return realSize;
}

const char * HTTPRequestService::getTaskQueueName() const
{
    return HTTP_REQUEST_SERVICE_QUEUE;
}
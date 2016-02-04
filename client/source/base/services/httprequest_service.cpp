#include "pch.h"
#include "httprequest_service.h"
#include "service_manager.h"
#include "curl/curl.h"






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
        curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1);
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

int HTTPRequestService::makeRequestAsync(const char * url, const char * payload, const std::function<void(int, const std::string&)>& responseCallback)
{
    Request request;
    request.url = url;
    if (payload)
        request.postPayload = payload;
    request.responseCallback = responseCallback;

    // note: request is copied by value
    return _taskQueueService->addWorkItem(HTTP_REQUEST_SERVICE_QUEUE, std::bind(&HTTPRequestService::sendRequest, this, request));
}

void HTTPRequestService::makeRequestSync(const char * url, const char * payload, const std::function<void(int, const std::string&)>& responseCallback)
{
    Request request;
    request.url = url;
    if (payload)
        request.postPayload = payload;
    request.responseCallback = responseCallback;

    sendRequest(request);
}

void HTTPRequestService::sendRequest(const Request& request)
{
    // make sure curl is used only for one thread in any moment
    std::unique_lock<std::mutex> lock(_requestProcessingMutex);

    _response.clear();
    
#ifndef __EMSCRIPTEN__
    curl_easy_setopt(_curl, CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, request.postPayload.c_str());
    curl_easy_setopt(_curl, CURLOPT_POST, request.postPayload.empty() ? 0 : 1);

    CURLcode res = curl_easy_perform(_curl);
#else
    CURLcode res = CURLE_FAILED_INIT;
#endif
    
    // response is copied by value since callback is invoked on main thread
    _taskQueueService->runOnMainThread(std::bind(request.responseCallback, res, _response));
}

size_t HTTPRequestService::writeFunction(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realSize = size * nmemb;
    HTTPRequestService * _this = reinterpret_cast<HTTPRequestService *>(userp);

    _this->_response += std::string(reinterpret_cast<const char *>(contents), realSize);

    return realSize;
}

const char * HTTPRequestService::getTaskQueueName() const
{
    return HTTP_REQUEST_SERVICE_QUEUE;
}
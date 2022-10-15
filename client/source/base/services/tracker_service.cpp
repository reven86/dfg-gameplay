#include "pch.h"
#include "tracker_service.h"
#include "service_manager.h"
#include "main.h"
#include "curl/curl.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif



static const char * __endpoint = "https://www.google-analytics.com/collect";

bool TrackerService::_threadForceQuit = false;
int TrackerService::_dispatchPeriod = 1000;


size_t writeFunction(void *contents, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
}



TrackerService::TrackerService(const ServiceManager * manager)
    : Service(manager)
    , _trackingId()
    , _clientId()
    , _protocolVersion(1)
    , _curl(NULL)
    , _allowDispatch(true)
    , _sessionStartTime(-1.0f)
    , _lastPayloadSentTime(0)
{
    memset(_customMetrics, 0, sizeof(_customMetrics));
    time(&_trackerStartTime);
}

TrackerService::~TrackerService()
{
}

static char errorBuffer[CURL_ERROR_SIZE];
void TrackerService::setupTracking(const char * gaAppId, const char * clientId, const char * startScreen)
{
    _trackingId = gaAppId;
    _clientId = clientId;

    _appName = Utils::urlEncode(gameplay::Game::getInstance()->getConfig()->getNamespace("window", true)->getString("title"));
    _appVersion = Utils::urlEncode(gameplay::Game::getInstance()->getConfig()->getNamespace("window", true)->getString("version"));

    // load previously saved payloads which weren't sent to server
    std::string filename = std::string(static_cast<DfgGame *>(gameplay::Game::getInstance())->getUserDataFolder()) + "/payloads.dat";
    gameplay::Stream * stream(gameplay::FileSystem::open(filename.c_str(), gameplay::FileSystem::READ));

    if (stream)
    {
        int32_t version = 0;
        stream->read(&version, sizeof(version), 1);

        if (version == 2)
        {
            time_t lastTrackerStartTime;
            stream->read(&lastTrackerStartTime, sizeof(lastTrackerStartTime), 1);

            int32_t payloadsCount = 0;
            stream->read(&payloadsCount, sizeof(payloadsCount), 1);

            int payloadDiffInSeconds = static_cast<int>(difftime(_trackerStartTime, lastTrackerStartTime));

            for (int i = 0; i < payloadsCount; i++)
            {
                int32_t size = 0;
                static char params[1024];

                stream->read(&size, sizeof(size), 1);
                size = std::min(1023, size);

                stream->read(params, size, 1);
                params[size] = '\0';

                PayloadInfo payload;
                payload.params = params;

                stream->read(&payload.createTime, sizeof(payload.createTime), 1);

                // adjust payload's createTime by diff between runs
                payload.createTime -= static_cast<float>(payloadDiffInSeconds);

                // filter out too old payloads (older thah 4h)
                if (payload.createTime > -4.0f * 3600.0f)
                    _payloadsQueue.push_back(payload);
            }
        }

        SAFE_DELETE(stream);
    }

#ifndef __EMSCRIPTEN__
    _curl = curl_easy_init();
    if (_curl)
    {
        curl_easy_setopt(_curl, CURLOPT_URL, __endpoint);
        curl_easy_setopt(_curl, CURLOPT_POST, 1);
        curl_easy_setopt(_curl, CURLOPT_USERAGENT, gameplay::Game::getInstance()->getUserAgentString());
        //curl_easy_setopt( _curl, CURLOPT_DNS_CACHE_TIMEOUT, -1 );
        curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 4);
        curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 2);
        curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(_curl, CURLOPT_TCP_NODELAY, 1);  // make sure packets are sent immediately
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &writeFunction);
        curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0);
    }
#endif

    if (!_payloadsQueue.empty())
    {
        // make sure previous session is ended
        endSession();
    }

    _currentView = Utils::urlEncode(startScreen);
    sendData("t=appview&cd=%s",
        _currentView.c_str()
        );
}

bool TrackerService::onInit()
{
#ifndef __EMSCRIPTEN__
    _dispatchThread.reset(new std::thread(&TrackerService::dispatchThreadProc, this));
#endif
    return true;
}

bool TrackerService::onShutdown()
{
    // try to empty payloads queue, thread will exit right after sleep.
    endSession();

    if (_dispatchThread.get())
    {
        _dispatchMutex.lock();
        _threadForceQuit = true;
        _dispatchMutex.unlock();
        _dispatchThread->join();
    }

#ifndef __EMSCRIPTEN__
    if (_curl)
        curl_easy_cleanup(_curl);
#endif

    _payloadsQueue.clear();

    return true;
}

void TrackerService::flushPayloads()
{
#ifndef __EMSCRIPTEN__
    std::string filename = std::string(static_cast<DfgGame *>(gameplay::Game::getInstance())->getUserDataFolder()) + "/payloads.dat";
    gameplay::Stream * stream(gameplay::FileSystem::open(filename.c_str(), gameplay::FileSystem::WRITE));

    if (stream)
    {
        int32_t version = 2;
        stream->write(&version, sizeof(version), 1);
        stream->write(&_trackerStartTime, sizeof(_trackerStartTime), 1);

        int32_t maxPayloadsToStore = std::min<int32_t>(1024, static_cast<int32_t>(_payloadsQueue.size()));
        stream->write(&maxPayloadsToStore, sizeof(maxPayloadsToStore), 1);
        _payloadQueueMutex.lock();
        std::deque< PayloadInfo >::iterator it = _payloadsQueue.begin();
        while (maxPayloadsToStore-- > 0)
        {
            GP_ASSERT(it != _payloadsQueue.end());
            const PayloadInfo& payload = *it;

            int32_t size = static_cast<int32_t>(payload.params.size());
            stream->write(&size, sizeof(size), 1);
            stream->write(payload.params.c_str(), size, 1);
            stream->write(&payload.createTime, sizeof(payload.createTime), 1);

            it++;
        }
        _payloadQueueMutex.unlock();

        SAFE_DELETE(stream);
    }
#endif
}

bool TrackerService::onTick()
{
    return false; // as long as service is active dispatch thread is working
}

void TrackerService::forceDispatch()
{
#ifndef __EMSCRIPTEN__
    _dispatchMutex.lock();

    // if thread is forced to quit, _payloadsQueue may be invalid
    if (_threadForceQuit)
    {
        _dispatchMutex.unlock();
        return;
    }

    while (!_payloadsQueue.empty())
    {
        // get a copy of payload because queue can be modified in another thread
        PayloadInfo payload = _payloadsQueue.front();

        if (!dispatch(payload))
            break;

        _payloadQueueMutex.lock();
        _payloadsQueue.pop_front();
        _payloadQueueMutex.unlock();
    }
    _dispatchMutex.unlock();
#endif
}

void TrackerService::endSession(const char * viewName)
{
    _dispatchMutex.lock();
    if (_payloadsQueue.empty() || _payloadsQueue.back().params.find("&sc=end") == _payloadsQueue.back().params.npos)
    {
        if (_currentView != viewName)
        {
            sendView(viewName);
            _payloadsQueue.back().params += "&sc=end";
        }
    }
    _dispatchMutex.unlock();

    forceDispatch();

    _dispatchMutex.lock();
    _sessionStartTime = -1.0f;
    _dispatchMutex.unlock();

    // if payload queue still not empty (e.g., internet connection was lost)
    // store it offline to send payloads on next run
    flushPayloads();
}

bool TrackerService::dispatch(const PayloadInfo& payload)
{
    if (!_allowDispatch)
        return false;

    float curTime = static_cast<float>(gameplay::Game::getInstance()->getAbsoluteTime());

    int timeDiff = static_cast<int>((curTime - payload.createTime) * 1000.0f);
    std::string finalRequest = Utils::format("v=%d&tid=%s&cid=%s&an=%s&av=%s&%s",
        _protocolVersion, _trackingId.c_str(), _clientId.c_str(),
        _appName.c_str(),
        _appVersion.c_str(),
        payload.params.c_str());

    if (!_userId.empty())
        finalRequest += Utils::format("&uid=%s", _userId.c_str());

    // reset session when time between payloads passed midnight
    time_t time1 = _trackerStartTime + static_cast<int>(_lastPayloadSentTime);
    time_t time2 = _trackerStartTime + static_cast<int>(curTime);
    int day1 = localtime(&time1)->tm_mday;
    int day2 = localtime(&time2)->tm_mday;

    if (payload.createTime >= 0 && payload.params.find("&sc=end") == payload.params.npos && gameplay::Game::getInstance() &&
        (_sessionStartTime < 0.0f
        || (payload.createTime - _sessionStartTime) > 240.0f * 60.0f      // sessions in GA are automaticaly ended after some period of time
        || day1 != day2)
        )
    {
        // send language and resolution for first payload
        // assuming the first payload starts a session
        int width = gameplay::Game::getInstance()->getWidth();
        int height = gameplay::Game::getInstance()->getHeight();

        finalRequest += Utils::format("&sr=%dx%d&ul=%s&sc=start",
            width, height,
            static_cast<DfgGame *>(gameplay::Game::getInstance())->getGameLocale()
            );
    }

    if (timeDiff > 0)
        finalRequest += Utils::format("&qt=%d", timeDiff);

#ifndef __EMSCRIPTEN__
    
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, finalRequest.c_str());
    
    CURLcode res = curl_easy_perform(_curl);
    
#else
    
    emscripten_async_wget2_data(__endpoint, "POST", finalRequest.c_str(), NULL, 1, NULL, NULL, NULL);
    
    // do not wait for response
    CURLcode res = CURLE_OK;

#endif

    if (res == CURLE_OK)
    {
        if (_sessionStartTime < 0.0f)
            _sessionStartTime = payload.createTime;

        _lastPayloadSentTime = payload.createTime;
    }

#ifdef _DEBUG
    time_t seconds;
    time(&seconds);
    tm * t = localtime(&seconds);

    GP_LOG(
        "%02d-%02d-%02d %02d:%02d:%02d %08d %08d GA: %s - %d\n",
        t->tm_mon + 1, t->tm_mday, t->tm_year % 100, t->tm_hour, t->tm_min, t->tm_sec,
        static_cast<int>(curTime * 1000.0f), static_cast<int>(payload.createTime * 1000.0f), finalRequest.c_str(), res);

    GP_ASSERT(abs(static_cast<int>(curTime * 1000.0f)-timeDiff - static_cast<int>(payload.createTime * 1000.0f)) < 2);
#endif

    return res == CURLE_OK;
}

void TrackerService::sendView(const char * screenName)
{
    std::string newView(Utils::urlEncode(screenName));
    if (_currentView == newView)
        return;

    _currentView = newView;
    sendData("t=appview&cd=%s", _currentView.c_str());
}

void TrackerService::sendEvent(const char * category, const char * action, const char * label, const int& value, bool interactive)
{
    sendData("t=event&ec=%s&ea=%s&el=%s&ev=%u&ni=%d&cd=%s",
        Utils::urlEncode(category).c_str(),
        Utils::urlEncode(action).c_str(),
        Utils::urlEncode(label).c_str(),
        value,
        interactive ? 0 : 1,
        _currentView.c_str()
        );
}

void TrackerService::sendSocialEvent(const char * network, const char * action, const char * target)
{
    sendData("t=social&sn=%s&sa=%s&st=%s&cd=%s",
        Utils::urlEncode(network).c_str(),
        Utils::urlEncode(action).c_str(),
        Utils::urlEncode(target).c_str(),
        _currentView.c_str()
        );
}

void TrackerService::sendTiming(const char * category, const int& timeMs, const char * name, const char * label)
{
    sendData("t=timing&utc=%s&utv=%s&utt=%d&utl=%s&cd=%s",
        Utils::urlEncode(category).c_str(),
        name == NULL ? "" : Utils::urlEncode(name).c_str(),
        timeMs,
        label == NULL ? "" : Utils::urlEncode(label).c_str(),
        _currentView.c_str()
        );
}

void TrackerService::sendException(const char * type, bool isFatal)
{
    sendData("t=exception&exd=%s&exf=%d&cd=%s",
        Utils::urlEncode(type).c_str(),
        isFatal ? 1 : 0,
        _currentView.c_str()
        );
}

void TrackerService::sendTransaction(const char * transactionID, const char * store, float totalRevenue, float shippingCost, float tax, const char * currencyCode)
{
    sendData("t=transaction&ti=%s&ta=%s&tr=%.2f&ts=%.2f&tt=%.2f&cu=%s",
        Utils::urlEncode(transactionID).c_str(),
        Utils::urlEncode(store).c_str(),
        totalRevenue,
        shippingCost,
        tax,
        Utils::urlEncode(currencyCode).c_str()
        );
}

void TrackerService::sendItem(const char * transactionID, const char * name, float price, int quantity, const char * sku, const char * category, const char * currencyCode)
{
    sendData("t=item&ti=%s&in=%s&ip=%.2f&iq=%d&ic=%s&iv=%s&cu=%s",
        Utils::urlEncode(transactionID).c_str(),
        Utils::urlEncode(name).c_str(),
        price,
        quantity,
        Utils::urlEncode(sku).c_str(),
        Utils::urlEncode(category).c_str(),
        Utils::urlEncode(currencyCode).c_str()
        );
}

void TrackerService::setCustomDimension(const int& id, const char * dimension)
{
    if (id < 0 || id > MAX_CUSTOM_DIMENSIONS)
        return;

    _customDimensions[id] = dimension;
}

void TrackerService::setCustomMetric(const int& id, const int& metric)
{
    if (id < 0 || id > MAX_CUSTOM_METRICS)
        return;

    _customMetrics[id] = metric;
}

void TrackerService::sendData(const char * format, ...)
{
    va_list args;
    va_start(args, format);

    char request[4096];
    vsnprintf(request, 4095, format, args);
    request[4095] = '\0';

    int i;
    for (i = 0; i < MAX_CUSTOM_DIMENSIONS; i++)
        if (!_customDimensions[i].empty())
        {
            char buf[256];
            sprintf(buf, "&cd%d=%s", i + 1, Utils::urlEncode(_customDimensions[i]).c_str());
            strncat(request, buf, 4095);
        }

    for (i = 0; i < MAX_CUSTOM_METRICS; i++)
        if (_customMetrics[i] > 0)
        {
            char buf[256];
            sprintf(buf, "&cm%d=%d", i + 1, _customMetrics[i]);
            strncat(request, buf, 4095);
        }

    PayloadInfo payload;
    payload.params = request;
    payload.createTime = static_cast<float>(gameplay::Game::getInstance()->getAbsoluteTime());

#ifndef __EMSCRIPTEN__
    _payloadQueueMutex.lock();
    _payloadsQueue.push_back(payload);
    _payloadQueueMutex.unlock();
#else
    dispatch(payload);
#endif
    
    //memset(_customMetrics, 0, sizeof(_customMetrics));
    //for (i = 0; i < MAX_CUSTOM_DIMENSIONS; i++)
    //    _customDimensions[i].clear();
}

void TrackerService::setUserId(const char * userId)
{
    _userId = userId ? userId : "";
}

void TrackerService::dispatchThreadProc(void * arg)
{
    TrackerService * _this = reinterpret_cast<TrackerService *>(arg);

    if (!_this->_curl)
        return;

    while (!_this->_threadForceQuit && (_this->getState() == Service::INITIALIZING || _this->getState() == Service::RUNNING))
    {
        _this->forceDispatch();
        std::this_thread::sleep_for(std::chrono::milliseconds(_this->_dispatchPeriod));
    }
}
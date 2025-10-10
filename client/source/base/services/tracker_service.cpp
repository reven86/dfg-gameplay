#include "pch.h"
#include "tracker_service.h"
#include "service_manager.h"
#include "httprequest_service.h"
#include "main.h"
#include <iomanip>

#if defined(__ANDROID__) || defined(__APPLE__) && TARGET_OS_IPHONE
#define FIREBASE_AVAILABLE
#endif

#ifdef FIREBASE_AVAILABLE
#include "firebase/analytics.h"
#include "firebase/analytics/event_names.h"
#include "firebase/analytics/parameter_names.h"
#include "firebase/analytics/user_property_names.h"
#include "firebase/app.h"
#include "firebase/log.h"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __ANDROID__
#include <android_native_app_glue.h>
extern struct android_app* __state;
#endif





TrackerService::TrackerService(const ServiceManager * manager)
    : Service(manager)
    , _firebaseApp(NULL)
    , _httpRequestService(NULL)
{
#ifdef FIREBASE_AVAILABLE
#ifdef __ANDROID__
    android_app* app = __state;
    JNIEnv* env = app->activity->env;
    JavaVM* vm = app->activity->vm;
    vm->AttachCurrentThread(&env, NULL);

    _firebaseApp = firebase::App::Create(env, __state->activity->clazz);

    vm->DetachCurrentThread();
#else

    _firebaseApp = firebase::App::Create();
#endif

    //firebase::SetLogLevel(firebase::kLogLevelDebug);

    if (_firebaseApp)
        firebase::analytics::Initialize(*_firebaseApp);
#endif
}

TrackerService::~TrackerService()
{
#ifdef FIREBASE_AVAILABLE
    if (_firebaseApp)
        firebase::analytics::Terminate();
#endif
}

void TrackerService::setupTracking(const char * appId, const char * appInstanceId, const char * apiSecret)
{
    _appId = appId;
    _appInstanceId = appInstanceId;
    _apiSecret = apiSecret;

    // GA require app instance id to be 32 digits hex number
    _appInstanceId.erase(std::remove(_appInstanceId.begin(), _appInstanceId.end(), '-'), _appInstanceId.end());

#ifdef FIREBASE_AVAILABLE
    firebase::analytics::GetAnalyticsInstanceId().OnCompletion([this](const firebase::Future<std::string>& future) {
        if (future.error() == 0)
            _appInstanceId = *future.result();
    });

    _appId = _firebaseApp->options().app_id();
#endif

#ifdef __EMSCRIPTEN__
    EM_ASM({
        Module.faQueue = [];
        Module.faUserId = null;
        Module.faUserProperties = {};
        import("https://www.gstatic.com/firebasejs/10.13.2/firebase-app.js").then((mod)=>{ 
            Module.firebaseApp = mod;
            import("https://www.gstatic.com/firebasejs/10.13/firebase-analytics.js").then((mod)=>{ 
                Module.firebaseAnalytics = mod;

                if (Module.firebaseConfig)
                {
                    const app = Module.firebaseApp.initializeApp(Module.firebaseConfig);
                    const analytics = Module.firebaseAnalytics.getAnalytics(app);

                    Module.fa = analytics;
                    Module.firebaseAnalytics.setUserId(Module.fa, Module.faUserId);
                    Module.firebaseAnalytics.setUserProperties(Module.fa, Module.faUserProperties);

                    Module.faQueue.forEach(function(v) {
                        Module.firebaseAnalytics.logEvent(Module.fa, v[0], v[1]);
                    });
                }
            });
        });
    });
#endif
}

bool TrackerService::onPreInit()
{
    _httpRequestService = _manager->findService<HTTPRequestService>();

    return true;
}

bool TrackerService::onShutdown()
{
    return true;
}

bool TrackerService::onTick()
{
    return true;
}

void TrackerService::sendView(const char * screenName, const char * screenClass, const Parameter * parameters, unsigned parameterCount)
{
    std::unique_ptr<Parameter[]> params(new Parameter[parameterCount + 2]);
    params[0].name = "screen_name";
    params[0].value.set(std::string(screenName));
    params[1].name = "screen_class";
    params[1].value.set(std::string(screenClass));
    memcpy(params.get() + 2, parameters, sizeof(Parameter) * parameterCount);

    sendEvent("screen_view", params.get(), parameterCount + 2);
}

// Function to escape JSON string
std::string escapeJsonString(const std::string& input)
{
    std::ostringstream ss;
    for (char c : input)
    {
        switch (c)
        {
        case '"':  ss << "\\\""; break;
        case '\\': ss << "\\\\"; break;
        case '\b': ss << "\\b";  break;
        case '\f': ss << "\\f";  break;
        case '\n': ss << "\\n";  break;
        case '\r': ss << "\\r";  break;
        case '\t': ss << "\\t";  break;
        default:
            if (static_cast<unsigned char>(c) < 0x20 || c == 0x7F)
            {
                // Escape control characters
                ss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                    << static_cast<int>(static_cast<unsigned char>(c));
            }
            else
            {
                ss << c;
            }
        break;
        }
    }
    return ss.str();
}

std::string formatVariantType(const VariantType& value)
{
    switch (value.getType())
    {
    case VariantType::TYPE_INT8:
    case VariantType::TYPE_INT16:
    case VariantType::TYPE_INT32:
    case VariantType::TYPE_INT64:
    case VariantType::TYPE_UINT8:
    case VariantType::TYPE_UINT16:
    case VariantType::TYPE_UINT32:
    case VariantType::TYPE_UINT64:
        return fmt::format("{}", value.get<int64_t>());

    case VariantType::TYPE_BOOLEAN:
        return value.get<bool>() ? "true" : "false";

    case VariantType::TYPE_FLOAT:
        return fmt::format("{}", value.get<float>());

    case VariantType::TYPE_FLOAT64:
        return fmt::format("{}", value.get<double>());

    case VariantType::TYPE_STRING:
        return std::string("\"") + escapeJsonString(value.get<std::string>()) + "\"";
        
    default:
        GP_ASSERT(!"Unsupported variant type");
    }

    return "";
}

std::string TrackerService::getJsonParamsPayload(const Parameter * parameters, unsigned parameterCount) const
{
    std::string paramsPayload = "{";
    for (unsigned i = 0; i < parameterCount; i++)
    {
        paramsPayload += fmt::format("\"{}\":", escapeJsonString(parameters[i].name));
        paramsPayload += formatVariantType(parameters[i].value);
        paramsPayload += ",";
    }

    if (parameterCount > 0)
        paramsPayload.pop_back();
    paramsPayload += "}";

    return std::move(paramsPayload);
}

void TrackerService::sendEcommerceEvent(const char * eventName, const EcommerceItem * items, unsigned itemCount, const Parameter * parameters, unsigned parameterCount)
{
    std::string paramsPayload = getJsonParamsPayload(parameters, parameterCount);
    paramsPayload.pop_back();
    paramsPayload += ", \"items\":[";

    for (unsigned j = 0; j < itemCount; j++)
    {
        paramsPayload += getJsonParamsPayload(items[j].parameters, items[j].parameterCount);
        paramsPayload += ",";
    }

    GP_ASSERT(itemCount > 0);
    paramsPayload.pop_back();
    paramsPayload += "]}";

    sendGAEvent(eventName, paramsPayload);
}

void TrackerService::sendEvent(const char * eventName, const Parameter * parameters, unsigned parameterCount)
{
#ifdef FIREBASE_AVAILABLE
    if (_firebaseApp)
    {
        std::unique_ptr<firebase::analytics::Parameter[]> params(new firebase::analytics::Parameter[parameterCount]);

        for (unsigned i = 0; i < parameterCount; i++)
        {
            params[i].name = parameters[i].name;

            switch (parameters[i].value.getType())
            {
            case VariantType::TYPE_INT8:
            case VariantType::TYPE_INT16:
            case VariantType::TYPE_INT32:
            case VariantType::TYPE_INT64:
            case VariantType::TYPE_UINT8:
            case VariantType::TYPE_UINT16:
            case VariantType::TYPE_UINT32:
            case VariantType::TYPE_UINT64:
                params[i].value.set_int64_value(parameters[i].value.get<int64_t>());
                break;

            case VariantType::TYPE_BOOLEAN:
                params[i].value.set_bool_value(parameters[i].value.get<bool>());
                break;

            case VariantType::TYPE_FLOAT:
                params[i].value.set_double_value(parameters[i].value.get<float>());
                break;

            case VariantType::TYPE_FLOAT64:
                params[i].value.set_double_value(parameters[i].value.get<double>());
                break;

            case VariantType::TYPE_STRING:
                params[i].value.set_string_value(parameters[i].value.get<std::string>());
                break;

            default:
                GP_ASSERT(!"Unsupported variant type");
            }
        }

        firebase::analytics::LogEvent(eventName, params.get(), parameterCount);
        return;
    }

    // fallback to measurement protocol

#endif

    sendGAEvent(eventName, getJsonParamsPayload(parameters, parameterCount));
}

void TrackerService::sendGAEvent(const char * eventName, const std::string& paramsPayload) const
{
#ifdef __EMSCRIPTEN__

    EM_ASM_({
        if (Module.fa)
            Module.firebaseAnalytics.logEvent(Module.fa, Module.UTF8ToString($0), JSON.parse(Module.UTF8ToString($1)));
        else if (Module.faQueue)
            Module.faQueue.push([Module.UTF8ToString($0), JSON.parse(Module.UTF8ToString($1))]);
    }, eventName, paramsPayload.c_str());

#else

    //GP_ASSERT(_httpRequestService);
    if (!_httpRequestService)
        return;

    static std::string endpoint = fmt::format("https://www.google-analytics.com/mp/collect?api_secret={}&firebase_app_id={}", _apiSecret, _appId);

    std::string userIdPayload;
    if (!_userId.empty())
        userIdPayload = fmt::format(",\"user_id\": \"{}\"", _userId);

    std::string userPropertiesPayload;
    if (!_userProperties.empty())
    {
        userPropertiesPayload = ",\"user_properties\":{";
        for (const auto& prop : _userProperties)
            userPropertiesPayload += fmt::format(R"("{}":{{"value":"{}"}},)", prop.first, prop.second);
        userPropertiesPayload.pop_back();
        userPropertiesPayload += "}";
    }

    const char * clientKey = "app_instance_id";

    std::string payload = fmt::format(R"({{"{}":"{}"{}{},"events":[{{"name":"{}", "params":{}}}]}})", clientKey,
        _appInstanceId, userIdPayload, userPropertiesPayload, eventName, paramsPayload);

    //GP_LOG("making GA request %s %s", endpoint.c_str(), payload.c_str());
    _httpRequestService->makeRequestAsync({ endpoint.c_str(), payload.c_str(),
        { { "Content-Type", "application/json" },
        }
    });

#endif
}

void TrackerService::sendTiming(const char * category, const char * variable, const int& timeMs, const Parameter * parameters, unsigned parameterCount)
{
    std::unique_ptr<Parameter[]> params(new Parameter[parameterCount + 3]);
    params[0].name = "event_category";
    params[0].value.set(std::string(category));
    params[1].name = "name";
    params[1].value.set(std::string(variable));
    params[2].name = "value";
    params[2].value.set(timeMs);
    memcpy(params.get() + 3, parameters, sizeof(Parameter) * parameterCount);

    sendEvent("timing_complete", params.get(), parameterCount + 3);
}

void TrackerService::sendException(const char * type, bool isFatal, const Parameter * parameters, unsigned parameterCount)
{
    std::unique_ptr<Parameter[]> params(new Parameter[parameterCount + 2]);
    params[0].name = "description";
    params[0].value.set(std::string(type));
    params[1].name = "fatal";
    params[1].value.set(isFatal);
    memcpy(params.get() + 2, parameters, sizeof(Parameter) * parameterCount);

    sendEvent("exception", params.get(), parameterCount + 2);
}

void TrackerService::setUserId(const char * userId)
{
    _userId = userId ? userId : "";

#ifdef FIREBASE_AVAILABLE
    if (_firebaseApp)
        firebase::analytics::SetUserId(userId);
#endif

#ifdef __EMSCRIPTEN__
    EM_ASM_({
        if (Module.fa)
            Module.firebaseAnalytics.setUserId(Module.fa, ($0) ? Module.UTF8ToString($0) : null);
        else
            Module.faUserId = ($0) ? Module.UTF8ToString($0) : null;
    }, userId);
#endif
}

void TrackerService::setUserProperty(const char * name, const char * value)
{
    if (value)
        _userProperties[name] = value;
    else
        _userProperties.erase(name);

#ifdef FIREBASE_AVAILABLE
    if (_firebaseApp)
        firebase::analytics::SetUserProperty(name, value);
#endif

#ifdef __EMSCRIPTEN__
    std::string userPropertiesPayload = "{";
    for (const auto& prop : _userProperties)
        userPropertiesPayload += fmt::format("\"{}\":\"{}\",", prop.first.c_str(), prop.second.c_str());
    userPropertiesPayload.pop_back();
    userPropertiesPayload += "}";

    EM_ASM_({
        if (Module.fa)
            Module.firebaseAnalytics.setUserProperties(Module.fa, JSON.parse(Module.UTF8ToString($0)));
        else
            Module.faUserProperties = JSON.parse(Module.UTF8ToString($0));
    }, userPropertiesPayload.c_str());
#endif
}

void TrackerService::setTrackerEnabled(bool enabled)
{
#ifdef FIREBASE_AVAILABLE
    if (_firebaseApp)
        firebase::analytics::SetAnalyticsCollectionEnabled(enabled);
#endif
}
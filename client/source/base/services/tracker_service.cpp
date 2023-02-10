#include "pch.h"
#include "tracker_service.h"
#include "service_manager.h"
#include "httprequest_service.h"
#include "main.h"

#include "firebase/analytics.h"
#include "firebase/analytics/event_names.h"
#include "firebase/analytics/parameter_names.h"
#include "firebase/analytics/user_property_names.h"
#include "firebase/app.h"

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
#ifdef __ANDROID__
    _firebaseApp = firebase::App::Create(__state->activity->env, __state->activity->clazz);
#else
    _firebaseApp = firebase::App::Create();
#endif

    if (_firebaseApp)
        firebase::analytics::Initialize(*_firebaseApp);
}

TrackerService::~TrackerService()
{
    if (_firebaseApp)
        firebase::analytics::Terminate();
}

void TrackerService::setupTracking(const char * apiSecret)
{
    _apiSecret = apiSecret;
}

bool TrackerService::onInit()
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
    params[0].name = firebase::analytics::kParameterScreenName;
    params[0].value.set(std::string(screenName));
    params[1].name = firebase::analytics::kParameterScreenClass;
    params[1].value.set(std::string(screenClass));
    memcpy(params.get() + 2, parameters, sizeof(Parameter) * parameterCount);

    sendEvent(firebase::analytics::kEventScreenView, params.get(), parameterCount + 2);
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
        return Utils::format("lld", value.get<int64_t>());

    case VariantType::TYPE_BOOLEAN:
        return value.get<bool>() ? "true" : "false";

    case VariantType::TYPE_FLOAT:
    case VariantType::TYPE_FLOAT64:
        return Utils::format("%f", value.get<double>());

    case VariantType::TYPE_STRING:
        return value.get<std::string>();
        
    default:
        GP_ASSERT(!"Unsupported variant type");
    }

    return "";
}

void TrackerService::sendEcommerceEvent(const char * eventName, const EcommerceItem * items, unsigned itemCount, const Parameter * parameters, unsigned parameterCount)
{
    GP_ASSERT(_httpRequestService && _firebaseApp);
    if (!_httpRequestService || !_firebaseApp)
        return;

    std::string paramsPayload = "{";
    for (unsigned i = 0; i < parameterCount; i++)
    {
        paramsPayload += Utils::format("\"%s\":", parameters[i].name);
        paramsPayload += formatVariantType(parameters[i].value);
        paramsPayload += ",";
    }

    paramsPayload += Utils::format("\"items\":[");

    for (unsigned j = 0; j < itemCount; j++)
    {
        paramsPayload += "{";
        for (unsigned i = 0; i < items[j].parameterCount; i++)
        {
            paramsPayload += Utils::format("\"%s\":", items[j].parameters[i].name);
            paramsPayload += formatVariantType(items[j].parameters[i].value);
            paramsPayload += ",";
        }

        paramsPayload.pop_back();
        paramsPayload += "},";
    }

    GP_ASSERT(itemCount > 0);
    paramsPayload.pop_back();
    paramsPayload += "]}";
    
    firebase::analytics::GetAnalyticsInstanceId().OnCompletion([this, eventName, paramsPayload](const firebase::Future<std::string>& completedFuture) {

        std::string endpoint = Utils::format("https://www.google-analytics.com/mp/collect?api_secret=%s&firebase_app_id=%s", _apiSecret.c_str(), _firebaseApp->options().app_id());

        std::string userIdPayload;
        if (!_userId.empty())
            userIdPayload = Utils::format(",\"user_id\": \"%s\"", _userId.c_str());

        std::string userPropertiesPayload;
        if (!_userProperties.empty())
        {
            userPropertiesPayload = ",\"user_properties\":{";
            for (const auto& prop : _userProperties)
                userPropertiesPayload += Utils::format("\"%s\":\"%s\",", prop.first.c_str(), prop.second.c_str());
            userPropertiesPayload.pop_back();
            userPropertiesPayload += "}";
        }

        std::string payload = Utils::format("{\"app_instance_id\":\"%s\"%s%s,\"events\":[{\"name\":\"%s\", \"params\":%s}]}",
            completedFuture.result()->c_str(), userIdPayload.c_str(), userPropertiesPayload.c_str(), eventName, paramsPayload.c_str());

        _httpRequestService->makeRequestAsync({ endpoint.c_str(), payload.c_str(),
            { { "Content-Type", "application/json" },
            }
        });
    });
}

void TrackerService::sendEvent(const char * eventName, const Parameter * parameters, unsigned parameterCount)
{
    if (!_firebaseApp)
        return;
        
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
    if (!_firebaseApp)
        return;

    _userId = userId;
    firebase::analytics::SetUserId(userId);
}

void TrackerService::setUserProperty(const char * name, const char * value)
{
    if (!_firebaseApp)
        return;

    if (value)
        _userProperties[name] = value;
    else
        _userProperties.erase(name);

    firebase::analytics::SetUserProperty(name, value);
}

void TrackerService::setTrackerEnabled(bool enabled)
{
    if (!_firebaseApp)
        return;

    firebase::analytics::SetAnalyticsCollectionEnabled(enabled);
}
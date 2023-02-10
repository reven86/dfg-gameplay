#pragma once

#include "service.h"


namespace firebase { class App; };

/**
 * Tracks user interactions, game events and sends data to Google Analytics.
 * A wrapper around Firebase Analytics.
 */

class TrackerService : public Service
{
    friend class ServiceManager;

public:
    struct Parameter
    {
        const char * name;
        VariantType value;
    };

    struct EcommerceItem
    {
        const Parameter * parameters;
        unsigned parameterCount;
    };

    /**
     * Setup tracking for using Measurement Protocol. The Measurement Protocol needs
     * firebase_app_id and api_secret to make sure request reach the analytics server.
     * The app id can be obtained using the firebase::AppOptions, while api_secret needs
     * to be generated in the Analytics Settings Console.
     * 
     * @param[in] apiSecret API secret.
     */
    void setupTracking(const char * apiSecret);

    void setTrackerEnabled(bool enabled);

    static const char * getTypeName() { return "TrackerService"; }
    
    /**
     * General method to send events to GA.
     *
     * @param[in] eventName Name of the event.
     * @param[in] parameters Array of parameters passed in.
     * @param[in] parameterCount Number of parameters.
     */
    void sendEvent(const char * eventName, const Parameter * parameters = NULL, unsigned parameterCount = 0);

    /**
     * Send ecommerce event that requires to put 'items' parameter as an array.
     *
     * This method is used as workaround to send ecommerce-related events using
     * Measurement Protocol, because currently Firebase C++ SDK doesn't allow to 
     * send array parameters.
     * 
     * Uses HttpRequestService.
     * 
     * @param[in] eventName Event name (begin_checout, purchase, etc)
     * @param[in] items Item array.
     * @param[in] itemCount Number of items.
     * @param[in] parameters Additional event parameters.
     * @param[in] parameterCount Number of event parameters.
     */
    void sendEcommerceEvent(const char * eventName, const EcommerceItem * items, unsigned itemCount, const Parameter * parameters = NULL, unsigned parameterCount = 0);

    // helper methods
    void sendView(const char * screenName, const char * screenClass, const Parameter * parameters = NULL, unsigned parameterCount = 0);
    void sendTiming(const char * category, const char * variable, const int& timeMs, const Parameter * parameters = NULL, unsigned parameterCount = 0);
    void sendException(const char * type, bool isFatal, const Parameter * parameters = NULL, unsigned parameterCount = 0);

    /** 
     * Set UserID property.
     *
     * UserID is added to every request to GA and helps track user's behavior
     * accross different devices.
     *
     * @param[in] userId UserID (or null to remove UserID tracking).
     */
    void setUserId(const char * userId);

    /**
     * Set user property.
     *
     * @param[in] name Property name.
     * @param[in] value Property value.
     */
    void setUserProperty(const char * name, const char * value);

protected:
    /**
    * Constructs tracker service.
    *
    * @param[in]  manager     ServiceManager.
    */
    TrackerService(const ServiceManager * manager);
    virtual ~TrackerService();

    virtual bool onInit();
    virtual bool onShutdown();
    virtual bool onTick();

private:
    firebase::App * _firebaseApp;

    class HTTPRequestService * _httpRequestService;

    // the following variables are needed to workaround missed methods of Firebase C++ SDK
    // by using Measurement Protocol
    std::string _apiSecret;
    std::string _userId;
    std::map<std::string, std::string> _userProperties;
};

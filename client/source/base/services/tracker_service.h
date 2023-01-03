#ifndef __DFG_TRACKERSERVICE__
#define __DFG_TRACKERSERVICE__

#include "service.h"




/**
 * Tracks user interactions, game events and sends data to Google Analytics.
 * If internet connection is lost, analytics data is stored offline, until
 * connection is restored.
 */

class TrackerService : public Service
{
    friend class ServiceManager;

public:
    /*! Setup app tracking
     *
     *  \param[in]  gaAppId     Google Analytics Tracking ID (UA-123456-1).
     *  \param[in]  clientId    Client UUID (http://www.ietf.org/rfc/rfc4122.txt).
     *  \param[in]  startScreen Initial screen name.
     *  \param[in]  installerId Name of installer package (source of installation).
     */
    void setupTracking(const char * gaAppId, const char * clientId, const char * startScreen, const char * installerId = nullptr);

    void setTrackerEnabled(bool enabled) { _allowDispatch = enabled; };
    const bool& getTrackerEnabled() const { return _allowDispatch; };

    static const char * getTypeName() { return "TrackerService"; }

    const char * getView() const { return _currentView.c_str(); };

    void sendView(const char * screenName);
    void sendEvent(const char * category, const char * action, const char * label, const int& value = 0, bool interactive = true);
    void sendSocialEvent(const char * network, const char * action, const char * target);
    void sendTiming(const char * category, const int& timeMs, const char * name = NULL, const char * label = NULL);
    void sendException(const char * type, bool isFatal);
    void sendTransaction(const char * transactionID, const char * store, float totalRevenue, float shippingCost, float tax, const char * currencyCode);
    void sendItem(const char * transactionID, const char * name, float price, int quantity, const char * sku, const char * category, const char * currencyCode);

    void setCustomDimension(const int& id, const char * dimension);
    void setCustomMetric(const int& id, const int& metric);

    /** 
     * Set UserID property.
     *
     * UserID is added to every request to GA and helps track user's behavior
     * accross different devices.
     *
     * @param[in] userId UserID (or null to remove UserID tracking).
     */
    void setUserId(const char * userId);
    const char * getUserId() const { return _userId.c_str(); };

    /**
     * Get Client ID used in all tracker events.
     */
    const char * getClientId() const { return _clientId.c_str(); };

    /** 
     * Immediately send all payloads to analytics server
     */
    void forceDispatch();

    /**
     * End session. Sends new view and attaches session end marker to it.
     *
     * @param viewName Session end view name. Default is 'SessionEnd'.
     */
    void endSession(const char * viewName = "SessionEnd");

    /**
     * Flush all payloads immediately to disk.
     */
    void flushPayloads();

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
    struct PayloadInfo
    {
        std::string params;
        float createTime;       // since programm start in ms
    };

    void sendData(const char * format, ...);
    bool dispatch(const PayloadInfo& payload);

    static void dispatchThreadProc(void * arg);


    std::string _trackingId;
    std::string _clientId;
    int _protocolVersion;

    enum{ MAX_CUSTOM_DIMENSIONS = 20 };
    enum{ MAX_CUSTOM_METRICS = 20 };
    std::string _customDimensions[MAX_CUSTOM_DIMENSIONS];
    int _customMetrics[MAX_CUSTOM_METRICS];

    std::deque< PayloadInfo > _payloadsQueue;

    void * _curl;

    std::string _appName;           // cached app name
    std::string _appVersion;        // cached app version
    std::string _appInstallerId;    // name of the installer package (Android)
    std::string _currentView;
    std::string _userId;           // UserID

    std::unique_ptr< std::thread > _dispatchThread;
    std::mutex _payloadQueueMutex;
    std::mutex _dispatchMutex;
    static bool _threadForceQuit;
    static int _dispatchPeriod;  // in ms

    bool _allowDispatch;
    time_t _trackerStartTime;
    float _sessionStartTime;
    float _lastPayloadSentTime;     // since programm start in ms
};




#endif // __DFG_TRACKERSERVICE__
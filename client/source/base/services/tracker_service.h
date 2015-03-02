#ifndef __DFG_TRACKERSERVICE__
#define __DFG_TRACKERSERVICE__

#include "service.h"




/*! Tracks user interactions, game events and sends data to Google Analytics.
 *  If internet connection is lost, analytics data is stored offline, until
 *  connection is restored.
 */

class TrackerService : public Service
{
    std::string _trackingId;
    std::string _clientId;
    int _protocolVersion;

    enum{ MAX_CUSTOM_DIMENSIONS = 20 };
    enum{ MAX_CUSTOM_METRICS = 20 };
    std::string _customDimensions[ MAX_CUSTOM_DIMENSIONS ];
    int _customMetrics[ MAX_CUSTOM_METRICS ];

    struct PayloadInfo
    {
        std::string params;
        float createTime;       // since programm start in ms
    };

    std::deque< PayloadInfo > _payloadsQueue;

    void * _curl;
    
    std::string _appName;       // cached app name
    std::string _appVersion;    // cached app version
    std::string _currentView;

    static std::auto_ptr< std::thread > _dispatchThread;
	std::mutex _payloadQueueMutex;
	std::mutex _dispatchMutex;
    static bool _threadForceQuit;
    static int _dispatchPeriod;  // in ms
    static int _dispatchRate;  // in ms
    
    bool _allowDispatch;
    time_t _trackerStartTime;
    float _sessionStartTime;
    float _lastPayloadSentTime;     // since programm start in ms

public:
    /*! Constructs tracker service.
     *
     *  \param[in]  manager     ServiceManager.
     */
	TrackerService( const ServiceManager * manager );
	virtual ~TrackerService( );

    /*! Setup app tracking
     *
     *  \param[in]  gaAppId     Google Analytics Tracking ID (UA-123456-1).
     *  \param[in]  clientId    Client UUID (http://www.ietf.org/rfc/rfc4122.txt). 
     *  \param[in]  startScreen Initial screen name.
     */
    void SetupTracking( const char * gaAppId, const char * clientId, const char * startScreen );

    void SetTrackerEnabled( bool enabled ) { _allowDispatch = enabled; };
    const bool& GetTrackerEnabled( ) const { return _allowDispatch; };

	static const char * GetName( ) { return "TrackerService"; }

	virtual bool OnInit( );
	virtual bool OnShutdown( );
    virtual bool OnTick( );

    const char * GetView() const { return _currentView.c_str(); };

    void SendView( const char * screenName );
    void SendEvent( const char * category, const char * action, const char * label, const int& value = 0, bool interactive = true );
    void SendSocialEvent( const char * network, const char * action, const char * target );
    void SendTiming( const char * category, const int& timeMs, const char * name = NULL, const char * label = NULL );
    void SendException( const char * type, bool isFatal );
    void SendTransaction( const char * transactionID, const char * store, float totalRevenue, float shippingCost, float tax, const char * currencyCode );
    void SendItem( const char * transactionID, const char * name, float price, int quantity, const char * sku, const char * category, const char * currencyCode );
    
    void SetCustomDimension( const int& id, const char * dimension );
    void SetCustomMetric( const int& id, const int& metric );

    /*! Immediately send all payloads to analytics server
     *
     *  Normally all payloads except the last are dispatched. This is needed to properly send 'session end' signal.
     *  Session end signal is always attached to the last payload, which should be available.
     *
     *  \param[in]  dispatchCount   Amount of payloads to be sent. 0 to send all.
     */
    void ForceDispatch( int dispatchCount );
    void EndSession( );
    
    //! Flush all payloads immediately to disk.
    void FlushPayloads( );

private:
    void SendData( const char * format, ... );
    bool Dispatch( const PayloadInfo& payload );

    const char * UrlEncode( const char * str );

    static void DispatchThreadProc( void * arg );
};




#endif // __DFG_TRACKERSERVICE__
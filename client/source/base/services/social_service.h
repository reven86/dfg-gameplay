#ifndef __DFG_SOCIALSERVICE__
#define __DFG_SOCIALSERVICE__

#include "service.h"




/** @brief Social service.
 */

class SocialService : public Service, public gameplay::SocialSessionListener
{
    friend class ServiceManager;

public:
    static const char * getTypeName() { return "SocialService"; }

    gameplay::SocialSession * getSession() { return _session; };

protected:
    SocialService(const ServiceManager * manager);
    virtual ~SocialService();

    virtual bool onPreInit();
    virtual bool onInit();
    virtual bool onTick();
    virtual bool onShutdown();

    /**
     * @see gameplay::SocialSessionListener::authenticateEvent
     */
    virtual void authenticateEvent(ResponseCode code, gameplay::SocialSession* session);

private:
    gameplay::SocialSession * _session;
};



#endif //__DFG_SOCIALSERVICE__
#ifndef __DFG_SOCIALSERVICE__
#define __DFG_SOCIALSERVICE__

#include "service.h"




/*! \brief Social service.
 */

class SocialService : public Service, public gameplay::SocialSessionListener
{
    gameplay::SocialSession * _session;

public:
    SocialService(const ServiceManager * manager);
    virtual ~SocialService();

    static const char * getTypeName() { return "SocialService"; }

    virtual bool onPreInit();
    virtual bool onInit();
    virtual bool onTick();
    virtual bool onShutdown();

    gameplay::SocialSession * getSession() { return _session; };

protected:
    /**
     * @see gameplay::SocialSessionListener::authenticateEvent
     */
    virtual void authenticateEvent(ResponseCode code, gameplay::SocialSession* session);
};



#endif //__DFG_SOCIALSERVICE__
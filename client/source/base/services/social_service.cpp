#include "pch.h"
#include "social_service.h"
#include "services/service_manager.h"




SocialService::SocialService( const ServiceManager * manager )
    : Service( manager )
    , _session( NULL )
{
}

SocialService::~SocialService( )
{
}

bool SocialService::OnPreInit( )
{
    return true;
}

bool SocialService::OnTick( )
{
    return false;
}

bool SocialService::OnInit( )
{
    if( gameplay::Game::getInstance()->getSocialController() )
        gameplay::Game::getInstance()->getSocialController()->authenticate(this);
    
    return true;
}

bool SocialService::OnShutdown( )
{
    _session = NULL;
    return true;
}

void SocialService::authenticateEvent(ResponseCode code, gameplay::SocialSession *session)
{
    _session = session;

	if (code == SUCCESS)
	{
	}
	else
    {
        GP_WARN( "Error authenticating the social session %d", code );
    }
    
    _manager->signals.socialAuthenticatedEvent( code );
}
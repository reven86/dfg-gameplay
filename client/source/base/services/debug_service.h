#ifndef __DFG_DEBUGSERVICE__
#define __DFG_DEBUGSERVICE__

#include "service.h"




class RenderStep;
class RenderClick;


/*! \brief Debug tools.
 */

class DebugService : public Service
{
	RefPtr< gameplay::Font > _systemFont;
    gameplay::SpriteBatch * _whiteSprite;
    float _fontSize;

	RefPtr< RenderStep > _debugRenderStep;
	RefPtr< RenderClick > _fpsRenderClick;

    bool _showProfiler;

public:
	DebugService ( const ServiceManager * manager );
	virtual ~DebugService ( );

	static const char * GetName( ) { return "DebugService"; }

    void Setup( gameplay::SpriteBatch * whiteSprite, gameplay::Font * font, float scale = 1.0f );

    void ShowProfiler( bool show ) { _showProfiler = show; };
    bool IsProfilerShown( ) const { return _showProfiler; };

    virtual bool OnPreInit( );
	virtual bool OnInit( );
    virtual bool OnTick( );
	virtual bool OnShutdown( );

private:
    void DrawTextOutline ( float x, float y, const gameplay::Vector4& color, const wchar_t * text ) const;

	void DrawFPS ( ) const;
};



#endif //__DFG_DEBUGSERVICE__
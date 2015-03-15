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

    std::unique_ptr< RenderStep > _debugRenderStep;
    std::unique_ptr< RenderClick > _fpsRenderClick;

    bool _showProfiler;

public:
    DebugService(const ServiceManager * manager);
    virtual ~DebugService();

    static const char * getTypeName() { return "DebugService"; }

    void setup(gameplay::SpriteBatch * whiteSprite, gameplay::Font * font, float fontSize);

    void showProfiler(bool show) { _showProfiler = show; };
    bool isProfilerVisible() const { return _showProfiler; };

    virtual bool onPreInit();
    virtual bool onInit();
    virtual bool onTick();
    virtual bool onShutdown();

private:
    void drawTextOutline(float x, float y, const gameplay::Vector4& color, const wchar_t * text) const;
    void drawFPS() const;
};



#endif //__DFG_DEBUGSERVICE__
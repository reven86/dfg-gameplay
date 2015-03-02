#ifndef __DFG_INPUTSERVICE__
#define __DFG_INPUTSERVICE__

#include "service.h"




/*! \brief Processes input from all input devices.
 *
 */

class InputService : public Service
{
public:
    InputService(const ServiceManager * manager);
    virtual ~InputService();

    static const char * getTypeName() { return "InputService"; }

    virtual bool onInit();
    virtual bool onShutdown();
    virtual bool onTick() { return true; };

    void injectKeyEvent(const gameplay::Keyboard::KeyEvent& ev, int key);
    void injectTouchEvent(const gameplay::Touch::TouchEvent& ev, int x, int y, unsigned int contactIndex);
    bool injectMouseEvent(const gameplay::Mouse::MouseEvent& ev, int x, int y, int wheelDelta);
    void injectGesturePinchEvent(int x, int y, float scale, int numberOfTouches);
};




#endif // __DFG_INPUTSERVICE__
#ifndef __DFG_INPUTSERVICE__
#define __DFG_INPUTSERVICE__

#include "service.h"




/** @brief Processes input from all input devices.
 *
 */

class InputService : public Service
{
    friend class ServiceManager;

public:
    static const char * getTypeName() { return "InputService"; }

    void injectKeyEvent(const gameplay::Keyboard::KeyEvent& ev, int key);
    void injectTouchEvent(const gameplay::Touch::TouchEvent& ev, int x, int y, unsigned int contactIndex);
    bool injectMouseEvent(const gameplay::Mouse::MouseEvent& ev, int x, int y, float wheelDelta);
    void injectGesturePinchEvent(int x, int y, float scale, int numberOfTouches);
    void injectGestureRotationEvent(int x, int y, float rotation, int numberOfTouches);
    void injectGesturePanEvent(int x, int y, int numberOfTouches);

    void injectKeyGlobalEvent(const gameplay::Keyboard::KeyEvent& ev, int key);
    void injectTouchGlobalEvent(const gameplay::Touch::TouchEvent& ev, int x, int y, unsigned int contactIndex);
    bool injectMouseGlobalEvent(const gameplay::Mouse::MouseEvent& ev, int x, int y, float wheelDelta);

    /**
     * Inject Gesture::SWIPE event to be signals system.
     *
     * @param x The x-coordinate of the start of the swipe.
     * @param y The y-coordinate of the start of the swipe.
     * @param direction The direction of the swipe
     *
     * @see Gesture::SWIPE_DIRECTION_UP
     * @see Gesture::SWIPE_DIRECTION_DOWN
     * @see Gesture::SWIPE_DIRECTION_LEFT
     * @see Gesture::SWIPE_DIRECTION_RIGHT
     */
    void injectGestureSwipeEvent(int x, int y, int direction);

protected:
    InputService(const ServiceManager * manager);
    virtual ~InputService();

    virtual bool onInit();
    virtual bool onShutdown();
    virtual bool onTick() { return true; };
};




#endif // __DFG_INPUTSERVICE__
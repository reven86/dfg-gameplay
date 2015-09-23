#include "pch.h"
#include "input_service.h"
#include "service_manager.h"






InputService::InputService(const ServiceManager * manager)
    : Service(manager)
{
}

InputService::~InputService()
{
}

bool InputService::onInit()
{
    return true;
}

bool InputService::onShutdown()
{
    return true;
}

void InputService::injectKeyEvent(const gameplay::Keyboard::KeyEvent& ev, int key)
{
    for (auto it = _manager->signals.inputKeyEvent.signal.slots().begin(), end_it = _manager->signals.inputKeyEvent.signal.slots().end(); it != end_it; it++)
        if ((*it)(ev, key))
            break;
}

void InputService::injectTouchEvent(const gameplay::Touch::TouchEvent& ev, int x, int y, unsigned int contactIndex)
{
    for (auto it = _manager->signals.inputTouchEvent.signal.slots().begin(), end_it = _manager->signals.inputTouchEvent.signal.slots().end(); it != end_it; it++)
        if ((*it)(ev, x, y, contactIndex))
            break;
}

bool InputService::injectMouseEvent(const gameplay::Mouse::MouseEvent& ev, int x, int y, int wheelDelta)
{
    for (auto it = _manager->signals.inputMouseEvent.signal.slots().begin(), end_it = _manager->signals.inputMouseEvent.signal.slots().end(); it != end_it; it++)
        if ((*it)(ev, x, y, wheelDelta))
            return true;

    return false;
}

void InputService::injectGesturePinchEvent(int x, int y, float scale, int numberOfTouches)
{
    for (auto it = _manager->signals.inputGesturePinchEvent.signal.slots().begin(), end_it = _manager->signals.inputGesturePinchEvent.signal.slots().end(); it != end_it; it++)
        if ((*it)(x, y, scale, numberOfTouches))
            break;
}

void InputService::injectGestureSwipeEvent(int x, int y, int direction)
{
    for (auto& slot : _manager->signals.inputGestureSwipeEvent.signal.slots())
    {
        if (slot(x, y, direction))
            break;
    }
}
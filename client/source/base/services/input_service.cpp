#include "pch.h"
#include "input_service.h"
#include "service_manager.h"






InputService::InputService( const ServiceManager * manager )
    : Service( manager )
{
}

InputService::~InputService( )
{
}

bool InputService::OnInit( )
{
    return true;
}

bool InputService::OnShutdown( )
{
    return true;
}

void InputService::InjectKeyEvent( const gameplay::Keyboard::KeyEvent& ev, int key )
{
    for( auto it = _manager->signals.inputKeyEvent.signal.slots( ).begin( ), end_it = _manager->signals.inputKeyEvent.signal.slots( ).end( ); it != end_it; it++ )
        if( ( *it )( ev, key ) )
            break;
}

void InputService::InjectTouchEvent( const gameplay::Touch::TouchEvent& ev, int x, int y, unsigned int contactIndex )
{
    for( auto it = _manager->signals.inputTouchEvent.signal.slots( ).begin( ), end_it = _manager->signals.inputTouchEvent.signal.slots( ).end( ); it != end_it; it++ )
        if( ( *it )( ev, x, y, contactIndex ) )
            break;
}

bool InputService::InjectMouseEvent( const gameplay::Mouse::MouseEvent& ev, int x, int y, int wheelDelta )
{
    for( auto it = _manager->signals.inputMouseEvent.signal.slots( ).begin( ), end_it = _manager->signals.inputMouseEvent.signal.slots( ).end( ); it != end_it; it++ )
        if( ( *it )( ev, x, y, wheelDelta ) )
            return true;

    return false;
}

void InputService::InjectGesturePinchEvent(int x, int y, float scale, int numberOfTouches)
{
    for (auto it = _manager->signals.inputGesturePinchEvent.signal.slots().begin(), end_it = _manager->signals.inputGesturePinchEvent.signal.slots().end(); it != end_it; it++)
        if ((*it)(x, y, scale, numberOfTouches))
            break;
}

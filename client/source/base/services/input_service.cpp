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
    for( sigc::signal< bool, gameplay::Keyboard::KeyEvent, int >::slot_list_type::iterator it = _manager->signals.inputKeyEvent.signal.slots( ).begin( ), 
        end_it = _manager->signals.inputKeyEvent.signal.slots( ).end( ); it != end_it; it++ )
    {
        if( ( *it )( ev, key ) )
            break;
    }
}

void InputService::InjectTouchEvent( const gameplay::Touch::TouchEvent& ev, int x, int y, unsigned int contactIndex )
{
    for( sigc::signal< bool, gameplay::Touch::TouchEvent, int, int, unsigned >::slot_list_type::iterator it = _manager->signals.inputTouchEvent.signal.slots( ).begin( ), 
        end_it = _manager->signals.inputTouchEvent.signal.slots( ).end( ); it != end_it; it++ )
    {
        if( ( *it )( ev, x, y, contactIndex ) )
            break;
    }
}

bool InputService::InjectMouseEvent( const gameplay::Mouse::MouseEvent& ev, int x, int y, int wheelDelta )
{
    for( sigc::signal< bool, gameplay::Mouse::MouseEvent, int, int, int >::slot_list_type::iterator it = _manager->signals.inputMouseEvent.signal.slots( ).begin( ), 
        end_it = _manager->signals.inputMouseEvent.signal.slots( ).end( ); it != end_it; it++ )
    {
        if( ( *it )( ev, x, y, wheelDelta ) )
            return true;
    }

    return false;
}
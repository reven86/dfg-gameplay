#ifndef __DFG_INPUTSERVICE__
#define __DFG_INPUTSERVICE__

#include "service.h"




/*! \brief Processes input from all input devices.
 *
 */

class InputService : public Service
{
public:
	InputService( const ServiceManager * manager );
	virtual ~InputService( );

	static const char * GetName( ) { return "InputService"; }

	virtual bool OnInit( );
	virtual bool OnShutdown( );
    virtual bool OnTick( ) { return true; };

    void InjectKeyEvent( const gameplay::Keyboard::KeyEvent& ev, int key );
    void InjectTouchEvent( const gameplay::Touch::TouchEvent& ev, int x, int y, unsigned int contactIndex );
    bool InjectMouseEvent( const gameplay::Mouse::MouseEvent& ev, int x, int y, int wheelDelta );
    void InjectGesturePinchEvent(int x, int y, float scale, int numberOfTouches);
};




#endif // __DFG_INPUTSERVICE__
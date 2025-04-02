#ifndef __DFG_SIGNALS__
#define __DFG_SIGNALS__

#include "utils/priority_signal.h"





/** @brief A common registry of all signals. Available through service manager.
 */
struct Signals : Noncopyable
{
    //
    // System-wide signals
    //

    sigc::signal<void> pauseEvent;
    sigc::signal<void> resumeEvent;
    sigc::signal<void, unsigned, unsigned> resizeEvent;
    sigc::signal<void, float, float> virtualKeyboardSizeChanged;    // passing (0,0) when keyboard is hidden
    sigc::signal<void, float, float, float, float> safeAreaChangedEvent;

    //
    // ServiceManager
    //

    sigc::signal< void, const Service::State& > serviceManagerStateChangedEvent;

    //
    // SocialService
    //

    sigc::signal< void, gameplay::SocialSessionListener::ResponseCode > socialAuthenticatedEvent;

    //
    // InputService
    //

    priority_signal< sigc::signal< bool, gameplay::Keyboard::KeyEvent, int > > inputKeyEvent;
    priority_signal< sigc::signal< bool, gameplay::Touch::TouchEvent, int, int, unsigned > > inputTouchEvent;
    priority_signal< sigc::signal< bool, gameplay::Mouse::MouseEvent, int, int, float > > inputMouseEvent;
    priority_signal< sigc::signal< bool, int, int, float, int > > inputGesturePinchEvent;
    priority_signal< sigc::signal< bool, int, int, float, int > > inputGestureRotationEvent;
    priority_signal< sigc::signal< bool, int, int, int > > inputGesturePanEvent;
    priority_signal< sigc::signal< bool, int, int, int > > inputGestureSwipeEvent;

    // global events are fired even after UI has consumed them
    priority_signal< sigc::signal< bool, gameplay::Keyboard::KeyEvent, int > > inputKeyGlobalEvent;
    priority_signal< sigc::signal< bool, gameplay::Touch::TouchEvent, int, int, unsigned > > inputTouchGlobalEvent;
    priority_signal< sigc::signal< bool, gameplay::Mouse::MouseEvent, int, int, float > > inputMouseGlobalEvent;

    //
    // StorefrontService
    //

    sigc::signal< void > storefrontGetProductsSucceededEvent;
    sigc::signal< void, int, const char * > storefrontGetProductsFailedEvent;
    sigc::signal< void, const char *, int > storefrontTransactionInProcessEvent;
    sigc::signal< void, const char *, int, int, const char * > storefrontTransactionFailedEvent;
    sigc::signal< void, const char *, int, double, const char *, void * > storefrontTransactionSucceededEvent;
    sigc::signal< void, const char *, int, double, const char *, void * > storefrontTransactionRestoredEvent;
    sigc::signal< void, void *, int, const char * > storefrontReceiptRequestedEvent;

    //
    // TaskQueueService
    //
    sigc::signal< void, const char *, int > taskQueueWorkItemLoadedEvent;
    sigc::signal< void, const char *, int > taskQueueWorkItemProcessedEvent;
    sigc::signal< void, const char * > taskQueueStartedEvent;
    sigc::signal< void, const char * > taskQueueStoppedEvent;

    //
    // RenderService
    //
    sigc::signal< void > framePreRender;
    sigc::signal< void > framePostRender;
};



#endif
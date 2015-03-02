#ifndef __DFG_SIGNALS__
#define __DFG_SIGNALS__

#include "utils/priority_signal.h"



struct interruptable_accumulator
{
    typedef bool result_type;
    template<typename T_iterator>
    result_type operator()(T_iterator first, T_iterator last) const
    {
        for (; first != last; ++first)
            if (!*first) return false;
        return true;
    }
};


/*! \brief A common registry of all signals. Available through service manager.
 */
struct Signals : Noncopyable
{
    //
    // System-wide signals
    //

    sigc::signal< void > pauseEvent;
    sigc::signal< void > resumeEvent;

    //
    // SocialService
    //
    
    sigc::signal< void, gameplay::SocialSessionListener::ResponseCode > socialAuthenticatedEvent;

    //
    // InputService
    //
    
    priority_signal< sigc::signal< bool, gameplay::Keyboard::KeyEvent, int > > inputKeyEvent;
    priority_signal< sigc::signal< bool, gameplay::Touch::TouchEvent, int, int, unsigned > > inputTouchEvent;
    priority_signal< sigc::signal< bool, gameplay::Mouse::MouseEvent, int, int, int > > inputMouseEvent;

    //
    // StorefrontService
    //

    sigc::signal< void > storefrontGetProductsSucceededEvent;
    sigc::signal< void, int, const char * > storefrontGetProductsFailedEvent;
    sigc::signal< void, const char *, int > storefrontTransactionInProcessEvent;
    sigc::signal< void, const char *, int, int, const char * > storefrontTransactionFailedEvent;
    sigc::signal< bool, const char *, int, double, const char * >::accumulated< interruptable_accumulator > storefrontTransactionSucceededEvent;
    sigc::signal< bool, const char *, int, double, const char * >::accumulated< interruptable_accumulator > storefrontTransactionRestoredEvent;
    sigc::signal< bool, const char * >::accumulated< interruptable_accumulator > storefrontIsProductConsumable;
};



#endif
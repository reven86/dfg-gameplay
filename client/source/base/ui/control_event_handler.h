#pragma once

#ifndef __DFG_CONTROL_EVENT_HANDLER_H__
#define __DFG_CONTROL_EVENT_HANDLER_H__




/**
 * Helper class to subscribe to Control's events.
 * Several functors can be bound to different Control's events
 */
class ControlEventHandler : public gameplay::Control::Listener, Noncopyable
{
public:
    /**
     * Stop listener. No events' functors get called after that.
     */
    void stopEventListener();
    
    /**
     * Start listener. Listener is started by default in constructor.
     */
    void startEventListener();

protected:
    /**
     * Constructor.
     */
    ControlEventHandler();

    /**
     * Destructor.
     */
    virtual ~ControlEventHandler();

    /**
     * Bind functor to control's event.
     *
     * @param control Control to listening events for.
     * @param evt Event type.
     * @param fn Functor to invoke when event occurs.
     */
    void bindControlEvent(gameplay::Control* control, gameplay::Control::Listener::EventType evt, std::function<void(gameplay::Control*)> fn);

private:
    virtual void controlEvent(gameplay::Control* control, gameplay::Control::Listener::EventType evt);

    std::map<std::pair<gameplay::Control *, gameplay::Control::Listener::EventType>, std::function<void(gameplay::Control*)> > _controlEventHandlers;
    bool _handlingEvents;
};




#endif // __DFG_CONTROL_EVENT_HANDLER_H__
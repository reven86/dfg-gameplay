#include "pch.h"
#include "control_event_handler.h"





ControlEventHandler::ControlEventHandler()
    : _handlingEvents(true)
{
}

ControlEventHandler::~ControlEventHandler()
{
}

void ControlEventHandler::bindControlEvent(gameplay::Control * control, gameplay::Control::Listener::EventType evt, std::function<void(gameplay::Control *)> fn)
{
    control->addListener(this, evt);
    _controlEventHandlers.insert(std::make_pair(std::make_pair(control, evt), fn));
}

void ControlEventHandler::controlEvent(gameplay::Control* control, gameplay::Control::Listener::EventType evt)
{
    if (!_handlingEvents)
        return;

    auto it = _controlEventHandlers.find(std::make_pair(control, evt));
    if (it != _controlEventHandlers.end())
        (*it).second(control);
}

void ControlEventHandler::stopEventListener()
{
    _handlingEvents = false;
}

void ControlEventHandler::startEventListener()
{
    _handlingEvents = true;
}
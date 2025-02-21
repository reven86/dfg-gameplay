#include "pch.h"
#include "control_event_handler.h"





ControlEventHandler::ControlEventHandler()
    : _handlingEvents(true)
{
}

ControlEventHandler::~ControlEventHandler()
{
}

void ControlEventHandler::bindControlEvent(gameplay::Control * control, gameplay::Control::Listener::EventType evt, std::function<void()> fn)
{
    GP_ASSERT(_controlEventHandlers.find(std::make_pair(control, evt)) == _controlEventHandlers.end());

    control->addListener(this, evt);
    _controlEventHandlers.insert(std::make_pair(std::make_pair(control, evt), fn));
}

void ControlEventHandler::unbindControlEvent(gameplay::Control * control, gameplay::Control::Listener::EventType evt)
{
    control->removeListener(this);
    _controlEventHandlers.erase(std::make_pair(control, evt));
}

void ControlEventHandler::controlEvent(gameplay::Control* control, gameplay::Control::Listener::EventType evt)
{
    if (!_handlingEvents)
        return;

    auto it = _controlEventHandlers.find(std::make_pair(control, evt));
    if (it != _controlEventHandlers.end())
        (*it).second();
}

void ControlEventHandler::stopEventListener()
{
    _handlingEvents = false;
}

void ControlEventHandler::startEventListener()
{
    _handlingEvents = true;
}
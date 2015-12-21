#include "pch.h"
#include "expanded_tab.h"





static std::vector<ExpandedTab*> __tabs;

ExpandedTab::ExpandedTab()
    : _widthMinimized(0.0f)
    , _widthMaximized(100.0f)
    , _state(MAXIMIZED)
    , _animationDuration(600)
    , _animationInterpolator(gameplay::Curve::QUARTIC_IN_OUT)
    , _stateChangeClip(NULL)
    , _clickWaitClip(NULL)
{
    __tabs.push_back(this);
}

ExpandedTab::~ExpandedTab()
{
    // Remove this tab from the global list.
    std::vector<ExpandedTab*>::iterator it = std::find(__tabs.begin(), __tabs.end(), this);
    if (it != __tabs.end())
    {
        __tabs.erase(it);
    }
}

void ExpandedTab::setState(ExpandedTab::States state, bool immediately)
{
    if (_state == state)
        return;

    if (state == MAXIMIZED)
        ExpandedTab::minimizeAll(_groupId);

    _state = state;
    notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);

    if (_stateChangeClip)
    {
        _stateChangeClip->stop();
        _stateChangeClip = NULL;
    }

    if (_state != HIDDEN)
        setVisible(true);

    float to[] = { 0.0f, _widthMinimized, _widthMaximized };
    if (immediately)
    {
        setWidth(to[_state]);
    }
    else
    {
        float from = getWidth();

        gameplay::Animation * animation = createAnimationFromTo("tab-change-state", gameplay::Control::ANIMATE_SIZE_WIDTH, &from, &to[_state], _animationInterpolator, _animationDuration);
        _stateChangeClip = animation->getClip();

        if (_state == HIDDEN || _state == MINIMIZED && _widthMinimized <= 0.0f)
            _stateChangeClip->addEndListener(this);

        _stateChangeClip->play();
    }
}

void ExpandedTab::animationEvent(gameplay::AnimationClip * clip, gameplay::AnimationClip::Listener::EventType type)
{
    GP_ASSERT(type == gameplay::AnimationClip::Listener::END);

    if (clip == _clickWaitClip)
    {
        gameplay::Container::notifyListeners(gameplay::Control::Listener::CLICK);
    }
    else if (clip == _stateChangeClip)  // if we stop previous _stateChangeClip we should not react on its END event
    {
        if (_state == HIDDEN || _state == MINIMIZED && _widthMinimized <= 0.0f)
            setVisible(false);
    }
}

gameplay::Control * ExpandedTab::create(gameplay::Theme::Style * style, gameplay::Properties * properties)
{
    ExpandedTab* tab = new ExpandedTab();
    tab->initialize(tab->getTypeName(), style, properties);
    return tab;
}

void ExpandedTab::initialize(const char * typeName, gameplay::Theme::Style * style, gameplay::Properties * properties)
{
    gameplay::Container::initialize(typeName, style, properties);

    if (properties)
    {
        // Make tabs to consume input events by default (all other containers don't)
        _consumeInputEvents = properties->getBool("consumeInputEvents", true);

        _widthMinimized = properties->getFloat("widthMinimized");
        _widthMaximized = getWidth();

        const char* groupId = properties->getString("group");
        if (groupId)
        {
            _groupId = groupId;
        }

        const char * state = properties->getString("state");
        if (gameplay::strcmpnocase(state, "HIDDEN") == 0)
        {
            setState(HIDDEN, true);
        }
        else if (gameplay::strcmpnocase(state, "MINIMIZED") == 0)
        {
            setState(MINIMIZED, true);
        }
        else
        {
            // state is maximized by default, force other tabs with same group to minimize themselves
            GP_ASSERT(_state == MAXIMIZED);
            setState(MAXIMIZED, true);
        }

        const char * interpolator = properties->getString("animationInterpolator");
        if (interpolator)
        {
            int type = gameplay::Curve::getInterpolationType(interpolator);
            if (type != -1)
                _animationInterpolator = static_cast<gameplay::Curve::InterpolationType>(type);
        }

        int duration = properties->getInt("animationDuration");
        if (duration > 0)
            _animationDuration = static_cast<unsigned>(duration);
    }
}

const char * ExpandedTab::getTypeName() const
{
    return "ExpandedTab";
}

void ExpandedTab::minimizeAll(const std::string& groupId)
{
    std::vector<ExpandedTab*>::const_iterator it;
    for (it = __tabs.begin(); it < __tabs.end(); ++it)
    {
        ExpandedTab* tab = *it;
        GP_ASSERT(tab);
        if (groupId == tab->_groupId && tab->getState() == MAXIMIZED)
            tab->setState(MINIMIZED);
    }
}

void ExpandedTab::controlEvent(Control::Listener::EventType evt)
{
    Container::controlEvent(evt);
}

void ExpandedTab::notifyListeners(gameplay::Control::Listener::EventType eventType)
{
#if 0
    // NOTE: double click is not yet properly working in case we want to show/hide tab by clicking on empty space once

    // delay actual click event if we're waiting for double click
    if (eventType == gameplay::Control::Listener::CLICK)
    {
        if (_clickWaitClip && _clickWaitClip->isPlaying())
        {
            if (_clickWaitClip->getElapsedTime() > 0)
            {
                // that is second click, fire DOUBLE_CLICK event
                notifyListeners(static_cast<gameplay::Control::Listener::EventType>(Listener::DOUBLE_CLICK));
                return;
            }
        }
        else
        {
            // it's first click, start a waiting animation
            float from = 0;
            float to = 1;
            gameplay::Animation * animation = createAnimationFromTo("double-click-wait", ANIMATE_DOUBLE_CLICK, &from, &to, gameplay::Curve::LINEAR, 300);
            _clickWaitClip = animation->getClip();
            _clickWaitClip->addEndListener(this);
            _clickWaitClip->play();
        }
    }
#endif

    gameplay::Container::notifyListeners(eventType);
}

unsigned int ExpandedTab::getAnimationPropertyComponentCount(int propertyId) const
{
    switch (propertyId)
    {
    case ANIMATE_DOUBLE_CLICK:
        return 1;
    default:
        return Container::getAnimationPropertyComponentCount(propertyId);
    }
}

void ExpandedTab::getAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value)
{
    GP_ASSERT(value);

    switch (propertyId)
    {
    case ANIMATE_DOUBLE_CLICK:
        value->setFloat(0, 0.0f);
        break;
    default:
        gameplay::Container::getAnimationPropertyValue(propertyId, value);
        break;
    }
}

void ExpandedTab::setAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value, float blendWeight)
{
    GP_ASSERT(value);

    switch (propertyId)
    {
    case ANIMATE_DOUBLE_CLICK:
        break;
    default:
        gameplay::Container::setAnimationPropertyValue(propertyId, value, blendWeight);
        break;
    }
}
#include "pch.h"
#include "expanded_tab.h"





static std::vector<ExpandedTab*> __tabs;

ExpandedTab::ExpandedTab()
    : _widthMinimized(0.0f)
    , _widthMaximized(100.0f)
    , _state(MAXIMIZED)
    , _animationDuration(1000)
    , _animationInterpolator(gameplay::Curve::CUBIC_IN_OUT)
    , _stateChangeClip(NULL)
{
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
    if (state == MAXIMIZED)
        ExpandedTab::minimizeAll(_groupId);

    if (_state == state)
        return;

    _state = state;

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

        if (_state == HIDDEN)
            _stateChangeClip->addEndListener(this);

        _stateChangeClip->play();
    }
}

void ExpandedTab::animationEvent(gameplay::AnimationClip * clip, gameplay::AnimationClip::Listener::EventType type)
{
    GP_ASSERT(type == gameplay::AnimationClip::Listener::END);
    if (_state == HIDDEN)
        setVisible(false);
}

gameplay::Control * ExpandedTab::create(gameplay::Theme::Style * style, gameplay::Properties * properties)
{
    ExpandedTab* tab = new ExpandedTab();
    tab->initialize(tab->getTypeName(), style, properties);
    __tabs.push_back(tab);
    return tab;
}

void ExpandedTab::initialize(const char * typeName, gameplay::Theme::Style * style, gameplay::Properties * properties)
{
    gameplay::Container::initialize(typeName, style, properties);

    if (properties)
    {
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

    setConsumeInputEvents(true);
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

    switch (evt)
    {
    case Control::Listener::CLICK:
        if (_state == MINIMIZED)
            setState(MAXIMIZED);
        break;
    }
}
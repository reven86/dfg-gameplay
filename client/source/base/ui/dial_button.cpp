#include "pch.h"
#include "dial_button.h"





DialButton::DialButton()
    : _currentItemIndex(INVALID_ITEM_INDEX)
    , _heightCollapsed(100.0f)
    , _heightExpanded(300.0f)
    , _expandAnimationClip(NULL)
    , _expandingFactor(0.0f)
    , _targetScrollPositionOnExpand(0.0f)
    , _animationInterpolator(gameplay::Curve::CUBIC_IN_OUT)
    , _animationWaitDuration(700)
    , _animationDuration(500)
    , _currentItemBeforeTouch(INVALID_ITEM_INDEX)
    , _startScrollingPosition(0.0f, 0.0f)
    , _itemScrollingClip(NULL)
    , _menuState(false)
    , _lastScrollPositionOnPress(0)
    , _rawScrollPosition(0, 0)
    , _freeSliding(false)
{
}

DialButton::~DialButton()
{
}

const char * DialButton::getTypeName() const
{
    return "DialButton";
}

gameplay::Control * DialButton::create(gameplay::Theme::Style* style, gameplay::Properties* properties)
{
    DialButton * button = new DialButton();
    button->initialize(button->getTypeName(), style, properties);
    return button;
}

void DialButton::initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties)
{
    gameplay::Container::initialize(typeName, style, properties);

    _heightCollapsed = getHeight();
    _heightExpanded = properties->getFloat("heightExpanded");
    if (_heightExpanded <= _heightCollapsed)
        _heightExpanded = 2.0f * _heightCollapsed;

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

    duration = properties->getInt("animationWaitDuration");
    if (duration > 0)
        _animationWaitDuration = static_cast<unsigned>(duration);

    _freeSliding = properties->getBool("freeSliding");

    setConsumeInputEvents(true);

    for (gameplay::Control * child : getControls())
        unsetConsumeInputEvents(child);
}

unsigned int DialButton::draw(gameplay::Form * form) const
{
    // do not draw scrollbars
    const_cast<DialButton *>(this)->_scrollBarOpacity = _expandingFactor;

    return gameplay::Container::draw(form);
}

bool DialButton::touchEventScroll(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    if (_menuState)
        return gameplay::Container::touchEventScroll(evt, x, y, contactIndex);

    if (evt == gameplay::Touch::TOUCH_MOVE && _expandingFactor > 0.0f)
    {
        if (_expandingFactor >= 1.0f && (!_expandAnimationClip || !_expandAnimationClip->isPlaying()))
        {
            int localY = y + _absoluteBounds.y - _viewportBounds.y;
            _currentItemIndex = findClosestControlIndex(localY - _scrollPosition.y, true);

            float relativeDistance = 2.0f * y  / _absoluteBounds.height - 1.0f;
            if (relativeDistance > 0.5f)
                _scrollingVelocity.y = _heightCollapsed * (0.5f - relativeDistance) / 0.5f;
            else if (relativeDistance < -0.5f)
                _scrollingVelocity.y = -_heightCollapsed * (0.5f + relativeDistance) / 0.5f;
            else
                _scrollingVelocity.y = 0.0f;

            setDirty(DIRTY_BOUNDS);
            setChildrenDirty(DIRTY_BOUNDS, true);
        }
        return false;
    }

    // using _currentItemBeforeTouch also as a flag that touch is still pressed (INVALID_ITEM_INDEX)
    // remap scroll position only when touch is pressed
    if (_currentItemBeforeTouch == INVALID_ITEM_INDEX)
        _rawScrollPosition = _scrollPosition;

    _scrollPosition = _rawScrollPosition;
    bool res = gameplay::Container::touchEventScroll(evt, x, y, contactIndex);
    _rawScrollPosition = _scrollPosition;

    // reset velocity
    _scrollingVelocity.set(0.0f, 0.0f);

    if (_expandingFactor <= 0.0f && getControlCount() > 0)
    {
        unsigned closestControlIndex = findClosestControlIndex(-_scrollPosition.y, false);

        if (!_freeSliding)
        {
            // remap scrollPosition so it feels like there is a spring inside button
            // that help items to stick to borders when rotating a dial
            gameplay::Control * closestControl = getControl(closestControlIndex);
            float distance = 0.5f * (closestControl->getHeight() + closestControl->getMargin().top + closestControl->getMargin().bottom);
            float relativeOffsetToClosestItem = (closestControl->getY() + closestControl->getHeight() * 0.5f + _scrollPosition.y) / distance - 1.0f;
            relativeOffsetToClosestItem = std::min(1.0f, std::max(-1.0f, relativeOffsetToClosestItem));

            relativeOffsetToClosestItem *= relativeOffsetToClosestItem * relativeOffsetToClosestItem;
            _scrollPosition.y = (relativeOffsetToClosestItem + 1.0f) * distance - closestControl->getY() - closestControl->getHeight() * 0.5f;

            closestControlIndex = findClosestControlIndex(-_scrollPosition.y, false);
        }

        switch (evt)
        {
        case gameplay::Touch::TOUCH_MOVE:
            if (_currentItemBeforeTouch != INVALID_ITEM_INDEX && _currentItemIndex != closestControlIndex)
            {
                _currentItemIndex = closestControlIndex;
                if (_expandAnimationClip)
                {
                    _expandAnimationClip->stop();
                    _expandAnimationClip = NULL;
                }
            }
            break;
        case gameplay::Touch::TOUCH_RELEASE:
            // scroll to nearest item
            if (_currentItemBeforeTouch != INVALID_ITEM_INDEX)
            {
                _currentItemIndex = closestControlIndex;
                scrollToItem(closestControlIndex);
                if (_currentItemBeforeTouch != _currentItemIndex)
                {
                    if (newItemIsAboutToBeSet(_currentItemIndex))
                        notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
                    else
                        _currentItemIndex = _currentItemBeforeTouch;
                }
                _currentItemBeforeTouch = INVALID_ITEM_INDEX;
            }
            break;
        case gameplay::Touch::TOUCH_PRESS:
            _currentItemBeforeTouch = _currentItemIndex == INVALID_ITEM_INDEX ? 0 : _currentItemIndex;
            break;
        }
    }

    return res;
}

unsigned DialButton::findClosestControlIndex(float localY, bool exitOnPositiveOffset) const
{
    float minDistance = FLT_MAX;
    unsigned closestControlIndex = 0;
    unsigned index = 0;
    for (gameplay::Control * control : getControls())
    {
        if (control->isVisible())
        {
            float distance = control->getY() - control->getMargin().top - localY;
            if (exitOnPositiveOffset && distance > -control->getHeight())
                return index;

            if (fabs(distance) < minDistance)
            {
                minDistance = fabs(distance);
                closestControlIndex = index;
                if (distance > 0.0f)
                    return closestControlIndex;
            }
        }
        index++;
    }

    return closestControlIndex < getControlCount() ? closestControlIndex : INVALID_ITEM_INDEX;
}

void DialButton::scrollToItem(unsigned itemIndex, bool immediately)
{
    if (itemIndex >= getControlCount() && itemIndex != INVALID_ITEM_INDEX)
        return;

    if (_itemScrollingClip && _itemScrollingClip->isPlaying())
    {
        _itemScrollingClip->stop();
        _itemScrollingClip = NULL;
    }

    unsigned int lastItem = _currentItemIndex;
    if (_currentItemIndex != itemIndex)
    {
        if (!newItemIsAboutToBeSet(itemIndex))
            return;

        _currentItemIndex = itemIndex;
        notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);

        // controls count may change after notifying listeners
        if (_currentItemIndex >= getControlCount())
        {
            _currentItemIndex = INVALID_ITEM_INDEX;
            notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
            return;
        }
    }

    if (!immediately && lastItem < getControlCount())
    {
        float from = 0.0f;
        float to = 1.0f;

        _startScrollingPosition = _scrollPosition;

        gameplay::Animation * animation = createAnimationFromTo("scrollbar-scroll-to-item", ANIMATE_SCROLL_TO_ITEM, &from, &to,
            gameplay::Curve::QUADRATIC_IN, 200);
        _itemScrollingClip = animation->getClip();
        _itemScrollingClip->play();
    }
    else if (itemIndex < getControlCount())
    {
        updateChildBounds();
        updateBounds();
        gameplay::Control * itemToScrollTo = getControl(itemIndex);
        gameplay::Vector2 desiredScrollPosition(0.0f, -(itemToScrollTo->getY() - itemToScrollTo->getMargin().top));
        setScrollPosition(desiredScrollPosition);
    }
}

unsigned int DialButton::getAnimationPropertyComponentCount(int propertyId) const
{
    switch (propertyId)
    {
    case ANIMATE_SCROLL_TO_ITEM:
        return 1;
    case ANIMATE_BUTTON_EXPANDING:
        return 1;
    default:
        return Container::getAnimationPropertyComponentCount(propertyId);
    }
}

void DialButton::getAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value)
{
    GP_ASSERT(value);

    switch (propertyId)
    {
    case ANIMATE_SCROLL_TO_ITEM:
        {
            GP_ASSERT(_currentItemIndex < getControlCount());
            gameplay::Control * itemToScrollTo = getControl(_currentItemIndex);
            gameplay::Vector2 desiredScrollPosition(0.0f, -(itemToScrollTo->getY() - itemToScrollTo->getMargin().top));
            value->setFloat(0, (_scrollPosition - _startScrollingPosition).length() / (desiredScrollPosition - _startScrollingPosition).length());
        }
        break;
    case ANIMATE_BUTTON_EXPANDING:
        value->setFloat(0, _expandingFactor);
        break;
    default:
        Container::getAnimationPropertyValue(propertyId, value);
        break;
    }
}

void DialButton::setAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value, float blendWeight)
{
    GP_ASSERT(value);

    switch (propertyId)
    {
    case ANIMATE_SCROLL_TO_ITEM:
        if (_currentItemIndex < getControlCount())
        {
            gameplay::Control * itemToScrollTo = getControl(_currentItemIndex);
            gameplay::Vector2 desiredScrollPosition(0.0f, -(itemToScrollTo->getY() - itemToScrollTo->getMargin().top));

            float scrollFactor = value->getFloat(0);
            _scrollPosition = _startScrollingPosition + (desiredScrollPosition - _startScrollingPosition) * scrollFactor * blendWeight;

            setDirty(DIRTY_BOUNDS);
            setChildrenDirty(DIRTY_BOUNDS, true);
        }
        break;
    case ANIMATE_BUTTON_EXPANDING:
        {
            float oldExpandingFactor = _expandingFactor;
            _expandingFactor = value->getFloat(0) * blendWeight;
        
            if (oldExpandingFactor <= 0.0f && _expandingFactor > 0.0f)
                if (!buttonIsAboutToExpandSignal())
                {
                    // break expanding
                    _expandingFactor = 0.0f;
                    buttonExpandingSignal(_expandingFactor);
                    if (_expandAnimationClip)
                    {
                        _expandAnimationClip->stop();
                        _expandAnimationClip = NULL;
                    }
                    _menuState = false;

                    // scroll to nearest item
                    scrollToItem(_currentItemIndex);
                    break;
                }

            float height = gameplay::Curve::lerp(_expandingFactor, _heightCollapsed, _heightExpanded);
            setHeight(height);

            // update scroll as well since the current item should be placed in center of the container
            if (_expandingFactor > 0.0f && _currentItemIndex != INVALID_ITEM_INDEX)
            {
                gameplay::Control * currentItem = getControl(_currentItemIndex);
                float currentItemOffset = currentItem->getY() - currentItem->getMargin().top;
                _scrollPosition.y = gameplay::Curve::lerp(_expandingFactor, -currentItemOffset, _targetScrollPositionOnExpand);
                setDirty(DIRTY_BOUNDS);
                setChildrenDirty(DIRTY_BOUNDS, true);
            }

            buttonExpandingSignal(_expandingFactor);
        }
        break;
    default:
        Container::setAnimationPropertyValue(propertyId, value, blendWeight);
        break;
    }
}

bool DialButton::touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    if (evt == gameplay::Touch::TOUCH_MOVE && _expandingFactor > 0.0f)
        return false;

    bool res = gameplay::Container::touchEvent(evt, x, y, contactIndex);

    if (_menuState && _expandingFactor >= 1.0f)
    {
        if (evt == gameplay::Touch::TOUCH_PRESS)
        {
            _lastScrollPositionOnPress = _scrollPosition.y;
        }
        else if (evt == gameplay::Touch::TOUCH_RELEASE)
        {
            // check that we just tap instead of scrolling a list
            float distance = fabsf(_scrollPosition.y - _lastScrollPositionOnPress);
            if (distance > _heightCollapsed * 0.5f)
            {
                // scrolling, do nothing
                return res;
            }

            // set new current item and fallback to shrink animation
            int localY = y + _absoluteBounds.y - _viewportBounds.y;
            _currentItemBeforeTouch = _currentItemIndex;
            _currentItemIndex = findClosestControlIndex(localY - _scrollPosition.y, true);
        }
    }

    if (evt == gameplay::Touch::TOUCH_RELEASE && _expandAnimationClip && _expandingFactor <= 0.0f)
    {
        // button is about to expand but touch was released a bit earlier
        // expand button and transition to a 'menu' state
        transitionToMenu();
    }
    else if ((evt == gameplay::Touch::TOUCH_PRESS && _expandingFactor <= 0.0f) || evt == gameplay::Touch::TOUCH_RELEASE)
    {
        if (_expandAnimationClip)
        {
            _expandAnimationClip->stop();
            _expandAnimationClip = NULL;
        }

        float from = _heightExpanded > _heightCollapsed ? (getHeight() - _heightCollapsed) / (_heightExpanded - _heightCollapsed) : 1.0f;
        gameplay::Animation * animation = NULL;
        if (evt == gameplay::Touch::TOUCH_PRESS)
        {
            float to = 1.0f;
            unsigned times[] = { 0, _animationWaitDuration, _animationWaitDuration + _animationDuration };
            float values[] = { from, from, to };
            animation = createAnimation("dial-button-expand", ANIMATE_BUTTON_EXPANDING, 3, times, values, _animationInterpolator);

            if (_currentItemIndex != INVALID_ITEM_INDEX)
            {
                GP_ASSERT(_currentItemIndex < getControlCount());
                gameplay::Control * currentItem = getControl(_currentItemIndex);
                float currentItemOffset = currentItem->getY() - currentItem->getMargin().top;
                _targetScrollPositionOnExpand = -currentItemOffset + (_heightExpanded - _heightCollapsed) * 0.5f;
            }
            else
            {
                _targetScrollPositionOnExpand = 0.0f;
            }
        }
        else if (from > 0.0f)
        {
            float to = 0.0f;
            unsigned times[] = { 0, _animationDuration };
            float values[] = { from, to };
            animation = createAnimation("dial-button-shrink", ANIMATE_BUTTON_EXPANDING, 2, times, values, _animationInterpolator);
            _targetScrollPositionOnExpand = _scrollPosition.y;
        }

        if (animation)
        {
            _expandAnimationClip = animation->getClip();
            if (evt == gameplay::Touch::TOUCH_RELEASE)
                _expandAnimationClip->addEndListener(this);
            _expandAnimationClip->play();
        }
    }

    return res;
}

void DialButton::animationEvent(gameplay::AnimationClip* clip, gameplay::AnimationClip::Listener::EventType type)
{
    GP_ASSERT(type == gameplay::AnimationClip::Listener::END);
    _menuState = false;
    if (isEnabled() && _currentItemBeforeTouch != _currentItemIndex)
    {
        if (newItemIsAboutToBeSet(_currentItemIndex))
            notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
        else
            _currentItemIndex = _currentItemBeforeTouch;
    }
    _currentItemBeforeTouch = INVALID_ITEM_INDEX;
    buttonIsCollapsedSignal();
}

void DialButton::removeControl(unsigned int index)
{
    gameplay::Container::removeControl(index);
    if (_currentItemIndex != INVALID_ITEM_INDEX && _currentItemIndex >= index && _currentItemIndex > 0)
        _currentItemIndex--;

    if (_currentItemIndex >= getControlCount())
    {
        _currentItemIndex = INVALID_ITEM_INDEX;
        notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
    }
}

unsigned int DialButton::addControl(gameplay::Control * control)
{
    unsigned int res = gameplay::Container::addControl(control);
    unsetConsumeInputEvents(control);
    return res;
}

void DialButton::insertControl(gameplay::Control * control, unsigned int index)
{
    gameplay::Container::insertControl(control, index);
    unsetConsumeInputEvents(control);
}

void DialButton::transitionToMenu()
{
    if (_menuState)
        return;

    if (_expandAnimationClip)
    {
        _expandAnimationClip->stop();
        _expandAnimationClip = NULL;
    }

    float from = _heightExpanded > _heightCollapsed ? (getHeight() - _heightCollapsed) / (_heightExpanded - _heightCollapsed) : 0.0f;
    float to = 1.0f;
    unsigned times[] = { 0, _animationDuration };
    float values[] = { from, to };
    gameplay::Animation * animation = createAnimation("dial-button-expand", ANIMATE_BUTTON_EXPANDING, 2, times, values, _animationInterpolator);

    if (_currentItemIndex != INVALID_ITEM_INDEX)
    {
        gameplay::Control * currentItem = getControl(_currentItemIndex);
        float currentItemOffset = currentItem->getY() - currentItem->getMargin().top;
        _targetScrollPositionOnExpand = -currentItemOffset + (_heightExpanded - _heightCollapsed) * 0.5f;
    }
    else
    {
        _targetScrollPositionOnExpand = 0.0f;
    }

    _expandAnimationClip = animation->getClip();
    _expandAnimationClip->play();

    _menuState = true;
    _lastScrollPositionOnPress = _targetScrollPositionOnExpand;
}

void DialButton::controlEvent(gameplay::Control::Listener::EventType evt)
{
    gameplay::Container::controlEvent(evt);

    if (_expandingFactor > 0.0f && evt == gameplay::Control::Listener::FOCUS_LOST)
    {
        if (_expandAnimationClip)
        {
            _expandAnimationClip->stop();
            _expandAnimationClip = NULL;
        }

        float from = _heightExpanded > _heightCollapsed ? (getHeight() - _heightCollapsed) / (_heightExpanded - _heightCollapsed) : 1.0f;
        float to = 0.0f;
        unsigned times[] = { 0, _animationDuration };
        float values[] = { from, to };
        gameplay::Animation * animation = createAnimation("dial-button-shrink", ANIMATE_BUTTON_EXPANDING, 2, times, values, _animationInterpolator);
        _targetScrollPositionOnExpand = _scrollPosition.y;

        _expandAnimationClip = animation->getClip();
        _expandAnimationClip->addEndListener(this);
        _expandAnimationClip->play();
    }
}

void DialButton::setEnabled(bool enabled)
{
    gameplay::Container::setEnabled(enabled);

    if (!enabled)
    {
        // reset any behavior related to user input
        stopScrolling();
        if (_currentItemBeforeTouch != INVALID_ITEM_INDEX)
            _currentItemBeforeTouch = INVALID_ITEM_INDEX;

        if (_expandAnimationClip && _expandAnimationClip->isPlaying())
            _expandAnimationClip->removeEndListener(this);
    }
}

void DialButton::unsetConsumeInputEvents(gameplay::Control * control)
{
    control->setConsumeInputEvents(false);

    if (control->isContainer())
        for (gameplay::Control * child : static_cast<gameplay::Container*>(control)->getControls())
            unsetConsumeInputEvents(child);
}
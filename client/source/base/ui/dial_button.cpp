#include "pch.h"
#include "dial_button.h"





DialButton::DialButton()
    : _currentItemIndex(0)
    , _heightCollapsed(100.0f)
    , _heightExpanded(300.0f)
    , _expandAnimationClip(NULL)
    , _expandingFactor(0.0f)
    , _targetScrollPositionOnExpand(0.0f)
    , _animationInterpolator(gameplay::Curve::CUBIC_IN_OUT)
    , _animationWaitDuration(1000)
    , _animationDuration(1000)
    , _currentItemBeforeTouch((unsigned)~0)
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
        child->setConsumeInputEvents(false);
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

    // using _currentItemBeforeTouch also as a flag that touch is still pressed ()
    // remap scroll position only touch is pressed
    if (_currentItemBeforeTouch >= getControlCount())
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
            //GP_LOG("%f", relativeOffsetToClosestItem);
            relativeOffsetToClosestItem = std::min(1.0f, std::max(-1.0f, relativeOffsetToClosestItem));

            relativeOffsetToClosestItem *= relativeOffsetToClosestItem * relativeOffsetToClosestItem;
            _scrollPosition.y = (relativeOffsetToClosestItem + 1.0f) * distance - closestControl->getY() - closestControl->getHeight() * 0.5f;

            closestControlIndex = findClosestControlIndex(-_scrollPosition.y, false);
        }

        switch (evt)
        {
        case gameplay::Touch::TOUCH_MOVE:
            if (_currentItemBeforeTouch < getControlCount() && _currentItemIndex != closestControlIndex)
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
            if (_currentItemBeforeTouch < getControlCount())
            {
                _currentItemIndex = closestControlIndex;
                scrollToItem(closestControlIndex);
                if (_currentItemBeforeTouch != _currentItemIndex)
                    notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
                _currentItemBeforeTouch = (unsigned)~0;
            }
            break;
        case gameplay::Touch::TOUCH_PRESS:
            _currentItemBeforeTouch = _currentItemIndex;
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

    return closestControlIndex;
}

void DialButton::scrollToItem(unsigned itemIndex, bool immediately)
{
    if (itemIndex >= getControlCount())
        return;

    if (_itemScrollingClip && _itemScrollingClip->isPlaying())
    {
        _itemScrollingClip->stop();
        _itemScrollingClip = NULL;
    }

    unsigned int lastItem = _currentItemIndex;
    if (_currentItemIndex != itemIndex)
    {
        _currentItemIndex = itemIndex;
        notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
    }

    if (!immediately)
    {
        float from = 0.0f;
        float to = 1.0f;

        float scrollDistance = fabsf(static_cast<float>(lastItem)-itemIndex);

        _startScrollingPosition = _scrollPosition;

        gameplay::Animation * animation = createAnimationFromTo("scrollbar-scroll-to-item", ANIMATE_SCROLL_TO_ITEM, &from, &to,
            gameplay::Curve::QUADRATIC_IN, std::max(200UL, static_cast<unsigned long>(scrollDistance * 200)));
        _itemScrollingClip = animation->getClip();
        _itemScrollingClip->play();
    }
    else
    {
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
            _expandingFactor = value->getFloat(0) * blendWeight;
        
            float height = gameplay::Curve::lerp(_expandingFactor, _heightCollapsed, _heightExpanded);
            setHeight(height);

            // update scroll as well since the current item should be placed in center of the container
            if (_expandingFactor > 0.0f && getControlCount() > 0)
            {
                gameplay::Control * currentItem = getControl(_currentItemIndex);
                float currentItemOffset = currentItem->getY() - currentItem->getMargin().top;
                _scrollPosition.y = gameplay::Curve::lerp(_expandingFactor, -currentItemOffset, _targetScrollPositionOnExpand);
                setDirty(DIRTY_BOUNDS);
                setChildrenDirty(DIRTY_BOUNDS, true);
            }
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
            _currentItemIndex = findClosestControlIndex(localY - _scrollPosition.y, true);
        }
    }

    if (evt == gameplay::Touch::TOUCH_RELEASE && _expandAnimationClip && _expandingFactor <= 0.0f)
    {
        // button is about to expand but touch was released a bit earlier
        // expand button and transition to a 'menu' state

        if (_expandAnimationClip)
        {
            _expandAnimationClip->stop();
            _expandAnimationClip = NULL;
        }

        float from = (getHeight() - _heightCollapsed) / (_heightExpanded - _heightCollapsed);
        float to = 1.0f;
        unsigned times[] = { 0, _animationDuration };
        float values[] = { from, to };
        gameplay::Animation * animation = createAnimation("dial-button-expand", ANIMATE_BUTTON_EXPANDING, 2, times, values, _animationInterpolator);

        if (getControlCount() > 0)
        {
            gameplay::Control * currentItem = getControl(_currentItemIndex);
            float currentItemOffset = currentItem->getY() - currentItem->getMargin().top;
            _targetScrollPositionOnExpand = -currentItemOffset + (_heightExpanded - _heightCollapsed) * 0.5f;
        }

        _expandAnimationClip = animation->getClip();
        _expandAnimationClip->play();

        _menuState = true;
        _lastScrollPositionOnPress = _targetScrollPositionOnExpand;
    }
    else if ((evt == gameplay::Touch::TOUCH_PRESS && _expandingFactor <= 0.0f) || evt == gameplay::Touch::TOUCH_RELEASE)
    {
        if (_expandAnimationClip)
        {
            _expandAnimationClip->stop();
            _expandAnimationClip = NULL;
        }

        float from = (getHeight() - _heightCollapsed) / (_heightExpanded - _heightCollapsed);
        gameplay::Animation * animation;
        if (evt == gameplay::Touch::TOUCH_PRESS)
        {
            float to = 1.0f;
            unsigned times[] = { 0, _animationWaitDuration, _animationWaitDuration + _animationDuration };
            float values[] = { from, from, to };
            animation = createAnimation("dial-button-expand", ANIMATE_BUTTON_EXPANDING, 3, times, values, _animationInterpolator);

            if (getControlCount() > 0)
            {
                gameplay::Control * currentItem = getControl(_currentItemIndex);
                float currentItemOffset = currentItem->getY() - currentItem->getMargin().top;
                _targetScrollPositionOnExpand = -currentItemOffset + (_heightExpanded - _heightCollapsed) * 0.5f;
            }
        }
        else
        {
            float to = 0.0f;
            unsigned times[] = { 0, _animationDuration };
            float values[] = { from, to };
            animation = createAnimation("dial-button-expand", ANIMATE_BUTTON_EXPANDING, 2, times, values, _animationInterpolator);
            _targetScrollPositionOnExpand = _scrollPosition.y;
        }

        _expandAnimationClip = animation->getClip();
        if (evt == gameplay::Touch::TOUCH_RELEASE)
            _expandAnimationClip->addEndListener(this);
        _expandAnimationClip->play();
    }

    return res;
}

void DialButton::animationEvent(gameplay::AnimationClip* clip, gameplay::AnimationClip::Listener::EventType type)
{
    GP_ASSERT(type == gameplay::AnimationClip::Listener::END);
    _menuState = false;
    if (_currentItemBeforeTouch < getControlCount() && _currentItemBeforeTouch != _currentItemIndex)
    {
        notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
        _currentItemBeforeTouch = (unsigned)~0;
    }
}

unsigned int DialButton::drawBorder(gameplay::Form * form) const
{
    unsigned int drawCalls = gameplay::Container::drawBorder(form);

    if (getControlCount() > 0)
    {
        gameplay::SpriteBatch* batch = _style->getTheme()->getSpriteBatch();
        startBatch(form, batch);

        gameplay::Control * currentItem = getControl(_currentItemIndex);
        gameplay::Rectangle itemBounds;

        gameplay::Rectangle::intersect(currentItem->getAbsoluteBounds(), _viewportClipBounds, &itemBounds);

        // TODO: Use normal UV coords from skin settings
        batch->draw(itemBounds.x, itemBounds.y, itemBounds.width, itemBounds.height,
            175.0f / 256.0f, 1.0f - 20.0f / 256.0f, 175.0f / 256.0f, 1.0f - 20.0f / 256.0f, gameplay::Vector4(1.0f, 1.0f, 1.0f, 0.33f), _viewportClipBounds);
        ++drawCalls;

        finishBatch(form, batch);
    }

    return drawCalls;
}

void DialButton::removeControl(unsigned int index)
{
    gameplay::Container::removeControl(index);
    if (_currentItemIndex >= index && _currentItemIndex > 0)
        _currentItemIndex--;
}

unsigned int DialButton::addControl(gameplay::Control * control)
{
    unsigned int res = gameplay::Container::addControl(control);
    control->setConsumeInputEvents(false);
    return res;
}

void DialButton::insertControl(gameplay::Control * control, unsigned int index)
{
    gameplay::Container::insertControl(control, index);
    control->setConsumeInputEvents(false);
}
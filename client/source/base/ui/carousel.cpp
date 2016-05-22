#include "pch.h"
#include "carousel.h"





Carousel::Carousel()
    : _currentItemIndex(INVALID_ITEM_INDEX)
    , _currentItemBeforeTouch(INVALID_ITEM_INDEX)
    , _startScrollingPosition(0.0f, 0.0f)
    , _itemScrollingClip(NULL)
    , _rawScrollPosition(0, 0)
    , _freeSliding(false)
    , _passiveState(false)
    , _touchPressX(0)
    , _touchPressY(0)
    , _animationDuration(200)
    , _animationInterpolator(gameplay::Curve::QUADRATIC_IN)
{
}

Carousel::~Carousel()
{
}

const char * Carousel::getTypeName() const
{
    return "Carousel";
}

gameplay::Control * Carousel::create(gameplay::Theme::Style* style, gameplay::Properties* properties)
{
    Carousel * res = new Carousel();
    res->initialize(res->getTypeName(), style, properties);
    return res;
}

void Carousel::initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties)
{
    gameplay::Container::initialize(typeName, style, properties);

    if (properties)
    {
        _freeSliding = properties->getBool("freeSliding", false);
        _passiveState = properties->getBool("passive", false);

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

    if (!_passiveState)
    {
        setReceiveInputEvents(true);

        for (gameplay::Control * child : getControls())
            unsetConsumeInputEvents(child);
    }
}

bool Carousel::touchEventScroll(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    if (evt != gameplay::Touch::TOUCH_PRESS && _currentItemBeforeTouch == INVALID_ITEM_INDEX || _passiveState)
        return false;

    // using _currentItemBeforeTouch also as a flag that touch is still pressed (INVALID_ITEM_INDEX)
    // remap scroll position only when touch is pressed
    if (_currentItemBeforeTouch == INVALID_ITEM_INDEX)
        _rawScrollPosition = _scrollPosition;

    _scrollPosition = _rawScrollPosition;
    bool res = gameplay::Container::touchEventScroll(evt, x, y, contactIndex) || _currentItemBeforeTouch != INVALID_ITEM_INDEX;
    _rawScrollPosition = _scrollPosition;

    // reset velocity
    _scrollingVelocity.set(0.0f, 0.0f);

    if (getControlCount() > 0)
    {
        unsigned closestControlIndex = findClosestControlIndex(-_scrollPosition.x, false);

        if (!_freeSliding)
        {
            // remap scrollPosition so it feels like there is a spring inside button
            // that help items to stick to borders when rotating a dial
            gameplay::Control * closestControl = getControl(closestControlIndex);
            float distance = 0.5f * (closestControl->getWidth() + closestControl->getMargin().left + closestControl->getMargin().bottom);
            float relativeOffsetToClosestItem = (closestControl->getX() + closestControl->getWidth() * 0.5f + _scrollPosition.x) / distance - 1.0f;
            relativeOffsetToClosestItem = std::min(1.0f, std::max(-1.0f, relativeOffsetToClosestItem));

            relativeOffsetToClosestItem *= relativeOffsetToClosestItem * relativeOffsetToClosestItem;
            _scrollPosition.x = (relativeOffsetToClosestItem + 1.0f) * distance - closestControl->getX() - closestControl->getWidth() * 0.5f;

            closestControlIndex = findClosestControlIndex(-_scrollPosition.x, false);
        }

        switch (evt)
        {
        case gameplay::Touch::TOUCH_MOVE:
            if (_currentItemBeforeTouch != INVALID_ITEM_INDEX && _currentItemIndex != closestControlIndex)
            {
                _currentItemIndex = closestControlIndex;
            }
            break;
        case gameplay::Touch::TOUCH_RELEASE:
            // scroll to nearest item
            if (_currentItemBeforeTouch != INVALID_ITEM_INDEX)
            {
                _currentItemIndex = closestControlIndex;
                scrollToItem(closestControlIndex);
                if (_currentItemBeforeTouch != _currentItemIndex)
                    notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
                _currentItemBeforeTouch = INVALID_ITEM_INDEX;
            }
            break;
        case gameplay::Touch::TOUCH_PRESS:
            if (_currentItemIndex != INVALID_ITEM_INDEX)
                _currentItemBeforeTouch = _currentItemIndex;
            _touchPressX = _absoluteClipBounds.x + x;
            _touchPressY = _absoluteClipBounds.y + y;
            break;
        }
    }

    return res;
}

unsigned Carousel::findClosestControlIndex(float localX, bool exitOnPositiveOffset) const
{
    float minDistance = FLT_MAX;
    unsigned closestControlIndex = 0;
    unsigned index = 0;
    for (gameplay::Control * control : getControls())
    {
        if (control->isVisible())
        {
            float distance = control->getX() - control->getMargin().left - localX;
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

void Carousel::scrollToItem(unsigned itemIndex, bool immediately)
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
            _animationInterpolator, _animationDuration);
        _itemScrollingClip = animation->getClip();
        _itemScrollingClip->play();
    }
    else
    {
        updateChildBounds();
        updateBounds();
        gameplay::Control * itemToScrollTo = getControl(itemIndex);
        gameplay::Vector2 desiredScrollPosition(-(itemToScrollTo->getX() - itemToScrollTo->getMargin().left), 0.0f);
        setScrollPosition(desiredScrollPosition);
    }
}

unsigned int Carousel::getAnimationPropertyComponentCount(int propertyId) const
{
    switch (propertyId)
    {
    case ANIMATE_SCROLL_TO_ITEM:
        return 1;
    default:
        return Container::getAnimationPropertyComponentCount(propertyId);
    }
}

void Carousel::getAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value)
{
    GP_ASSERT(value);

    switch (propertyId)
    {
    case ANIMATE_SCROLL_TO_ITEM:
        {
            GP_ASSERT(_currentItemIndex < getControlCount());
            gameplay::Control * itemToScrollTo = getControl(_currentItemIndex);
            gameplay::Vector2 desiredScrollPosition(-(itemToScrollTo->getX() - itemToScrollTo->getMargin().left), 0.0f);
            value->setFloat(0, (_scrollPosition - _startScrollingPosition).length() / (desiredScrollPosition - _startScrollingPosition).length());
        }
        break;
    default:
        Container::getAnimationPropertyValue(propertyId, value);
        break;
    }
}

void Carousel::setAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value, float blendWeight)
{
    GP_ASSERT(value);

    switch (propertyId)
    {
    case ANIMATE_SCROLL_TO_ITEM:
        if (_currentItemIndex < getControlCount())
        {
            updateChildBounds();
            updateBounds();
            gameplay::Control * itemToScrollTo = getControl(_currentItemIndex);
            gameplay::Vector2 desiredScrollPosition(-(itemToScrollTo->getX() - itemToScrollTo->getMargin().left), 0.0f);

            float scrollFactor = value->getFloat(0);
            _scrollPosition = _startScrollingPosition + (desiredScrollPosition - _startScrollingPosition) * scrollFactor * blendWeight;

            setDirty(DIRTY_BOUNDS);
            setChildrenDirty(DIRTY_BOUNDS, true);
        }
        break;
    default:
        Container::setAnimationPropertyValue(propertyId, value, blendWeight);
        break;
    }
}

void Carousel::removeControl(unsigned int index)
{
    gameplay::Container::removeControl(index);
    if (_currentItemIndex != INVALID_ITEM_INDEX && _currentItemIndex >= index)
    {
        _currentItemIndex--;
        if (getControlCount() > 0 && _currentItemIndex < getControlCount())
        {
            updateChildBounds();
            updateBounds();
            gameplay::Control * itemToScrollTo = getControl(_currentItemIndex);
            gameplay::Vector2 desiredScrollPosition(-(itemToScrollTo->getX() - itemToScrollTo->getMargin().left), 0.0f);
            setScrollPosition(desiredScrollPosition);
        }
    }

    if (_currentItemIndex >= getControlCount())
    {
        _currentItemIndex = INVALID_ITEM_INDEX;
        notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
    }
}

unsigned int Carousel::addControl(gameplay::Control * control)
{
    unsigned int res = gameplay::Container::addControl(control);
    return res;
}

void Carousel::insertControl(gameplay::Control * control, unsigned int index)
{
    gameplay::Container::insertControl(control, index);
}

void Carousel::setEnabled(bool enabled)
{
    gameplay::Container::setEnabled(enabled);

    if (!enabled)
    {
        // reset any behavior related to user input
        stopScrolling();
        if (_currentItemBeforeTouch != INVALID_ITEM_INDEX)
            _currentItemBeforeTouch = INVALID_ITEM_INDEX;
    }
}

void Carousel::unsetConsumeInputEvents(gameplay::Control * control)
{
    control->setConsumeInputEvents(false);

    if (control->isContainer())
        for (gameplay::Control * child : static_cast<gameplay::Container*>(control)->getControls())
            unsetConsumeInputEvents(child);
}

void Carousel::updateBounds()
{
    // calculate bounds as usual
    Container::updateBounds();

    // if we're not scrolling the carousel, update our bounds to currently selected child's bounds
    if ((_autoSize & AUTO_SIZE_HEIGHT) != 0 && _currentItemIndex != INVALID_ITEM_INDEX &&
        (!_itemScrollingClip || !_itemScrollingClip->isPlaying()) && _currentItemBeforeTouch == INVALID_ITEM_INDEX)
    {
        gameplay::Control * currentItem = getControl(_currentItemIndex);
        GP_ASSERT(currentItem);

        // Size ourself to tightly fit the height of our children
        float height = currentItem->getHeight() + currentItem->getMargin().bottom + currentItem->getY();
        height += getBorder(NORMAL).top + getBorder(NORMAL).bottom + getPadding().top + getPadding().bottom;
        setHeightInternal(height);

        // compute total bounds of container
        Control::updateBounds();
    }
}

void Carousel::notifyListeners(gameplay::Control::Listener::EventType eventType)
{
    // don't send a click event when we're rotating carousel
    if (eventType == gameplay::Control::Listener::CLICK && _currentItemBeforeTouch != INVALID_ITEM_INDEX && (_rawScrollPosition - _scrollPosition).lengthSquared() > 25.0f)
        return;

    Container::notifyListeners(eventType);
}
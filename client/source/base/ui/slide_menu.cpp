#include "pch.h"
#include "slide_menu.h"





SlideMenu::SlideMenu()
    : _currentItemIndex(INVALID_ITEM_INDEX)
    , _animationDuration(0.2f)
    , _animationInterpolator(gameplay::Curve::QUARTIC_IN_OUT)
    , _itemScrollingClip(NULL)
{
}

SlideMenu::~SlideMenu()
{
}

const char * SlideMenu::getTypeName() const
{
    return "SlideMenu";
}

gameplay::Control * SlideMenu::create(gameplay::Theme::Style* style, gameplay::Properties* properties)
{
    SlideMenu * res = new SlideMenu();
    res->initialize(res->getTypeName(), style, properties);
    return res;
}

void SlideMenu::initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties)
{
    gameplay::Container::initialize(typeName, style, properties);

    if (properties)
    {
        const char * interpolator = properties->getString("animationInterpolator");
        if (interpolator)
        {
            int type = gameplay::Curve::getInterpolationType(interpolator);
            if (type != -1)
                _animationInterpolator = static_cast<gameplay::Curve::InterpolationType>(type);
        }

        float duration = properties->getFloat("animationDuration");
        if (duration > 0)
            _animationDuration = duration;
    }
}

void SlideMenu::scrollToItem(unsigned itemIndex, bool immediately)
{
    if (itemIndex >= getControlCount() || itemIndex == _currentItemIndex)
        return;

    if (_itemScrollingClip && _itemScrollingClip->isPlaying())
    {
        _itemScrollingClip->removeEndListener(this);
        _itemScrollingClip->stop();
        _itemScrollingClip = NULL;
    }

    unsigned int lastItem = _currentItemIndex;
    if (_currentItemIndex != itemIndex)
    {
        _currentItemIndex = itemIndex;
        notifyListeners(gameplay::Control::Listener::VALUE_CHANGED);
    }

    if (!immediately && lastItem < getControlCount())
    {
        auto fadeAnimation = _fadeAnimations.find(_controls[itemIndex]);
        if (fadeAnimation != _fadeAnimations.end())
        {
            (*fadeAnimation).second->removeEndListener(this);
            (*fadeAnimation).second->stop();
        }

        float from = _controls[itemIndex]->isVisible() ? _controls[itemIndex]->getOpacity() : 0.0f;
        float to = 1.0f;
        _fadeAnimations[_controls[itemIndex]] = _controls[itemIndex]->createAnimationFromTo("scroll-fade-in", ANIMATE_OPACITY, &from, &to, gameplay::Curve::LINEAR, _animationDuration)->getClip();
        _fadeAnimations[_controls[itemIndex]]->play();
        itemFadeIn(itemIndex);

        fadeAnimation = _fadeAnimations.find(_controls[lastItem]);
        if (fadeAnimation != _fadeAnimations.end())
        {
            (*fadeAnimation).second->removeEndListener(this);
            (*fadeAnimation).second->stop();
        }

        from = _controls[lastItem]->isVisible() ? _controls[lastItem]->getOpacity() : 1.0f;
        to = 0.0f;
        _fadeAnimations[_controls[lastItem]] = _controls[lastItem]->createAnimationFromTo("scroll-fade-out", ANIMATE_OPACITY, &from, &to, gameplay::Curve::LINEAR, _animationDuration)->getClip();
        _fadeAnimations[_controls[lastItem]]->addEndListener(this);
        _fadeAnimations[_controls[lastItem]]->play();
        itemFadeOut(lastItem);

        for (unsigned i = 0; i < getControlCount(); i++)
        {
            bool visible = i == _currentItemIndex || i == lastItem;

            // stop all fade animations for items that doesn't participate in this sliding
            if (_controls[i]->isVisible() && !visible)
            {
                fadeAnimation = _fadeAnimations.find(_controls[i]);
                if (fadeAnimation != _fadeAnimations.end())
                {
                    (*fadeAnimation).second->removeEndListener(this);
                    (*fadeAnimation).second->stop();
                }
            }

            _controls[i]->setVisible(visible);
        }

        from = 0.0f;
        to = 0.0f;

        if (_layout->getType() == gameplay::Layout::LAYOUT_HORIZONTAL)
        {
            if ((_alignment & ALIGN_RIGHT) != 0)
            {
                if (itemIndex < lastItem)
                {
                    from = 0.0f;
                    to = _controls[lastItem]->getWidth() + _controls[lastItem]->getMargin().right + _controls[lastItem]->getMargin().left;
                }
                else
                {
                    GP_ASSERT(itemIndex > lastItem);
                    from = _controls[itemIndex]->getWidth() + _controls[itemIndex]->getMargin().right + _controls[itemIndex]->getMargin().left;
                    to = 0.0f;
                }
            }
            else if ((_alignment & ALIGN_LEFT) != 0)
            {
                if (itemIndex < lastItem)
                {
                    from = _controls[itemIndex]->getWidth() + _controls[itemIndex]->getMargin().right + _controls[itemIndex]->getMargin().left;
                    to = 0.0f;
                }
                else
                {
                    GP_ASSERT(itemIndex > lastItem);
                    from = 0.0f;
                    to = _controls[lastItem]->getWidth() + _controls[lastItem]->getMargin().right + _controls[lastItem]->getMargin().left;
                }
            }
        }
        else if (_layout->getType() == gameplay::Layout::LAYOUT_VERTICAL)
        {
            if ((_alignment & ALIGN_BOTTOM) != 0)
            {
                if (itemIndex < lastItem)
                {
                    from = 0.0f;
                    to = _controls[lastItem]->getHeight() + _controls[lastItem]->getMargin().top + _controls[lastItem]->getMargin().bottom;
                }
                else
                {
                    GP_ASSERT(itemIndex > lastItem);
                    from = _controls[itemIndex]->getHeight() + _controls[itemIndex]->getMargin().top + _controls[itemIndex]->getMargin().bottom;
                    to = 0.0f;
                }
            }
            else if ((_alignment & ALIGN_TOP) != 0)
            {
                if (itemIndex < lastItem)
                {
                    from = _controls[itemIndex]->getHeight() + _controls[itemIndex]->getMargin().top + _controls[itemIndex]->getMargin().bottom;
                    to = 0.0f;
                }
                else
                {
                    GP_ASSERT(itemIndex > lastItem);
                    from = 0.0f;
                    to = _controls[lastItem]->getHeight() + _controls[lastItem]->getMargin().top + _controls[lastItem]->getMargin().bottom;
                }
            }
        }

        gameplay::Animation * animation = createAnimationFromTo("menu-slide", ANIMATE_MENU_SLIDE, &from, &to,
            _animationInterpolator, _animationDuration);
        _itemScrollingClip = animation->getClip();
        _itemScrollingClip->play();
        _itemScrollingClip->addEndListener(this);
    }
    else
    {
        for (unsigned i = 0; i < getControlCount(); i++)
            _controls[i]->setVisible(i == _currentItemIndex);
    }
}

void SlideMenu::previewItem(unsigned itemIndex)
{
    // do not preview any items while scrolling
    if (_itemScrollingClip && _itemScrollingClip->isPlaying())
        return;

    unsigned i = 0;
    gameplay::Theme::Margin extraMargin;
    for (i = 0; i < getControlCount(); i++)
    {
        if (i == _currentItemIndex)
            continue;

        auto currentFadeAnimation = _fadeAnimations.find(_controls[i]);

        if (_controls[i]->isVisible() && i != itemIndex)
        {
            float from = _controls[i]->getOpacity();
            float to = 0.0f;

            if (currentFadeAnimation != _fadeAnimations.end())
            {
                (*currentFadeAnimation).second->removeEndListener(this);
                (*currentFadeAnimation).second->stop();
            }

            _fadeAnimations[_controls[i]] = _controls[i]->createAnimationFromTo("preview-fade-out", ANIMATE_OPACITY, &from, &to, gameplay::Curve::LINEAR, _animationDuration)->getClip();
            _fadeAnimations[_controls[i]]->addEndListener(this);
            _fadeAnimations[_controls[i]]->play();
            itemFadeOut(i);
        }
        else if (i == itemIndex)
        {
            float from = _controls[i]->isVisible() ? _controls[i]->getOpacity() : 0.0f;
            float to = 1.0f;

            if (currentFadeAnimation != _fadeAnimations.end())
            {
                (*currentFadeAnimation).second->removeEndListener(this);
                (*currentFadeAnimation).second->stop();
            }

            _controls[i]->setVisible(true);
            _fadeAnimations[_controls[i]] = _controls[i]->createAnimationFromTo("preview-fade-in", ANIMATE_OPACITY, &from, &to, gameplay::Curve::LINEAR, _animationDuration)->getClip();
            _fadeAnimations[_controls[i]]->play();
            itemFadeIn(i);
        }

        if (_controls[i]->isVisible())
        {
            if (_layout->getType() == gameplay::Layout::LAYOUT_HORIZONTAL)
            {
                float extraWidth = _controls[i]->getWidth() + _controls[i]->getMargin().right + _controls[i]->getMargin().left;
                if ((_alignment & ALIGN_LEFT) != 0 && i < _currentItemIndex)
                    extraMargin.left -= extraWidth;
                else if ((_alignment & ALIGN_RIGHT) != 0 && i > _currentItemIndex)
                    extraMargin.right -= extraWidth;
            }
            else if (_layout->getType() == gameplay::Layout::LAYOUT_VERTICAL)
            {
                float extraWidth = _controls[i]->getHeight() + _controls[i]->getMargin().top + _controls[i]->getMargin().bottom;
                if ((_alignment & ALIGN_TOP) != 0 && i < _currentItemIndex)
                    extraMargin.top -= extraWidth;
                else if ((_alignment & ALIGN_BOTTOM) != 0 && i > _currentItemIndex)
                    extraMargin.bottom -= extraWidth;
            }
        }
    }

    gameplay::Container::setMargin(_defaultMargin.top + extraMargin.top, _defaultMargin.bottom + extraMargin.bottom, _defaultMargin.left + extraMargin.left, _defaultMargin.right + extraMargin.right);
}

void SlideMenu::bindMenuButton(gameplay::Control * control, unsigned int menuIndex)
{
    bindControlEvent(control, gameplay::Control::Listener::PRESS, [=](gameplay::Control*){ this->previewItem(menuIndex); });
    bindControlEvent(control, gameplay::Control::Listener::RELEASE, [=](gameplay::Control*){ this->previewItem(SlideMenu::INVALID_ITEM_INDEX); });
    bindControlEvent(control, gameplay::Control::Listener::ENTER, [=](gameplay::Control* control){ 
        if (control->getState() == gameplay::Control::ACTIVE) 
            this->previewItem(menuIndex);
    });
    bindControlEvent(control, gameplay::Control::Listener::LEAVE, [=](gameplay::Control* control){ 
        if (control->getState() == gameplay::Control::ACTIVE) 
            this->previewItem(SlideMenu::INVALID_ITEM_INDEX); 
    });
    bindControlEvent(control, gameplay::Control::Listener::CLICK, [=](gameplay::Control*){ this->scrollToItem(menuIndex); });
}

void SlideMenu::unbindMenuButton(gameplay::Control * control)
{
    unbindControlEvent(control, gameplay::Control::Listener::PRESS);
    unbindControlEvent(control, gameplay::Control::Listener::RELEASE);
    unbindControlEvent(control, gameplay::Control::Listener::ENTER);
    unbindControlEvent(control, gameplay::Control::Listener::LEAVE);
    unbindControlEvent(control, gameplay::Control::Listener::CLICK);
}

void SlideMenu::setMargin(float top, float bottom, float left, float right)
{
    _defaultMargin.top = top;
    _defaultMargin.bottom = bottom;
    _defaultMargin.left = left;
    _defaultMargin.right = right;
    gameplay::Container::setMargin(top, bottom, left, right);
}

unsigned int SlideMenu::getAnimationPropertyComponentCount(int propertyId) const
{
    switch (propertyId)
    {
    case ANIMATE_MENU_SLIDE:
        return 1;
    default:
        return Container::getAnimationPropertyComponentCount(propertyId);
    }
}

void SlideMenu::getAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value)
{
    GP_ASSERT(value);

    switch (propertyId)
    {
    case ANIMATE_MENU_SLIDE:
        {
            const gameplay::Theme::Margin& margin = getMargin();
            if (margin.left < _defaultMargin.left)
                value->setFloat(0, _defaultMargin.left - margin.left);
            else if (margin.right < _defaultMargin.right)
                value->setFloat(0, _defaultMargin.right - margin.right);
            else if (margin.top < _defaultMargin.top)
                value->setFloat(0, _defaultMargin.top - margin.top);
            else if (margin.bottom < _defaultMargin.bottom)
                value->setFloat(0, _defaultMargin.bottom - margin.bottom);
        }
        break;
    default:
        Container::getAnimationPropertyValue(propertyId, value);
        break;
    }
}

void SlideMenu::setAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value, float blendWeight)
{
    GP_ASSERT(value);

    switch (propertyId)
    {
    case ANIMATE_MENU_SLIDE:
        if (_currentItemIndex < getControlCount())
        {
            float margin = value->getFloat(0);

            gameplay::Theme::Margin extraMargin;
            if (_currentItemIndex != INVALID_ITEM_INDEX)
            {
                if (_layout->getType() == gameplay::Layout::LAYOUT_HORIZONTAL)
                {
                    if ((_alignment & ALIGN_RIGHT) != 0)
                        extraMargin.right = -margin;
                    else if ((_alignment & ALIGN_LEFT) != 0)
                        extraMargin.left = -margin;
                }
                else if (_layout->getType() == gameplay::Layout::LAYOUT_VERTICAL)
                {
                    if ((_alignment & ALIGN_BOTTOM) != 0)
                        extraMargin.bottom = -margin;
                    else if ((_alignment & ALIGN_TOP) != 0)
                        extraMargin.top = -margin;
                }
            }
            gameplay::Container::setMargin(_defaultMargin.top + extraMargin.top, _defaultMargin.bottom + extraMargin.bottom, _defaultMargin.left + extraMargin.left, _defaultMargin.right + extraMargin.right);
        }
        break;
    default:
        Container::setAnimationPropertyValue(propertyId, value, blendWeight);
        break;
    }
}

void SlideMenu::animationEvent(gameplay::AnimationClip* clip, gameplay::AnimationClip::Listener::EventType type)
{
    GP_ASSERT(type == gameplay::AnimationClip::Listener::END);

    for (unsigned i = 0; i < getControlCount(); i++)
        _controls[i]->setVisible(i == _currentItemIndex);
    gameplay::Container::setMargin(_defaultMargin.top, _defaultMargin.bottom, _defaultMargin.left, _defaultMargin.right);
}
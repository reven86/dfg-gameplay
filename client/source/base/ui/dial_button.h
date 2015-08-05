#pragma once

#ifndef __DIAL_BUTTON_H__
#define __DIAL_BUTTON_H__




/**
 * Dial button combines the logic of button and listbox.
 * It can be in one of two states: collapsed and maximized.
 * In collapsed state the user can select next/previous item
 * by sliding it back or forth. When the user makes a long tap
 * on a button, it's expanded into full list view, where user
 * can select an item he's looking for.
 */

class DialButton : public gameplay::Container, public gameplay::AnimationClip::Listener
{
    friend class UIService2;

public:
    /**
     * Get current item index (visible child index).
     */
    unsigned getCurrentItemIndex() const { return _currentItemIndex; };

    /**
     * Set current item. 
     */
    void scrollToItem(unsigned itemIndex);

protected:

    /**
     * Constructor.
     */
    DialButton();

    /**
     * Destructor.
     */
    ~DialButton();

    /**
     * Extends ScriptTarget::getTypeName() to return the type name of this class.
     *
     * Child controls should override this function to return the correct type name.
     *
     * @return The type name of this class: "ExpandedTab"
     * @see ScriptTarget::getTypeName()
     */
    const char* getTypeName() const;

    /**
     * Create a dial button with a given style and properties.
     *
     * @param style The style to apply to this button.
     * @param properties A properties object containing a definition of the button (optional).
     *
     * @return The new button.
     */
    static gameplay::Control* create(gameplay::Theme::Style* style, gameplay::Properties* properties = NULL);

    /**
     * @see Control::initialize
     */
    void initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties);

protected:
    /**
     * Constant used to scroll container to fixed offset.
     */
    static const int ANIMATE_SCROLLBAR_POSITION = 9;

    /**
    * Constant used to animate button expanding process.
    */
    static const int ANIMATE_BUTTON_EXPANDING = 10;


    /**
     * @see Control::draw
     */
    virtual unsigned int draw(gameplay::Form* form) const;

    /**
     * Applies touch events to scroll state.
     *
     * @param evt The touch event that occurred.
     * @param x The x position of the touch in pixels. Left edge is zero.
     * @param y The y position of the touch in pixels. Top edge is zero.
     * @param contactIndex The order of occurrence for multiple touch contacts starting at zero.
     *
     * @return Whether the touch event was consumed by scrolling within this container.
     *
     * @see Container::touchEventScroll
     */
    virtual bool touchEventScroll(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

    /**
     * @see AnimationTarget::getAnimationPropertyComponentCount
     */
    virtual unsigned int getAnimationPropertyComponentCount(int propertyId) const;

    /**
     * @see AnimationTarget::getAnimationProperty
     */
    virtual void getAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value);

    /**
     * @see AnimationTarget::setAnimationProperty
     */
    virtual void setAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value, float blendWeight = 1.0f);

    /**
     * Touch callback on touch events.  Controls return true if they consume the touch event.
     *
     * @param evt The touch event that occurred.
     * @param x The x position of the touch in pixels. Left edge is zero.
     * @param y The y position of the touch in pixels. Top edge is zero.
     * @param contactIndex The order of occurrence for multiple touch contacts starting at zero.
     *
     * @return Whether the touch event was consumed by this control.
     *
     * @see Touch::TouchEvent
     */
    virtual bool touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

    /**
     * Handles when animation event occurs.
     */
    virtual void animationEvent(gameplay::AnimationClip* clip, gameplay::AnimationClip::Listener::EventType type);

    /**
     * @see Control::drawBorder.
     */
    virtual unsigned int drawBorder(gameplay::Form* form) const;

private:
    unsigned findClosestControlIndex(float localY, bool exitOnPositiveOffset) const;

    unsigned _currentItemIndex;
    float _heightCollapsed;
    float _heightExpanded;
    float _expandingFactor;
    float _targetScrollPositionOnExpand;

    gameplay::AnimationClip * _expandAnimationClip;
    gameplay::Curve::InterpolationType _animationInterpolator;
    unsigned int _animationWaitDuration;
    unsigned int _animationDuration;
};



#endif // __DIAL_BUTTON_H__
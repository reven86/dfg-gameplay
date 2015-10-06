#pragma once

#ifndef __DFG_DIAL_BUTTON_H__
#define __DFG_DIAL_BUTTON_H__




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
public:
    sigc::signal<bool>::accumulated<interruptable_accumulator> buttonIsAboutToExpandSignal;
    sigc::signal<void> buttonIsCollapsedSignal;

    static const unsigned int INVALID_ITEM_INDEX = 0xffffffff;

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
     * Get current item index (visible child index).
     */
    unsigned getCurrentItemIndex() const { return _currentItemIndex; };

    /**
     * Scroll to item. 
     * The current item index is changed immediately to itemIndex.
     * Duration of scroll is dependant on scroll distance.
     *
     * @param itemIndex Index of item to scroll to.
     * @param immediately Set current item without scrolling. 
     */
    void scrollToItem(unsigned itemIndex, bool immediately = false);

    /**
     * Adds a new control to this container.
     *
	 * @param control The control to add.
     *
     * @return The index assigned to the new Control.
     */
    virtual unsigned int addControl(gameplay::Control* control);

    /**
     * Inserts a control at a specific index.
     *
     * @param control The control to insert.
     * @param index The index at which to insert the control.
     */
    virtual void insertControl(gameplay::Control* control, unsigned int index);

    /**
     * Remove a control at a specific index.
     *
     * @param index The index from which to remove the control.
     */
    virtual void removeControl(unsigned int index);
    
    /** 
     * Get control's height in collapsed (minimized) state.
     */
    float getHeightCollapsed() const { return _heightCollapsed; };
    
    /**
     * Get control's height in expanded (maximized) state.
     */
    float getHeightExpanded() const { return _heightExpanded; };
    
    /**
     * Set control's height in collapsed (minimized) state.
     */
    void setHeightCollapsed(float height) { _heightCollapsed = height; };
    
    /**
     * Set control's height in expanded (maximized) state.
     */
    void setHeightExpanded(float height) { _heightExpanded = height; };
    
    /**
     * Set the button to a menu state, the button will automatically expand.
     */
    virtual void transitionToMenu();

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
     * @see Control::initialize
     */
    void initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties);

protected:
    /**
     * Constant used to scroll container to fixed offset.
     */
    static const int ANIMATE_SCROLL_TO_ITEM = 9;

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

    /**
     * Called when a control event is fired for this control, before external
     * listeners are notified of the event.
     *
     * @param evt The event type.
     */
    virtual void controlEvent(gameplay::Control::Listener::EventType evt);

private:
    unsigned findClosestControlIndex(float localY, bool exitOnPositiveOffset) const;

    unsigned _currentItemIndex;
    unsigned _currentItemBeforeTouch;
    float _heightCollapsed;
    float _heightExpanded;
    float _expandingFactor;
    float _targetScrollPositionOnExpand;
    gameplay::Vector2 _startScrollingPosition;

    gameplay::AnimationClip * _expandAnimationClip;
    gameplay::AnimationClip * _itemScrollingClip;
    gameplay::Curve::InterpolationType _animationInterpolator;
    unsigned int _animationWaitDuration;
    unsigned int _animationDuration;

    bool _menuState;
    float _lastScrollPositionOnPress;

    bool _freeSliding;
    gameplay::Vector2 _rawScrollPosition;
};



#endif // __DFG_DIAL_BUTTON_H__
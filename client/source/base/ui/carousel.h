#pragma once

#ifndef __DFG_CAROUSEL_H__
#define __DFG_CAROUSEL_H__




/**
 * Carousel allows you to cycle through child elements, it works like Carousel plugin for Bootstrap.
 * Cycling is possible through swiping elements left or right, or by calling appropriate methods.
 */

class Carousel : public gameplay::Container
{
public:
    static const unsigned int INVALID_ITEM_INDEX = 0xffffffff;

    /**
     * Create a Carousel with a given style and properties.
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
     * The current item index is changed immediately to itemIndex,
     * the animation is happend after.
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
    virtual unsigned int addControl(gameplay::Control* control) override;

    /**
     * Inserts a control at a specific index.
     *
     * @param control The control to insert.
     * @param index The index at which to insert the control.
     */
    virtual void insertControl(gameplay::Control* control, unsigned int index) override;

    /**
     * Remove a control at a specific index.
     *
     * @param index The index from which to remove the control.
     */
    virtual void removeControl(unsigned int index) override;
    using gameplay::Container::removeControl;

    /**
     * Enables/Disables a control.
     *
     * @param enabled true if the control is enabled; false if disabled.
     */
    virtual void setEnabled(bool enabled) override;

    /**
     * Get absolute touch's X position when scrolling starts.
     */
    int getTouchPressX() const { return _touchPressX; };

    /**
     * Get absolute touch's Y position when scrolling starts.
     */
    int getTouchPressY() const { return _touchPressY; };

protected:

    /**
     * Constructor.
     */
    Carousel();

    /**
     * Destructor.
     */
    ~Carousel();

    /**
     * Extends ScriptTarget::getTypeName() to return the type name of this class.
     *
     * Child controls should override this function to return the correct type name.
     *
     * @return The type name of this class: "Carousel"
     * @see ScriptTarget::getTypeName()
     */
    virtual const char* getTypeName() const override;

    /**
     * @see Control::initialize
     */
    virtual void initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties) override;

    /**
     * @see Control::updateBounds
     */
    void updateBounds() override;

protected:
    /**
     * Constant used to scroll container to fixed offset.
     */
    static const int ANIMATE_SCROLL_TO_ITEM = 100;


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
    virtual bool touchEventScroll(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex) override;

    /**
     * @see AnimationTarget::getAnimationPropertyComponentCount
     */
    virtual unsigned int getAnimationPropertyComponentCount(int propertyId) const  override;

    /**
     * @see AnimationTarget::getAnimationProperty
     */
    virtual void getAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value) override;

    /**
     * @see AnimationTarget::setAnimationProperty
     */
    virtual void setAnimationPropertyValue(int propertyId, gameplay::AnimationValue* value, float blendWeight = 1.0f) override;

    /**
     * @see Control::notifyListeners
     */
    void notifyListeners(gameplay::Control::Listener::EventType eventType) override;

private:
    unsigned findClosestControlIndex(float localY, bool exitOnPositiveOffset) const;
    void unsetConsumeInputEvents(gameplay::Control * control);

    unsigned _currentItemIndex;
    unsigned _currentItemBeforeTouch;
    gameplay::Vector2 _startScrollingPosition;

    gameplay::AnimationClip * _itemScrollingClip;
    bool _freeSliding;
    bool _passiveState;
    gameplay::Vector2 _rawScrollPosition;
    int _touchPressX;
    int _touchPressY;
    gameplay::Curve::InterpolationType _animationInterpolator;
    unsigned int _animationDuration;
};



#endif // __DFG_CAROUSEL_H__
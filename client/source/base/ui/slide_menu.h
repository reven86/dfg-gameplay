#pragma once

#ifndef __SLIDE_MENU_H__
#define __SLIDE_MENU_H__

#include "control_event_handler.h"




/**
 * Class for hierarchical menus. Basically each menu levels is
 * presented as a column of icons. When user selects one item
 * the new column of icons fades in and moves to the position
 * of old one, and old column moves out.
 */

class SlideMenu : public gameplay::Container, public ControlEventHandler, public gameplay::AnimationClip::Listener
{
public:
    // signal fired when item fades in (either for preview or when sliding)
    sigc::signal<void, unsigned int> itemFadeIn;

    // signal fired when item fades out (either for preview or when sliding)
    sigc::signal<void, unsigned int> itemFadeOut;

    static const unsigned int INVALID_ITEM_INDEX = 0xffffffff;

    /**
     * Create a SlideMenu with a given style and properties.
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
     * Preview item.
     * User may preview the next or previous menus, e.g. when he presses the button in currently active menu list.
     * The press event is handled on game-side and game decides to show the next menu user wants to navigate to,
     * it calls this method. The new menu is shown either left or right from current one if current layout is
     * LAYOUT_HORIZONTAL or above or below the current one if layout is LAYOUT_VERTICAL
     *
     * @param itemIndex Index of menu to preview.
     */
    void previewItem(unsigned itemIndex);

    /**
     * Bind one control to navigate to specified submenu.
     * This bind PRESS, RELEASE, ENTER, LEAVE events to show preview and CLICK to scroll to submenu.
     */
    void bindMenuButton(gameplay::Control * button, unsigned int itemIndex);

    /**
     * Unbind all menu events for specific control.
     */
    void unbindMenuButton(gameplay::Control * button);

    /**
     * Set this control's margin.
     *
     * @param top Height of top margin.
     * @param bottom Height of bottom margin.
     * @param left Width of left margin.
     * @param right Width of right margin.
     */
    virtual void setMargin(float top, float bottom, float left, float right) override;

    /**
     * Get animation interpolator type.
     */
    const gameplay::Curve::InterpolationType& getAnimationInterpolator() const;

    /**
     * Get animation duration in seconds.
     */
    float getAnimationDuration() const;

protected:

    /**
     * Constructor.
     */
    SlideMenu();

    /**
     * Destructor.
     */
    ~SlideMenu();

    /**
     * Extends ScriptTarget::getTypeName() to return the type name of this class.
     *
     * Child controls should override this function to return the correct type name.
     *
     * @return The type name of this class: "SlideMenu"
     * @see ScriptTarget::getTypeName()
     */
    virtual const char* getTypeName() const override;

    /**
     * @see Control::initialize
     */
    virtual void initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties) override;


    /**
     * Constant used to scroll item.
     */
    static const int ANIMATE_MENU_SLIDE = 100;

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
     * Handles when animation event occurs.
     */
    virtual void animationEvent(gameplay::AnimationClip* clip, gameplay::AnimationClip::Listener::EventType type) override;

private:
    gameplay::Curve::InterpolationType _animationInterpolator;
    float _animationDuration;

    unsigned _currentItemIndex;
    gameplay::AnimationClip * _itemScrollingClip;
    std::unordered_map<gameplay::Control *, gameplay::AnimationClip *> _fadeAnimations;

    gameplay::Theme::Margin _defaultMargin;
};




#include "slide_menu.inl"

#endif // __SLIDE_MENU_H__
#pragma once

#ifndef __DFG_EXPANDED_TAB_H__
#define __DFG_EXPANDED_TAB_H__

#undef DOUBLE_CLICK




/** 
 * ExpandedTab is a complex control which represents a tab
 * that can have three states: hidden, minimized and maximized states
 * Transition between states are smooth and uses Animation class.
 * Tabs can be grouped in hierarchy.
 */
class ExpandedTab : public gameplay::Container, public gameplay::AnimationClip::Listener
{
public:
    /**
     * The definition of tab states
     */
    enum States
    {
        HIDDEN,
        MINIMIZED,
        MAXIMIZED,
    };

    /**
     * Tab's specific listener that provides double-click event.
     */
    class Listener : public gameplay::Control::Listener
    {
    public:
        enum EventType
        {
            /**
             * Event triggered after consecutive PRESS and RELEASE events take place
             * within the bounds of a control that is happend twice in a short amount of time.
             */
            DOUBLE_CLICK = 0x2000
        };
    };

    /**
     * Create an expanded tab with a given style and properties.
     *
     * @param style The style to apply to this tab.
     * @param properties A properties object containing a definition of the tab (optional).
     *
     * @return The new tab.
     */
    static gameplay::Control* create(gameplay::Theme::Style* style, gameplay::Properties* properties = NULL);

    /** 
     * Set new tab's state. Triggers animation.
     *
     * @param state Desired state to set.
     * @param immediately Whether state is applied immediately, i.e. no animation is needed.
     */
    void setState(States state, bool immediately = false);

    /**
     * Get current tab's state.
     */
    States getState() const { return _state; };
    
    /**
     * Get control's width in maximized state.
     */
    float getWidthMaximized() const { return _widthMaximized; };

    /**
     * Get control's width in minimized state.
     */
    float getWidthMinimized() const { return _widthMinimized; };
    
    /**
     * Set control's width in maximized state.
     */
    void setWidthMaximized(float width);
    
    /**
     * Set control's width in minimized state.
     */
    void setWidthMinimized(float width);
    
    /**
     * Minimize all tabs in the given group.
     *
     * @param groupId The group to clear.
     * @param immediately Immediately set state without animation.
     */
    static void minimizeAll(const std::string& groupId, bool immediately);

protected:

    /**
     * Constructor.
     */
    ExpandedTab();

    /**
     * Destructor.
     */
    ~ExpandedTab();

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
     * Constant used as a timer to wait for double click event.
     */
    static const int ANIMATE_DOUBLE_CLICK = 100;

    /**
     * Constant used to animate expanding animation. We don't use ANIMATE_SIZE_WIDTH
     * to be able to change widthMaximized during the process os expanding.
     */
    static const int ANIMATE_EXPANDING = 101;

    /**
     * @see Control#controlEvent
     */
    void controlEvent(Control::Listener::EventType evt);

    /**
     * Handles when animation event occurs.
     */
    virtual void animationEvent(gameplay::AnimationClip* clip, gameplay::AnimationClip::Listener::EventType type);

    /**
     * Notify this control's listeners of a specific event.
     *
     * @param eventType The event to trigger.
     */
    virtual void notifyListeners(gameplay::Control::Listener::EventType eventType);

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
     * The tab's group ID.
     */
    std::string _groupId;

private:
    States _state;
    float _widthMaximized;
    float _widthMinimized;
    gameplay::Curve::InterpolationType _animationInterpolator;
    float _animationDuration;
    gameplay::AnimationClip * _stateChangeClip;
    gameplay::AnimationClip * _clickWaitClip;
};



#endif // __DFG_EXPANDED_TAB_H__
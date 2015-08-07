#pragma once

#ifndef __EXPANDED_TAB_H__
#define __EXPANDED_TAB_H__




/** 
 * ExpandedTab is a complex control which represents a tab
 * that can have three states: hidden, minimized and maximized states
 * Transition between states are smooth and uses Animation class.
 * Tabs can be grouped in hierarchy.
 */
class ExpandedTab : public gameplay::Container
{
    friend class UIService2;

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
    void setWidthMaximized(float width) { _widthMaximized = width; };
    
    /**
     * Set control's width in minimized state.
     */
    void setWidthMinimized(float width) { _widthMinimized = width; };
    
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
     * Create an expanded tab with a given style and properties.
     *
     * @param style The style to apply to this tab.
     * @param properties A properties object containing a definition of the tab (optional).
     *
     * @return The new tab.
     */
    static gameplay::Control* create(gameplay::Theme::Style* style, gameplay::Properties* properties = NULL);

    /**
     * @see Control::initialize
     */
    void initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties);

    /**
    * Minimize all tabs in the given group.
    *
    * @param groupId The group to clear.
    */
    static void minimizeAll(const std::string& groupId);

protected:
    /**
     * @see Control#controlEvent
     */
    void controlEvent(Control::Listener::EventType evt);

    /**
     * The tab's group ID.
     */
    std::string _groupId;

private:
    States _state;
    float _widthMaximized;
    float _widthMinimized;
    gameplay::Curve::InterpolationType _animationInterpolator;
    unsigned int _animationDuration;
};



#endif // __EXPANDED_TAB_H__
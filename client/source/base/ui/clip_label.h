#pragma once

#ifndef __DFG_CLIP_LABEL_H__
#define __DFG_CLIP_LABEL_H__



/**
 * ClipLabel is an extension for standard Label class. It supports
 * automaticall text clipping to bounds, inserting '...' at the end.
 * It also tries to use smaller character spacing and font size to 
 * make sure text is fit into bounds.
 */

class ClipLabel : public gameplay::Label
{
public:
    /**
     * Creates a new label.
     *
     * @param id The label id.
     * @param style The label style (optional).
     *
     * @return The new label.
     * @script{create}
     */
    static ClipLabel* create(const char* id, gameplay::Theme::Style* style = NULL);

    /**
     * Create a ClipLabel with a given style and properties.
     *
     * @param style The style to apply to this button.
     * @param properties A properties object containing a definition of the button (optional).
     *
     * @return The new button.
     */
    static gameplay::Control* create(gameplay::Theme::Style* style, gameplay::Properties* properties = NULL);

    /**
     * @see Label::setText
     */
    void setText(const wchar_t* text) override;

    /**
     * Enable or disable text clipping (enabled by default).
     *
     * @param enable Set to true to enable text clipping.
     */
    void enableClipping(bool enable);

    /**
     * Is text clipping enabled?
     */
    bool isClippingEnabled() const { return _clippingActive; };

protected:

    /**
     * Constructor.
     */
    ClipLabel();

    /**
     * Destructor.
     */
    ~ClipLabel();

    /**
     * Extends ScriptTarget::getTypeName() to return the type name of this class.
     *
     * Child controls should override this function to return the correct type name.
     *
     * @return The type name of this class: "ClipLabel"
     * @see ScriptTarget::getTypeName()
     */
    const char* getTypeName() const;

    /**
     * @see Control::initialize
     */
    void initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties);

    /**
     * @see Control::drawText
     */
    virtual unsigned int drawText(gameplay::Form* form) const;

    /**
     * @see Control::updateAbsoluteBounds
     */
    void updateAbsoluteBounds(const gameplay::Vector2& offset);

    /**
     * @see Control::updateState
     */
    void updateState(State state);

private:
    void clipText();

    std::wstring _clippedText;
    bool _clippingActive;
};




#endif // __DFG_CLIP_LABEL_H__
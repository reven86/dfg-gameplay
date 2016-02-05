#pragma once

#ifndef __HTTP_IMAGE_CONTROL_H__
#define __HTTP_IMAGE_CONTROL_H__




/**
 * Image control that can load it's content by using HTTP requests.
 * While content is not yet loaded, this control shows blank content.
 * This control can also preserv its aspect rate to make sure image
 * is always displayed with proper proportions.
 */
class HTTPImageControl : public gameplay::ImageControl
{
public:
    /**
     * Create an imaget control with a given style and properties.
     *
     * @param style The style to apply to this tab.
     * @param properties A properties object containing a definition of the tab (optional).
     *
     * @return The new tab.
     */
    static gameplay::Control* create(gameplay::Theme::Style* style, gameplay::Properties* properties = NULL);

    /**
     * Set the path of the image for this ImageControl to display.
     * File can't be opened using path argument, it is then interpreted
     * as URL and is tried to be downloaded using HTTP request.
     * The file is downloaded and stored in the temporary folder and 
     * then opened as usual file.
     *
     * @param path The path to the image or URL.
     */
    void setImage(const char* path) override;

    /**
     * Set to automatically preserve aspect ration when one of 
     * width or height are set relative to a parent, but not both.
     */
    void setPreserveAspect(bool set);

    /**
     * Get whether aspect ration is preserved or not.
     */
    bool getPreserveAspect() const { return _preserveAspect; };

protected:
    /**
     * Constructor.
     */
    HTTPImageControl();

    /**
     * Destructor.
     */
    virtual ~HTTPImageControl();

    /**
     * Extends ScriptTarget::getTypeName() to return the type name of this class.
     *
     * Child controls should override this function to return the correct type name.
     *
     * @return The type name of this class: "HTTPImageControl"
     * @see ScriptTarget::getTypeName()
     */
    const char* getTypeName() const;

    /**
     * @see Control::initialize
     */
    void initialize(const char* typeName, gameplay::Theme::Style* style, gameplay::Properties* properties);

    /**
     * @see Control::drawImages
     */
    unsigned int drawImages(gameplay::Form* form) const override;

    /**
     * @see Control::updateBounds
     */
    void updateBounds();

private:

    void imageDownloadedCallback(int curlCode, const std::vector<uint8_t>& response, const std::string& path);

    class HTTPRequestService * _httpRequestService;
    bool _preserveAspect;
};



#endif // __HTTP_IMAGE_CONTROL_H__
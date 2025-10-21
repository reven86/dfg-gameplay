#include "pch.h"
#include "clip_label.h"



/**
 * Clip text to bounds inserting '...' if text is too long.
 * Works with single line text.
 *
 * @param text Input text.
 * @param width Width to clip text by.
 * @param font Font.
 * @param fontSize Size of the font.
 * @param characterSpacing Additional spacing between character, in pixels.
 */
std::wstring clipTextToBounds(const wchar_t* text, float width, const gameplay::Font* font, float fontSize, float characterSpacing)
{
    std::wstring result;

    if (width <= 0.0f)
        return L"";

    float textw = 0, texth = 0;
    font->measureText(text, fontSize, gameplay::Font::LEFT_TO_RIGHT, &textw, &texth, characterSpacing);

    if (textw < width)
        return text;

    result = text;

    result.erase(result.end() - 1, result.end());
    result.push_back('.');
    result.push_back('.');
    result.push_back('.');
    do
    {
        result.erase(result.end() - 4, result.end());
        result.push_back('.');
        result.push_back('.');
        result.push_back('.');
        font->measureText(result.c_str(), fontSize, gameplay::Font::LEFT_TO_RIGHT, &textw, &texth, characterSpacing);
    } while (result.size() > 3 && textw >= width);

    return result;
}

/**
 * Clip text to bounds inserting '...' if text is too long.
 * This is multiline version of previous clipTextToBounds function.
 *
 * @param text Input text.
 * @param width Width to clip text by.
 * @param height Height to clip text by.
 * @param font Font.
 * @param fontSize Size of the font.
 * @param characterSpacing Additional spacing between character, in pixels.
 * @param line Additional spacing between lines, in pixels.
 */
std::wstring clipTextToBounds(const wchar_t* text, float width, float height, const gameplay::Font* font, float fontSize,
    float characterSpacing, float lineSpacing)
{
    std::wstring result;

    if (width <= 0.0f || height <= 0.0f)
        return L"";

    if (height < fontSize)
        height = fontSize;

    gameplay::Rectangle clip(width, height);
    gameplay::Rectangle out;

    font->measureText(text, clip, fontSize, gameplay::Font::LEFT_TO_RIGHT, &out, gameplay::Font::ALIGN_TOP_LEFT, true, true, characterSpacing, lineSpacing);

    if (out.width <= width && out.height <= height)
        return text;

    result = text;

    result.erase(result.end() - 1, result.end());
    result.push_back('.');
    result.push_back('.');
    result.push_back('.');
    do
    {
        result.erase(result.end() - 4, result.end());
        result.push_back('.');
        result.push_back('.');
        result.push_back('.');
        font->measureText(result.c_str(), clip, fontSize, gameplay::Font::LEFT_TO_RIGHT, &out, gameplay::Font::ALIGN_TOP_LEFT, true, true, characterSpacing, lineSpacing);
    } while (result.size() > 3 && (out.width > width || out.height > height));

    return result;
}



ClipLabel::ClipLabel()
    : _clippingActive(true)
{
}

ClipLabel::~ClipLabel()
{
}

const char * ClipLabel::getTypeName() const
{
    return "ClipLabel";
}

ClipLabel* ClipLabel::create(const char* id, gameplay::Theme::Style* style)
{
    ClipLabel * label = new ClipLabel();
    label->_id = id ? id : "";
    label->initialize(label->getTypeName(), style, NULL);
    return label;
}

gameplay::Control * ClipLabel::create(gameplay::Theme::Style * style, gameplay::Properties * properties)
{
    ClipLabel * res = new ClipLabel();
    res->initialize(res->getTypeName(), style, properties);
    return res;
}

void ClipLabel::initialize(const char * typeName, gameplay::Theme::Style * style, gameplay::Properties * properties)
{
    gameplay::Label::initialize(typeName, style, properties);

    if (properties)
    {
    }
}

void ClipLabel::updateAbsoluteBounds(const gameplay::Vector2& offset)
{
    gameplay::Rectangle oldBounds(_textBounds);
    gameplay::Label::updateAbsoluteBounds(offset);

    if (oldBounds.width != _textBounds.width || oldBounds.height != _textBounds.height)
        clipText();
}

void ClipLabel::setText(const wchar_t * text)
{
    std::wstring oldText(_text);
    gameplay::Label::setText(text);

    if (_text != oldText)
        clipText();
}

void ClipLabel::updateState(State state)
{
    gameplay::Font * oldFont(_font);
    gameplay::Label::updateState(state);
    if (oldFont != _font)
        clipText();
}

void ClipLabel::clipText()
{
    if (!_font || !_clippingActive)
    {
        _clippedText = _text;
        return;
    }

    gameplay::Control::State state = getState();

    _clippedText = clipTextToBounds(_text.c_str(), _textBounds.width, _textBounds.height, _font, getFontSize(state), getCharacterSpacing(state), getLineSpacing(state));
}

unsigned int ClipLabel::drawText(gameplay::Form * form) const
{
    if (_viewportClipBounds.width <= 0 || _viewportClipBounds.height <= 0)
        return 0;

    // Draw the text.
    if (_clippedText.size() > 0 && _font)
    {
        Control::State state = getState();
        float fontSize = getFontSize(state);

        gameplay::SpriteBatch* batch = _font->getSpriteBatch(fontSize);
        startBatch(form, batch);
        _font->drawText(_clippedText.c_str(), _textBounds, _textColor, fontSize, getTextAlignment(state), true, getTextDrawingFlags(state), _viewportClipBounds,
            getCharacterSpacing(state), getLineSpacing(state));
        finishBatch(form, batch);

        return 1;
    }

    return 0;
}

void ClipLabel::enableClipping(bool enable)
{
    _clippingActive = enable;
}
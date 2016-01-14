#include "pch.h"
#include "clip_label.h"




ClipLabel::ClipLabel()
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

    if (oldBounds != _textBounds)
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
    _clippedText = _font ? Utils::clipTextToBounds(_text.c_str(), _textBounds.width, _textBounds.height, _font, getFontSize(getState())) : _text;
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
        _font->drawText(_clippedText.c_str(), _textBounds, _textColor, fontSize, getTextAlignment(state), true, getTextDrawingFlags(state), _viewportClipBounds);
        finishBatch(form, batch);

        return 1;
    }

    return 0;
}
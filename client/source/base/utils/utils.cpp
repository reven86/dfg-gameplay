#include "pch.h"
#include "utils.h"
#include "ui/dial_button.h"
#include "ui/expanded_tab.h"
#include "zlib.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <iomanip>

#define UUID_SYSTEM_GENERATOR 
#include "uuid.h"





std::string Utils::generateUUID( )
{
    return uuids::to_string(uuids::uuid_system_generator{}());
}




std::wstring Utils::clipTextToBounds(const wchar_t * text, float width, const gameplay::Font * font, float fontSize, float characterSpacing)
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

std::wstring Utils::clipTextToBounds(const wchar_t * text, float width, float height, const gameplay::Font * font, float fontSize,
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


bool Utils::serializeString(gameplay::Stream * stream, const std::string& str)
{
    int32_t size = static_cast<int32_t>(str.size());
    if (stream->write(&size, sizeof(size), 1) != 1)
        return false;
    if (stream->write(str.c_str(), sizeof(char), size) != size)
        return false;
    return true;
}

bool Utils::deserializeString(gameplay::Stream * stream, std::string * str)
{
    int32_t size = 0;
    if (stream->read(&size, sizeof(size), 1) != 1)
        return false;

    if (size <= 0)
        return false;

    std::unique_ptr<char[]> buf(new char[size]);
    if (stream->read(buf.get(), 1, size) != size)
        return false;

    str->assign(buf.get(), size);
    return true;
}

void Utils::scaleUIControl(gameplay::Control * control, float kx, float ky)
{
    if (!control)
        return;

    // the actual scaling
    if (!control->isXPercentage())
        control->setX(roundf(control->getX() * kx));
    if (!control->isYPercentage())
        control->setY(roundf(control->getY() * ky));
    if (!control->isWidthPercentage() && (control->getAutoSize() & gameplay::Control::AUTO_SIZE_WIDTH) == 0)
        control->setWidth(roundf(control->getWidth() * kx));
    if (!control->isHeightPercentage() && (control->getAutoSize() & gameplay::Control::AUTO_SIZE_HEIGHT) == 0)
        control->setHeight(roundf(control->getHeight() * ky));

    const gameplay::Theme::Border& border = control->getBorder();
    const gameplay::Theme::Margin& margin = control->getMargin();
    const gameplay::Theme::Padding& padding = control->getPadding();
    control->setBorder(roundf(border.top * ky), roundf(border.bottom * ky), roundf(border.left * kx), roundf(border.right * kx));
    control->setMargin(roundf(margin.top * ky), roundf(margin.bottom * ky), roundf(margin.left * kx), roundf(margin.right * kx));
    control->setPadding(roundf(padding.top * ky), roundf(padding.bottom * ky), roundf(padding.left * kx), roundf(padding.right * kx));

    control->setFontSize(roundf(ky * control->getFontSize()));
    control->setCharacterSpacing(roundf(ky * control->getCharacterSpacing()));
    control->setLineSpacing(roundf(ky * control->getLineSpacing()));

    if (strcmp(control->getTypeName(), "Slider") == 0 || strcmp(control->getTypeName(), "Timeline") == 0)   // hack
        static_cast<gameplay::Slider *>(control)->setScaleFactor(ky * static_cast<gameplay::Slider *>(control)->getScaleFactor());

    if (strcmp(control->getTypeName(), "ImageControl") == 0)
    {
        gameplay::ImageControl * image = static_cast< gameplay::ImageControl * >(control);
        const gameplay::Rectangle& dstRegion = image->getRegionDst();
        image->setRegionDst(dstRegion.x * kx, dstRegion.y * ky, dstRegion.width * kx, dstRegion.height * ky);
    }
    
    if (strcmp(control->getTypeName(), "DialButton") == 0)
    {
        DialButton * button = static_cast<DialButton *>(control);
        button->setHeightCollapsed(ky * button->getHeightCollapsed());
        button->setHeightExpanded(ky * button->getHeightExpanded());
    }
    
    if (strcmp(control->getTypeName(), "ExpandedTab") == 0)
    {
        ExpandedTab * tab = static_cast<ExpandedTab * >(control);
        tab->setWidthMinimized(kx * tab->getWidthMinimized());
        tab->setWidthMaximized(kx * tab->getWidthMaximized());
    }

    if (strcmp(control->getTypeName(), "RadioButton") == 0)
    {
        gameplay::RadioButton * button = static_cast<gameplay::RadioButton *>(control);
        button->setIconScale(button->getIconScale() * ky);
    }

    if (strcmp(control->getTypeName(), "CheckBox") == 0)
    {
        gameplay::CheckBox * button = static_cast<gameplay::CheckBox *>(control);
        button->setIconScale(button->getIconScale() * ky);
    }

    if (control->isContainer())
    {
        gameplay::Container * container = static_cast<gameplay::Container *>(control);
        container->setScrollScale(container->getScrollScale() * ky);

        const gameplay::Vector2& scrollPos = container->getScrollPosition();
        container->setScrollPosition(gameplay::Vector2(scrollPos.x * kx, scrollPos.y * ky));

        gameplay::Layout::Type layoutType = container->getLayout()->getType();
        if (layoutType == gameplay::Layout::LAYOUT_FLOW)
        {
            float horizontalSpacing = static_cast<gameplay::FlowLayout *>(container->getLayout())->getHorizontalSpacing();
            float verticalSpacing = static_cast<gameplay::FlowLayout *>(container->getLayout())->getVerticalSpacing();
            static_cast<gameplay::FlowLayout *>(container->getLayout())->setSpacing(horizontalSpacing * kx, verticalSpacing * ky);
        }
        else if (layoutType == gameplay::Layout::LAYOUT_HORIZONTAL)
        {
            float spacing = static_cast<gameplay::HorizontalLayout *>(container->getLayout())->getSpacing();
            static_cast<gameplay::HorizontalLayout *>(container->getLayout())->setSpacing(spacing * kx);
        }
        else if (layoutType == gameplay::Layout::LAYOUT_VERTICAL)
        {
            float spacing = static_cast<gameplay::VerticalLayout *>(container->getLayout())->getSpacing();
            static_cast<gameplay::VerticalLayout *>(container->getLayout())->setSpacing(spacing * ky);
        }

        const std::vector< gameplay::Control * >& children = container->getControls();
        for (unsigned j = 0; j < children.size(); j++)
            scaleUIControl(children[j], kx, ky);
    }
}


void Utils::measureChildrenBounds(gameplay::Container * container, float * width, float * height)
{
    // Calculate total width and height.
    float totalWidth = 0.0f, totalHeight = 0.0f;
    const std::vector<gameplay::Control*>& controls = container->getControls();
    for (size_t i = 0, count = controls.size(); i < count; ++i)
    {
        gameplay::Control* control = controls[i];

        if (!control->isVisible())
            continue;

        const gameplay::Rectangle& bounds = control->getBounds();
        const gameplay::Theme::Margin& margin = control->getMargin();

        float newWidth = bounds.x + bounds.width + margin.right;
        if (newWidth > totalWidth)
            totalWidth = newWidth;

        float newHeight = bounds.y + bounds.height + margin.bottom;
        if (newHeight > totalHeight)
            totalHeight = newHeight;
    }

    if (width)
        *width = totalWidth;
    if (height)
        *height = totalHeight;
}



unsigned long Utils::compressToStream(const void * data, size_t dataLength, gameplay::Stream * stream, void * tmpBuf, size_t tmpBufSize)
{
    z_stream defstream;

    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    defstream.avail_in = dataLength;
    defstream.next_in = (Bytef *)data;
    deflateInit(&defstream, Z_BEST_COMPRESSION);

    do
    {
        defstream.avail_out = tmpBufSize;
        defstream.next_out = (Bytef *)tmpBuf;

        deflate(&defstream, Z_FINISH);

        if (stream)
            stream->write(tmpBuf, tmpBufSize - defstream.avail_out, 1);
    } while (defstream.avail_out == 0);

    deflateEnd(&defstream);

    return defstream.total_out;
}


// Function to calculate HMAC-SHA256
std::string Utils::calculateHMAC_SHA256(const std::string& key, const std::string& data) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    // Calculate HMAC
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key.c_str(), key.length(), EVP_sha256(), nullptr);
    HMAC_Update(ctx, reinterpret_cast<const unsigned char*>(data.c_str()), data.length());
    HMAC_Final(ctx, hash, &hash_len);
    HMAC_CTX_free(ctx);

    // Convert the hash to a hexadecimal string
    std::stringstream ss;
    for (unsigned int i = 0; i < hash_len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}


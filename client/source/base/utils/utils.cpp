#include "pch.h"
#include "utils.h"
#include "ui/dial_button.h"
#include "ui/expanded_tab.h"
#include "utf8.h"
#include <openssl/md5.h>

#if defined (WIN32)
#include <Rpc.h>
#elif defined (__ANDROID__) || defined (__EMSCRIPTEN__)
#include <uuidlib/uuid.h>
#else
#include <uuid/uuid.h>
#endif





std::string Utils::generateUUID( )
{
    std::string s;

#ifdef WIN32
    UUID uuid;
    UuidCreate ( &uuid );

    unsigned char * str;
    UuidToStringA ( &uuid, &str );

    s = ( const char* ) str;

    RpcStringFreeA ( &str );
#else
    uuid_t uuid;
    uuid_generate_random ( uuid );
    char str[37];
    uuid_unparse ( uuid, str );

    s = str;
#endif

    return s;
}




const wchar_t * Utils::UTF8ToWCS(const char * str)
{
    GP_ASSERT(str);

    static std::wstring res;
    res.clear();
    utf8::unchecked::utf8to16(str, str + strlen(str), std::back_inserter(res));
    return res.c_str();
}



const char * Utils::WCSToUTF8(const wchar_t * str)
{
    GP_ASSERT(str);

    static std::string res;
    res.clear();
    utf8::unchecked::utf16to8(str, str + wcslen(str), std::back_inserter(res));
    return res.c_str();
}




const wchar_t * Utils::ANSIToWCS(const char * str)
{
    static wchar_t result[2048];

    wchar_t * o = result;
    while( *str )
        *o++ = *str++;
    *o = 0;

    return result;
}




const char * Utils::format(const char * fmt, ...)
{
    static char results[16][2048];
    static int resInd = 0;

    char * result = results[resInd];
    resInd = (resInd + 1) & 15;

    va_list args;
    va_start(args, fmt);

#ifdef WIN32
    _vsnprintf(result, 2048, fmt, args);
#else
    vsnprintf(result, 2048, fmt, args);
#endif

    va_end(args);

    return result;
}


const char * Utils::urlEncode(const char * src)
{
    static std::string res[16];
    static int resInd = 0;

    std::string& result = res[resInd];
    resInd = (resInd + 1) & 15;

    result.clear();

    static char hexmap[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    size_t max = strlen(src);
    for (int i = 0; i < max; i++, src++)
    {
        if (('0' <= *src && *src <= '9') ||
            ('A' <= *src && *src <= 'Z') ||
            ('a' <= *src && *src <= 'z') ||
            (*src == '~' || *src == '-' || *src == '_' || *src == '.')
            )
        {
            result.push_back(*src);
        }
        else
        {
            result.push_back('%');
            result.push_back(hexmap[(unsigned char)(*src) >> 4]);
            result.push_back(hexmap[*src & 0x0F]);
        }
    }

    return result.c_str();
}



const wchar_t * Utils::clipTextToBounds(const wchar_t * text, float width, const gameplay::Font * font, float fontSize, float characterSpacing)
{
    static std::wstring result;

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

    return result.c_str();
}

const wchar_t * Utils::clipTextToBounds(const wchar_t * text, float width, float height, const gameplay::Font * font, float fontSize,
    float characterSpacing, float lineSpacing)
{
    static std::wstring result;

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

    return result.c_str();
}


void Utils::serializeString(gameplay::Stream * stream, const std::string& str)
{
    int32_t size = static_cast<int32_t>(str.size());
    stream->write(&size, sizeof(size), 1);
    stream->write(str.c_str(), sizeof(char), size);
}

void Utils::deserializeString(gameplay::Stream * stream, std::string * str)
{
    int32_t size = 0;
    if (stream->read(&size, sizeof(size), 1) != 1)
        return;

    if (size < 0 || size > 65535)
        return; // something wrong with data

    char * buf = reinterpret_cast<char *>(alloca(sizeof(char)* (size + 1)));
    if (buf)
    {
        stream->read(buf, sizeof(char), size);
        buf[size] = '\0';
        if (str)
        {
            str->clear();
            *str = buf;
        }
    }
}

void Utils::scaleUIControl(gameplay::Control * control, float kx, float ky)
{
    if (!control)
        return;

    // the actual scaling
    if (!control->isXPercentage())
        control->setX(control->getX() * kx);
    if (!control->isYPercentage())
        control->setY(control->getY() * ky);
    if (!control->isWidthPercentage() && (control->getAutoSize() & gameplay::Control::AUTO_SIZE_WIDTH) == 0)
        control->setWidth(control->getWidth() * kx);
    if (!control->isHeightPercentage() && (control->getAutoSize() & gameplay::Control::AUTO_SIZE_HEIGHT) == 0)
        control->setHeight(control->getHeight() * ky);

    const gameplay::Theme::Border& border = control->getBorder();
    const gameplay::Theme::Margin& margin = control->getMargin();
    const gameplay::Theme::Padding& padding = control->getPadding();
    control->setBorder(border.top * ky, border.bottom * ky, border.left * kx, border.right * kx);
    control->setMargin(margin.top * ky, margin.bottom * ky, margin.left * kx, margin.right * kx);
    control->setPadding(padding.top * ky, padding.bottom * ky, padding.left * kx, padding.right * kx);

    control->setFontSize(roundf(ky * control->getFontSize()));
    control->setCharacterSpacing(ky * control->getCharacterSpacing());
    control->setLineSpacing(ky * control->getLineSpacing());

    if (strcmp(control->getTypeName(), "Slider") == 0)
        static_cast<gameplay::Slider *>(control)->setScaleFactor(ky);

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



float Utils::luminosity(const gameplay::Vector4& color)
{
    return 0.3f * color.x + 0.59f * color.y + 0.11f * color.z;
}

float HueToRGB(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

gameplay::Vector4 Utils::HSLToRGB(const gameplay::Vector4& hsl)
{
    if (hsl.y == 0.0f)
        return gameplay::Vector4(1.0f, 1.0f, 1.0f, hsl.w);

    float q = hsl.z < 0.5f ? hsl.z * (1.0f + hsl.y) : hsl.z + hsl.y - hsl.z * hsl.y;
    float p = 2 * hsl.z - q;
    return gameplay::Vector4(HueToRGB(p, q, hsl.x + 1.0f / 3.0f), HueToRGB(p, q, hsl.x), HueToRGB(p, q, hsl.x - 1.0f / 3.0f), 1.0f);
}

gameplay::Vector4 Utils::RGBToHSL(const gameplay::Vector4& rgb)
{
    float max = std::max(std::max(rgb.x, rgb.y), rgb.z);
    float min = std::min(std::min(rgb.x, rgb.y), rgb.z);
    float h, s, l = (max + min) / 2;

    if (max == min)
        return gameplay::Vector4(0.0f, 0.0f, 0.0f, rgb.w);

    float d = max - min;
    s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
    if (max == rgb.x)
        h = (rgb.y - rgb.z) / d + (rgb.y < rgb.z ? 6.0f : 0.0f);
    else if (max == rgb.y)
        h = (rgb.z - rgb.x) / d + 2.0f;
    else
        h = (rgb.x - rgb.y) / d + 4.0f;
    h *= 1.0f / 6.0f;

    return gameplay::Vector4(h, s, l, 1.0f);
}



void Utils::base64Encode(const uint8_t * in, size_t len, std::string * out)
{
    if (!out)
        return;

    int val = 0, valb = -6;
    for (const uint8_t * c = in; c < in + len; c++)
    {
        val = (val << 8) + *c;
        valb += 8;
        while (valb >= 0)
        {
            out->push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        out->push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out->size() % 4)
        out->push_back('=');
}

void Utils::base64Decode(const std::string &in, std::vector<uint8_t> * out)
{
    if (!out)
        return;

    std::vector<int> T(256, -1);
    for (int i = 0; i<64; i++) 
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

    int val = 0, valb = -8;
    for (uint8_t c : in)
    {
        if (T[c] == -1)
            break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0)
        {
            out->push_back(uint8_t((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
}

void Utils::MD5(const void* data, size_t length, unsigned char outDigest[16])
{
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, data, length);
    MD5_Final(outDigest, &context);
}

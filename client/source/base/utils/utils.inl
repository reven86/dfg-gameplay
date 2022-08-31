#include "utils.h"
#include "utf8.h"
#include <openssl/md5.h>



inline std::wstring Utils::UTF8ToWCS(const std::string& str)
{
    std::wstring res;
    utf8::unchecked::utf8to16(str.begin(), str.end(), std::back_inserter(res));
    return res;
}



inline std::string Utils::WCSToUTF8(const std::wstring& str)
{
    std::string res;
    utf8::unchecked::utf16to8(str.begin(), str.end(), std::back_inserter(res));
    return res;
}




inline std::wstring Utils::ANSIToWCS(const std::string& str)
{
    return std::wstring(str.begin(), str.end());
}




inline std::string Utils::format(const char * fmt, ...)
{
    char result[2048];
    
    va_list args;
    va_start(args, fmt);

#ifdef WIN32
    _vsnprintf(result, 2048, fmt, args);
#else
    vsnprintf(result, 2048, fmt, args);
#endif

    va_end(args);

    return std::string(result);
}


inline std::wstring Utils::format(const wchar_t * fmt, ...)
{
    wchar_t result[2048];

    va_list args;
    va_start(args, fmt);

#ifdef WIN32
    _vsnwprintf(result, 2048, fmt, args);
#else
    vswprintf(result, 2048, fmt, args);
#endif

    va_end(args);

    return std::wstring(result);
}


inline std::string Utils::urlEncode(const std::string& source)
{
	if (source.empty())
		return std::string();

    std::string result;

    static char hexmap[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    size_t max = source.size();
    const char * src = &(*source.begin());
    for (size_t i = 0; i < max; i++, src++)
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

    return result;
}


inline std::string Utils::urlDecode(const std::string& source)
{
    if (source.empty())
        return std::string();

    std::string result;

    const char * src = source.c_str();
    const char * srcEnd = src + source.length();
    while (src < srcEnd - 2)
    {
        if (*src == '%')
        {
            uint8_t hi = std::tolower(*(src + 1));
            uint8_t lo = std::tolower(*(src + 2));

            if (std::isdigit(hi))
                hi -= '0';
            else
                hi = hi - 'a' + 10;

            if (std::isdigit(lo))
                lo -= '0';
            else
                lo = lo - 'a' + 10;

            GP_ASSERT(hi < 16 && lo < 16);

            result.push_back((hi << 4) + lo);
            src += 3;
            continue;
        }
        else if (*src == '+')
        {
            result.push_back(' ');
            src++;
            continue;
        }

        result.push_back(*src++);
    }

    while (src < srcEnd)
        result.push_back(*src++);

    return result;
}


inline float Utils::luminosity(const gameplay::Vector4& color)
{
    return 0.3f * color.x + 0.59f * color.y + 0.11f * color.z;
}

inline float HueToRGB(float p, float q, float t)
{
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

inline gameplay::Vector4 Utils::HSLToRGB(const gameplay::Vector4& hsl)
{
    if (hsl.y == 0.0f)
        return gameplay::Vector4(1.0f, 1.0f, 1.0f, hsl.w);

    float q = hsl.z < 0.5f ? hsl.z * (1.0f + hsl.y) : hsl.z + hsl.y - hsl.z * hsl.y;
    float p = 2 * hsl.z - q;
    return gameplay::Vector4(HueToRGB(p, q, hsl.x + 1.0f / 3.0f), HueToRGB(p, q, hsl.x), HueToRGB(p, q, hsl.x - 1.0f / 3.0f), 1.0f);
}

inline gameplay::Vector4 Utils::RGBToHSL(const gameplay::Vector4& rgb)
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


inline void Utils::base64Encode(const void * in, size_t len, std::string * out, bool urlsafe)
{
    if (!out)
        return;

    const char * alphabet = urlsafe ? "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_" : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    int val = 0, valb = -6;
    const uint8_t * inbuf = static_cast<const uint8_t *>(in);
    for (const uint8_t * c = inbuf; c < inbuf + len; c++)
    {
        val = (val << 8) + *c;
        valb += 8;
        while (valb >= 0)
        {
            out->push_back(alphabet[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        out->push_back(alphabet[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out->size() % 4)
        out->push_back('=');
}

inline void Utils::base64Decode(const std::string &in, std::vector<uint8_t> * out, bool urlsafe)
{
    if (!out)
        return;

    const char * alphabet = urlsafe ? "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_" : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::vector<int> T(256, -1);
    for (int i = 0; i<64; i++)
        T[alphabet[i]] = i;

    int val = 0, valb = -8;
    for (uint8_t c : in)
    {
        if (c == '\n')
            continue;
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

inline void Utils::MD5(const void* data, size_t length, unsigned char outDigest[16])
{
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, data, length);
    MD5_Final(outDigest, &context);
}


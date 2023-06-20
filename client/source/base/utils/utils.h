#ifndef __DFG_UTILS__
#define __DFG_UTILS__



struct interruptable_accumulator
{
    typedef bool result_type;
    template<typename T_iterator>
    result_type operator()(T_iterator first, T_iterator last) const
    {
        for (; first != last; ++first)
            if (!*first)
                return false;
        return true;
    }
};



struct Utils
{


/** @brief Simple lerp functor.
 *
 *	@author Andrew "RevEn" Karpushin
 */

template< class _Type >
struct LinearInterpolator
{
	_Type interpolate( const _Type& a, const _Type& b, const float& t ) { return static_cast< _Type >( a + ( b - a ) * t ); };
};


/**
 * Hermite spline interpolation.
 */
template< class _Type >
static _Type hermiteSpline( const _Type& v0, const _Type& t0, const _Type& v1, const _Type& t1, float t )
{
    float t2 = t * t;
    float t3 = t * t2;
    return ( 2.0f * t3 - 3.0f * t2 + 1 ) * v0 + ( t3 - 2.0f * t2 + t ) * t0 + ( 3.0f * t2 - 2.0f * t3 ) * v1 + ( t3 - t2 ) * t1;
};


/**
 * Generates new UUID.
 */
static std::string generateUUID();




/**
 * Convert UTF8 string to wchar_t one.
 */
static inline std::wstring UTF8ToWCS(const std::string& str);


/**
 * Convert wide-char string to UTF8 one.
 */
static inline std::string WCSToUTF8(const std::wstring& str);


/**
 * Function to map char * string to wchar_t * one. Works ONLY for ANSI characters. Max 2048 chars.
 */
static inline std::wstring ANSIToWCS(const std::string& str);


/**
 * Wrapper around sprintf. Result string length is limited to 2048 characters.
 */
static inline std::string format(const char * fmt, ...);
static inline std::wstring format(const wchar_t * fmt, ...);


/**
 * UrlEncode string.
 */
static inline std::string urlEncode(const std::string& src);


/**
 * UrlDecode string.
 */
static inline std::string urlDecode(const std::string& src);


/**
 * Encode binary data to base64 string.
 */
static inline void base64Encode(const void * in, size_t len, std::string * out, bool urlsafe = false);

/**
 * Decode base64 string to binary data.
 */
static inline void base64Decode(const std::string& in, std::vector<uint8_t> * out, bool urlsafe = false);

/**
 * Verify the signature using public key and algorithm.
 * 
 * @param algorithm Hashing algorithm SHA1, SHA256, etc.
 * @param data Pointer to the data.
 * @param dataLength Length of the data.
 * @param base64Signature Signature encoded in base64 encoding.
 * @param base64PublicKey Public key encoded in base64 encoding.
 * @return True if signature is valid.
 */
static inline bool verifySignature(const char * algorithm, const void * data, size_t dataLength, const std::string& base64Signature, const std::string& base64PublicKey);


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
static std::wstring clipTextToBounds(const wchar_t * text, float width, const gameplay::Font * font, float fontSize, 
    float characterSpacing = 0.0f);

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
static std::wstring clipTextToBounds(const wchar_t * text, float width, float height, const gameplay::Font * font, float fontSize, 
    float characterSpacing = 0.0f, float lineSpacing = 0.0f);



/**
 * Serialize string to stream.
 */
static bool serializeString(gameplay::Stream * stream, const std::string& str);

/**
 * Deserialize string from stream.
 */
static bool deserializeString(gameplay::Stream * stream, std::string * str);



/**
 * Recursively scale gameplay::Control and all its children by some factors.
 * The margin, padding and border are scaled as well.
 */
static void scaleUIControl(gameplay::Control * control, float kx, float ky);



/**
 * Calculate total width and height of all visible children in the container.
 * This matches scrollable width/height.
 */
static void measureChildrenBounds(gameplay::Container * container, float * width, float * height);



/**
 * Get luminosity level of color.
 *
 * @param color Incoming color. Alpha component is not used.
 * @return Luminosity level, from 0 to 1;
 */
static inline float luminosity(const gameplay::Vector4& color);


/**
 * Convert Hue, Saturation, Luminosity to Red, Green, Blue. Alpha component stays the same.
 *
 * @param hsl Color in HSL format. Note: All components are in rage [0, 1].
 * @return Color in RGB format.
 * @see RGBToHSL
 */
static inline gameplay::Vector4 HSLToRGB(const gameplay::Vector4& hsl);


/**
 * Convert Red, Green, Blue to Hue, Saturation, Luminosity. Alpha component stays the same.
 *
 * @param rgb Color in RGB format.
 * @return Color in HSL format.  Note: All components are in rage [0, 1].
 * @see HSLToRGB
 */
static inline gameplay::Vector4 RGBToHSL(const gameplay::Vector4& rgb);



/**
 * Computes MD5 hash for data.
 *
 * @param data Input data buffer.
 * @param length Length of the buffer.
 * @param outDigest Buffer to save digest to.
 */
static inline void MD5(const void* data, size_t length, unsigned char outDigest[16]);



/**
 * Compress some data using zlib to a stream.
 * 
 * @param data Input buffer.
 * @param dataLength Length of input buffer.
 * @param stream Stream to store result.
 * @param tmpBuf Temporary intermediate buffer to store compressed data.
 * @param tmpBufSize Size of temporary buffer.
 * 
 * @return Number of bytes written.
 */
static unsigned long compressToStream(const void * data, size_t dataLength, gameplay::Stream * stream, void * tmpBuf, size_t tmpBufSize);


};

#include "utils.inl"

#endif // __DFG_UTILS__
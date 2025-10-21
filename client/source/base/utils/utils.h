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



namespace Utils
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
std::string generateUUID();




/**
 * Convert UTF8 string to wchar_t one.
 */
inline std::wstring UTF8ToWCS(const std::string& str);


/**
 * Convert wide-char string to UTF8 one.
 */
inline std::string WCSToUTF8(const std::wstring& str);


/**
 * Function to map char * string to wchar_t * one. Works ONLY for ANSI characters. Max 2048 chars.
 */
inline std::wstring ANSIToWCS(const std::string& str);


/**
 * Wrapper around sprintf. Result string length is limited to 2048 characters.
 */
inline std::string format(const char * fmt, ...);
inline std::wstring format(const wchar_t * fmt, ...);


/**
 * UrlEncode string.
 */
inline std::string urlEncode(const std::string& src);


/**
 * UrlDecode string.
 */
inline std::string urlDecode(const std::string& src);


/**
 * Encode binary data to base64 string.
 */
inline void base64Encode(const void * in, size_t len, std::string * out, bool urlsafe = false);

/**
 * Decode base64 string to binary data.
 */
inline void base64Decode(const std::string& in, std::vector<uint8_t> * out, bool urlsafe = false);

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
inline bool verifySignature(const char * algorithm, const void * data, size_t dataLength, const std::string& base64Signature, const std::string& base64PublicKey);




/**
 * Serialize string to stream.
 */
bool serializeString(gameplay::Stream * stream, const std::string& str);

/**
 * Deserialize string from stream.
 */
bool deserializeString(gameplay::Stream * stream, std::string * str);



/**
 * Get luminosity level of color.
 *
 * @param color Incoming color. Alpha component is not used.
 * @return Luminosity level, from 0 to 1;
 */
inline float luminosity(const gameplay::Vector4& color);


/**
 * Convert Hue, Saturation, Luminosity to Red, Green, Blue. Alpha component stays the same.
 *
 * @param hsl Color in HSL format. Note: All components are in rage [0, 1].
 * @return Color in RGB format.
 * @see RGBToHSL
 */
inline gameplay::Vector4 HSLToRGB(const gameplay::Vector4& hsl);


/**
 * Convert Red, Green, Blue to Hue, Saturation, Luminosity. Alpha component stays the same.
 *
 * @param rgb Color in RGB format.
 * @return Color in HSL format.  Note: All components are in rage [0, 1].
 * @see HSLToRGB
 */
inline gameplay::Vector4 RGBToHSL(const gameplay::Vector4& rgb);



/**
 * Computes MD5 hash for data.
 *
 * @param data Input data buffer.
 * @param length Length of the buffer.
 * @param outDigest Buffer to save digest to.
 */
inline void MD5(const void* data, size_t length, unsigned char outDigest[16]);



/**
 * Computes SHA256 hash for data.
 *
 * @param data Input data buffer.
 * @param length Length of the buffer.
 * @param outDigest Buffer to save digest to.
 */
inline void SHA256(const void* data, size_t length, unsigned char outDigest[32]);

// Function to calculate HMAC-SHA256
std::string calculateHMAC_SHA256(const std::string& key, const std::string& data);


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
unsigned long compressToStream(const void * data, size_t dataLength, gameplay::Stream * stream, void * tmpBuf, size_t tmpBufSize);


};

#include "utils.inl"

#endif // __DFG_UTILS__
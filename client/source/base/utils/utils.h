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
static const wchar_t * UTF8ToWCS(const char * str);


/**
 * Convert wide-char string to UTF8 one.
 */
static const char * WCSToUTF8(const wchar_t * str);


/**
 * Function to map char * string to wchar_t * one. Works ONLY for ANSI characters. Max 2048 chars.
 */
static const wchar_t * ANSIToWCS(const char * str);


/**
 * Wrapper around sprintf. Nested calls are not allowed. Max 2048 chars.
 */
static const char * format(const char * fmt, ...);


/**
 * UrlEncode string. Nested calls are not allowed.
 */
static const char * urlEncode(const char * src);


/**
 * Clip text to bounds inserting '...' if text is too long.
 * Works with single line text.
 *
 * @param text Input text.
 * @param width Width to clip text by.
 * @param font Font.
 * @param fontSize Size of the font;
 */
static const wchar_t * clipTextToBounds(const wchar_t * text, float width, const gameplay::Font * font, float fontSize);

/**
 * Clip text to bounds inserting '...' if text is too long.
 * This is multiline version of previous clipTextToBounds function.
 *
 * @param text Input text.
 * @param width Width to clip text by.
 * @param height Height to clip text by.
 * @param font Font.
 * @param fontSize Size of the font;
 */
static const wchar_t * clipTextToBounds(const wchar_t * text, float width, float height, const gameplay::Font * font, float fontSize);



/**
 * Serialize string to stream.
 */
static void serializeString(gameplay::Stream * stream, const std::string& str);

/**
 * Deserialize string from stream.
 */
static void deserializeString(gameplay::Stream * stream, std::string * str);



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


};


#endif // __DFG_UTILS__
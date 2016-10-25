#pragma once


#ifndef __DFG_VARIANT_H__
#define __DFG_VARIANT_H__


/**
 * Defines Variant data type that can hold arbitrary other types.
 * Compatible with DAVA Framework's VariantType.
 * https://github.com/dava/dava.engine/blob/development/Sources/Internal/FileSystem/VariantType.h
 */

class VariantType
{
public:
    enum Type : uint8_t
    {
        TYPE_NONE = 0,
        TYPE_BOOLEAN,
        TYPE_INT32,
        TYPE_FLOAT,
        TYPE_STRING,
        TYPE_WIDE_STRING,
        TYPE_BYTE_ARRAY,
        TYPE_UINT32,
        TYPE_KEYED_ARCHIVE, // not supported at the moment
        TYPE_INT64,
        TYPE_UINT64,
        TYPE_VECTOR2,
        TYPE_VECTOR3,
        TYPE_VECTOR4,
        TYPE_MATRIX2,       // not supported at the moment
        TYPE_MATRIX3,
        TYPE_MATRIX4,
        TYPE_COLOR,         // not supported at the moment
        TYPE_FASTNAME,      // not supported at the moment
        TYPE_AABBOX3,       // not supported at the moment
        TYPE_FILEPATH,      // not supported at the moment
        TYPE_FLOAT64,
        TYPE_INT8,
        TYPE_UINT8,
        TYPE_INT16,
        TYPE_UINT16,
        TYPES_COUNT // every new type should be always added to the end for compatibility with old archives
    };

    /**
     * Signals after the underlying type or the value has been changed.
     * Instance of this VariantType is passed as argument.
     */
    sigc::signal<void, const VariantType&> valueChangedSignal;

public:
    VariantType();
    ~VariantType();

    /**
     * Explicit conversion constructors.
     */
    template<class _Type> explicit VariantType(const _Type& value);

    /**
     * Assignment operator.
     */
    VariantType& operator=(const VariantType& other);

    /**
     * Get data type held by variant.
     */
    inline Type getType() const;

    /**
     * Get data type name held by variant.
     */
    inline const char * getTypeName() const;

    /**
     * Set the contents of variant to specified value.
     *
     * \param value Value, can be one of supported data types.
     */
    template<typename _Type> inline void set(const _Type& value);

    /**
     * Get the contents of variant as a given type.
     */
    template<typename _Type> inline const _Type& get() const;

    /**
     * Set contents of variant as a blob (byte array).
     *
     * \param data Data source.
     * \param size Data size.
     */
    void setBlob(const uint8_t * data, uint32_t size);

    /**
     * Get the contents of a variant as a byte array (blob).
     *
     * \param[out] size Receives blob's size.
     * \return Pointer to blob's first byte.
     */
    const uint8_t * getBlob(uint32_t * size) const;

    inline bool operator== (const VariantType& other) const;
    inline bool operator!= (const VariantType& other) const;

private:
    // This constructor is private to prevent creation of VariantType from pointer
    // Without this, creating VariantType from any pointer will be automatically casted to BOOL
    // by C++ compiler, that is completely wrong
    VariantType(void*);

    void release();



    union {
        bool boolValue;
        int8_t int8Value;
        uint8_t uint8Value;

        int16_t int16Value;
        uint16_t uint16Value;

        int32_t int32Value;
        uint32_t uint32Value;

        float floatValue;
        double float64Value;

        int64_t int64Value;
        uint64_t uint64Value;

        gameplay::Vector2 * vector2Value;
        gameplay::Vector3 * vector3Value;
        gameplay::Vector4 * vector4Value;

        gameplay::Matrix3 * matrix3Value;
        gameplay::Matrix * matrix4Value;

        void* pointerValue;

        std::string * stringValue;
        std::wstring * wideStringValue;
    };

    Type type;
};




#include "variant.inl"


#endif // __DFG_VARIANT_H__
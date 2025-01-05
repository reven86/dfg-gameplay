#pragma once



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
        TYPE_KEYED_ARCHIVE,
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

        TYPE_RECT,         // not supported at the moment
        TYPE_LIST,
        TYPE_QUATERNION,   // not supported at the moment
        TYPE_TRANSFORM,    // not supported at the moment
        TYPE_AABBOX2,      // not supported at the moment

        TYPES_COUNT // every new type should be always added to the end for compatibility with old archives
    };

    /**
     * Signals after the underlying type or the value has been changed.
     * Instance of this VariantType is passed as argument.
     *
     * Using mutable keyword to make it able to connect slots even on constant variant objects.
     */
    mutable sigc::signal<void, const VariantType&> valueChangedSignal;

    /**
     * Signals when value is about to be changed.
     * Performs a validation of the value. If returns false, the value is not changed.
     * Accepts current value as first argument and new value to be set as a VariantType as a second argument.
     * Validator can also change the new value as needed since it's passed by reference.
     *
     * Validator signal is not invoked for blobs.
     */
    mutable sigc::signal<bool, const VariantType&, VariantType&>::accumulated<interruptable_accumulator> valueValidatorSignal;

public:
    VariantType();
    ~VariantType();

    /**
     * Explicit conversion constructors.
     */
    template<class _Type> explicit VariantType(const _Type& value);

    /**
     * Construct VariantType from a range.
     *
     * @param begin Begin iterator of a range.
     * @param end End iterator of a range.
     */
    template<typename _InputIterator> explicit VariantType(_InputIterator begin, _InputIterator end);

    /**
     * Construct VariantType from a initializer-list.
     */
    template<typename _Type> explicit VariantType(const std::initializer_list<_Type>& list);

    /**
     * Copy constructor.
     */
    VariantType(const VariantType& other);

    /**
     * Assignment operator.
     */
    VariantType& operator=(const VariantType& other);

    /**
     * Whether this instance has no values assigned to it.
     */
    inline bool isEmpty() const;

    /**
     * Set variant to empty.
     */
    inline void clear();

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
     * @param value Value, can be one of supported data types.
     */
    template<typename _Type> inline void set(const _Type& value);

    /**
     * Set the contents of a variant to an Archive instance.
     * This allows to store hierarchical structures inside VariantType.
     *
     * @param archive Arhive instance or NULL to create empty archive inplace.
     */
    void setArchive(const class Archive * archive = NULL);

    /**
     * Get the contents of variant as a given type.
     * You can use 'get' of larger types on smaller ones.
     * For example, you variant hold INT8 value, you can
     * call 'get<int16_t>' to get it as well.
     */
    template<typename _Type> inline const _Type& get() const;

    /**
     * Get contents of a variant as an archive.
     */
    inline class Archive * getArchive() const;

    /**
     * Set contents of variant as a blob (byte array).
     *
     * @param data Data source.
     * @param size Data size.
     */
    void setBlob(const void * data, uint32_t size);

    /**
     * Get the contents of a variant as a byte array (blob).
     *
     * @param[out] size Receives blob's size.
     * @return Pointer to blob's first byte.
     */
    const uint8_t * getBlob(uint32_t * size) const;

    /**
     * Helper function to get blob and convert it to a given data type.
     *
     * @return Pointer to a first byte of blob, converted to a given type.
     */
    template<typename _Type> inline const _Type* getBlob() const;

    /**
     * Get contents of a variant as a list.
     */
    inline const std::vector<VariantType> * getList() const;

    /**
     * Get contents of a variant as a list.
     */
    inline std::vector<VariantType> * getList();

    /**
     * Set the contents of a variant to a list of VariantTypes.
     *
     * @param begin Begin iterator.
     * @param end End iterator.
     */
    template<typename _InputIterator>
    inline void set(_InputIterator begin, _InputIterator end);

    /**
     * Get iterator to a first element of the list stored in the variant.
     */
    inline std::vector<VariantType>::iterator begin();

    /**
     * Get iterator to an element after the last of the list stored in the variant.
     */
    inline std::vector<VariantType>::iterator end();

    /**
     * Get iterator to a first element of the list stored in the variant.
     */
    inline std::vector<VariantType>::const_iterator begin() const;

    /**
     * Get iterator to an element after the last of the list stored in the variant.
     */
    inline std::vector<VariantType>::const_iterator end() const;

    /**
     * Allow to reference list items of VariantType by index.
     *
     * @param pos Index of VariantType inside the list.
     * @return Reference to VariantType.
     */
    inline const VariantType& operator[](unsigned pos) const;

    /**
     * Allow to reference list items of VariantType by index.
     *
     * @param pos Index of VariantType inside the list.
     * @return Reference to VariantType.
     */
    inline VariantType& operator[](unsigned pos);

    /**
     * Set contents of a variant to a data stored in the Python pickle format.
     * This is very simple implementation of Python's Unpickler, it supports only basic types.
     *
     * List and tuples are treated same. Objects instantinations (OBJ, INST, NEWOBJ) are not
     * handled properly. Instead the creation arguments are stored as a list in VariantType.
     *
     * @return False in case of malformed data.
     */
    bool unpickle(gameplay::Stream * stream);

    inline bool operator== (const VariantType& other) const;
    inline bool operator!= (const VariantType& other) const;


    /**
     * Serialize Variant to JSON.
     * 
     * @param outStr Output string where result is appended.
     * @return True on success, False when the value can't be serialized.
     */
    bool serializeToJSON(std::string * outStr) const;

private:
    // This constructor is private to prevent creation of VariantType from pointer
    // Without this, creating VariantType from any pointer will be automatically casted to BOOL
    // by C++ compiler, that is completely wrong
    VariantType(void*);

    void release();

    template<class _Type> inline void setInternal(const _Type& value, _Type& field, Type fieldType);
    template<class _Type> inline void setInternalObject(const _Type& value, _Type* field, Type fieldType);



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

#include "variant.h"



namespace detail
{
    static std::array<std::pair<VariantType::Type, const char *>, VariantType::TYPES_COUNT> __variantNames = {
        std::make_pair(VariantType::TYPE_NONE, "unknown"),
        std::make_pair(VariantType::TYPE_BOOLEAN, "bool"),
        std::make_pair(VariantType::TYPE_INT32, "int32"),
        std::make_pair(VariantType::TYPE_FLOAT, "float"),
        std::make_pair(VariantType::TYPE_STRING, "string"),
        std::make_pair(VariantType::TYPE_WIDE_STRING, "wideString"),
        std::make_pair(VariantType::TYPE_BYTE_ARRAY, "byteArray"),
        std::make_pair(VariantType::TYPE_UINT32, "uint32"),
        std::make_pair(VariantType::TYPE_KEYED_ARCHIVE, "keyedArchive"),
        std::make_pair(VariantType::TYPE_INT64, "int64"),
        std::make_pair(VariantType::TYPE_UINT64, "uint64"),
        std::make_pair(VariantType::TYPE_VECTOR2, "Vector2"),
        std::make_pair(VariantType::TYPE_VECTOR3, "Vector3"),
        std::make_pair(VariantType::TYPE_VECTOR4, "Vector4"),
        std::make_pair(VariantType::TYPE_MATRIX2, "Matrix2"),
        std::make_pair(VariantType::TYPE_MATRIX3, "Matrix3"),
        std::make_pair(VariantType::TYPE_MATRIX4, "Matrix4"),
        std::make_pair(VariantType::TYPE_COLOR, "Color"),
        std::make_pair(VariantType::TYPE_FASTNAME, "FastName"),
        std::make_pair(VariantType::TYPE_AABBOX3, "AABBox3"),
        std::make_pair(VariantType::TYPE_FILEPATH, "FilePath"),
        std::make_pair(VariantType::TYPE_FLOAT64, "float64"),
        std::make_pair(VariantType::TYPE_INT8, "int8"),
        std::make_pair(VariantType::TYPE_UINT8, "uint8"),
        std::make_pair(VariantType::TYPE_INT16, "int16"),
        std::make_pair(VariantType::TYPE_UINT16, "uint16"),
        std::make_pair(VariantType::TYPE_LIST, "list"),
    };
};


template<typename _Type> inline VariantType::VariantType(const _Type& var)
    : type(TYPE_NONE)
    , int64Value(0)
{
    GP_ASSERT(pointerValue == nullptr);
    set(var);
}

template<typename _InputIterator> inline VariantType::VariantType(_InputIterator begin, _InputIterator end)
    : type(TYPE_NONE)
    , int64Value(0)
{
    GP_ASSERT(pointerValue == nullptr);
    set(begin, end);
}

template<typename _Type> inline VariantType::VariantType(const std::initializer_list<_Type>& list)
    : type(TYPE_NONE)
    , int64Value(0)
{
    GP_ASSERT(pointerValue == nullptr);
    set(list.begin(), list.end());
}



inline VariantType::Type VariantType::getType() const
{
    return type;
}

inline const char * VariantType::getTypeName() const
{
    return detail::__variantNames[type].second;
}

template<class _Type> inline void VariantType::setInternal(const _Type& value, _Type& field, Type fieldType)
{
    if (!valueValidatorSignal.empty())
    {
        VariantType newValue(value);
        if (!valueValidatorSignal(*this, newValue))
            return;
        if (newValue.type != type)
        {
            set(newValue);
            return;
        }

        release();
        type = fieldType;
        field = newValue.get<_Type>();
        valueChangedSignal(*this);
        return;
    }
    if (type == fieldType && field == value)
        return;

    release();
    type = fieldType;
    field = value;
    valueChangedSignal(*this);
}

template<class _Type> inline void VariantType::setInternalObject(const _Type& value, _Type* field, Type fieldType)
{
    GP_ASSERT(field == pointerValue);
    if (!valueValidatorSignal.empty())
    {
        VariantType newValue(value);
        if (!valueValidatorSignal(*this, newValue))
            return;
        if (newValue.type != type)
        {
            set(newValue);
            return;
        }

        if (type == fieldType && field && *field == newValue.get<_Type>())
            return;

        release();
        type = fieldType;
        pointerValue = newValue.pointerValue;   // take ownership
        newValue.pointerValue = nullptr;
        valueChangedSignal(*this);
        return;
    }
    if (type == fieldType && field && *field == value)
        return;

    if (type == fieldType && field)
    {
        *field = value;
    }
    else
    {
        release();
        type = fieldType;
        pointerValue = new _Type(value);
    }
    valueChangedSignal(*this);
}

template<> inline void VariantType::set(const bool& value)
{
    setInternal(value, boolValue, TYPE_BOOLEAN);
    uint64Value &= 0xFF;
}

template<> inline void VariantType::set(const int8_t& value)
{
    setInternal(value, int8Value, TYPE_INT8);
    uint64Value &= 0xFF;
}

template<> inline void VariantType::set(const uint8_t& value)
{
    setInternal(value, uint8Value, TYPE_UINT8);
    uint64Value &= 0xFF;
}

template<> inline void VariantType::set(const int16_t& value)
{
    setInternal(value, int16Value, TYPE_INT16);
    uint64Value &= 0xFFFF;
}

template<> inline void VariantType::set(const uint16_t& value)
{
    setInternal(value, uint16Value, TYPE_UINT16);
    uint64Value &= 0xFFFF;
}

template<> inline void VariantType::set(const int32_t& value)
{
    setInternal(value, int32Value, TYPE_INT32);
    uint64Value &= 0xFFFFFFFF;
}

template<> inline void VariantType::set(const uint32_t& value)
{
    setInternal(value, uint32Value, TYPE_UINT32);
    uint64Value &= 0xFFFFFFFF;
}

template<> inline void VariantType::set(const int64_t& value)
{
    setInternal(value, int64Value, TYPE_INT64);
}

template<> inline void VariantType::set(const uint64_t& value)
{
    setInternal(value, uint64Value, TYPE_UINT64);
}

template<> inline void VariantType::set(const float& value)
{
    setInternal(value, floatValue, TYPE_FLOAT);
}

template<> inline void VariantType::set(const double& value)
{
    setInternal(value, float64Value, TYPE_FLOAT64);
}

template<> inline void VariantType::set(const std::string& value)
{
    setInternalObject(value, stringValue, TYPE_STRING);
}

template<> inline void VariantType::set(const std::wstring& value)
{
    setInternalObject(value, wideStringValue, TYPE_WIDE_STRING);
}

template<> inline void VariantType::set(const gameplay::Vector2& value)
{
    setInternalObject(value, vector2Value, TYPE_VECTOR2);
}

template<> inline void VariantType::set(const gameplay::Vector3& value)
{
    setInternalObject(value, vector3Value, TYPE_VECTOR3);
}

template<> inline void VariantType::set(const gameplay::Vector4& value)
{
    setInternalObject(value, vector4Value, TYPE_VECTOR4);
}

template<> inline void VariantType::set(const gameplay::Matrix3& value)
{
    setInternalObject(value, matrix3Value, TYPE_MATRIX3);
}

template<> inline void VariantType::set(const gameplay::Matrix& value)
{
    setInternalObject(value, matrix4Value, TYPE_MATRIX4);
}

template<> inline void VariantType::set(const gameplay::BoundingBox& value)
{
    gameplay::BoundingBox * bbox = reinterpret_cast<gameplay::BoundingBox *>(pointerValue);
    setInternalObject(value, bbox, TYPE_AABBOX3);
}

template<> inline void VariantType::set(const VariantType& value)
{
    if (this == &value)
        return;

    switch (value.type)
    {
    case TYPE_NONE:
        release();
        type = TYPE_NONE;
        break;
    case TYPE_BOOLEAN:
        set(value.boolValue);
        return;
    case TYPE_INT32:
        set(value.int32Value);
        return;
    case TYPE_FLOAT:
        set(value.floatValue);
        return;
    case TYPE_STRING:
        set(*value.stringValue);
        return;
    case TYPE_WIDE_STRING:
        set(*value.wideStringValue);
        return;
    case TYPE_BYTE_ARRAY:
        {
            uint32_t dataSize;
            const uint8_t * dataBuf = value.getBlob(&dataSize);
            setBlob(dataBuf, dataSize);
        }
        return;
    case TYPE_UINT32:
        set(value.uint32Value);
        return;
    case TYPE_KEYED_ARCHIVE:
        setArchive(reinterpret_cast<const Archive *>(value.pointerValue));
        return;
    case TYPE_INT64:
        set(value.int64Value);
        return;
    case TYPE_UINT64:
        set(value.uint64Value);
        return;
    case TYPE_VECTOR2:
        set(*value.vector2Value);
        return;
    case TYPE_VECTOR3:
        set(*value.vector3Value);
        return;
    case TYPE_VECTOR4:
        set(*value.vector4Value);
        return;
    case TYPE_MATRIX3:
        set(*value.matrix3Value);
        return;
    case TYPE_MATRIX4:
        set(*value.matrix4Value);
        return;
    case TYPE_COLOR:
        GP_ASSERT(!"Not implemented yet");
        return;
    case TYPE_FASTNAME:
        GP_ASSERT(!"Not implemented yet");
        return;
    case TYPE_AABBOX3:
        set(*reinterpret_cast<gameplay::BoundingBox *>(value.pointerValue));
        return;
    case TYPE_FILEPATH:
        GP_ASSERT(!"Not implemented yet");
        return;
    case TYPE_FLOAT64:
        set(value.float64Value);
        return;
    case TYPE_INT8:
        set(value.int8Value);
        return;
    case TYPE_UINT8:
        set(value.uint8Value);
        return;
    case TYPE_INT16:
        set(value.int16Value);
        return;
    case TYPE_UINT16:
        set(value.uint16Value);
        return;
    case TYPE_LIST:
        {
            std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(value.pointerValue);
            GP_ASSERT(list);
            set(list->begin(), list->end());
        }
        return;
    default:
        GP_ASSERT(!"Not implemented yet");
    }
}

template<> inline const bool& VariantType::get() const
{
    GP_ASSERT(type == TYPE_BOOLEAN);
    return boolValue;
}

template<> inline const int8_t& VariantType::get() const
{
    GP_ASSERT(type == TYPE_INT8 || type == TYPE_BOOLEAN);
    return int8Value;
}

template<> inline const uint8_t& VariantType::get() const
{
    GP_ASSERT(type == TYPE_UINT8 || type == TYPE_BOOLEAN);
    return uint8Value;
}

template<> inline const int16_t& VariantType::get() const
{
    GP_ASSERT(type == TYPE_INT16 || type == TYPE_INT8 || type == TYPE_BOOLEAN);
    return int16Value;
}

template<> inline const uint16_t& VariantType::get() const
{
    GP_ASSERT(type == TYPE_UINT16 || type == TYPE_UINT8 || type == TYPE_BOOLEAN);
    return uint16Value;
}

template<> inline const int32_t& VariantType::get() const
{
    GP_ASSERT(type == TYPE_INT32 || type == TYPE_INT16 || type == TYPE_INT8 || type == TYPE_BOOLEAN);
    return int32Value;
}

template<> inline const uint32_t& VariantType::get() const
{
    GP_ASSERT(type == TYPE_UINT32 || type == TYPE_UINT16 || type == TYPE_UINT8 || type == TYPE_BOOLEAN);
    return uint32Value;
}

template<> inline const int64_t& VariantType::get() const
{
    GP_ASSERT(type == TYPE_INT64 || type == TYPE_INT32 || type == TYPE_INT16 || type == TYPE_INT8 || type == TYPE_BOOLEAN);
    return int64Value;
}

template<> inline const uint64_t& VariantType::get() const
{
    GP_ASSERT(type == TYPE_UINT64 || type == TYPE_UINT32 || type == TYPE_UINT16 || type == TYPE_UINT8 || type == TYPE_BOOLEAN);
    return uint64Value;
}

template<> inline const float& VariantType::get() const
{
    GP_ASSERT(type == TYPE_FLOAT);
    return floatValue;
}

template<> inline const double& VariantType::get() const
{
    GP_ASSERT(type == TYPE_FLOAT64);
    return float64Value;
}

template<> inline const std::string& VariantType::get() const
{
    GP_ASSERT(type == TYPE_STRING);
    return *stringValue;
}

template<> inline const std::wstring& VariantType::get() const
{
    GP_ASSERT(type == TYPE_WIDE_STRING);
    return *wideStringValue;
}

template<> inline const gameplay::Vector2& VariantType::get() const
{
    GP_ASSERT(type == TYPE_VECTOR2);
    return *vector2Value;
}

template<> inline const gameplay::Vector3& VariantType::get() const
{
    GP_ASSERT(type == TYPE_VECTOR3);
    return *vector3Value;
}

template<> inline const gameplay::Vector4& VariantType::get() const
{
    GP_ASSERT(type == TYPE_VECTOR4);
    return *vector4Value;
}

template<> inline const gameplay::Matrix3& VariantType::get() const
{
    GP_ASSERT(type == TYPE_MATRIX3);
    return *matrix3Value;
}

template<> inline const gameplay::Matrix& VariantType::get() const
{
    GP_ASSERT(type == TYPE_MATRIX4);
    return *matrix4Value;
}

template<> inline const gameplay::BoundingBox& VariantType::get() const
{
    GP_ASSERT(type == TYPE_AABBOX3);
    return *reinterpret_cast<gameplay::BoundingBox *>(pointerValue);
}

inline class Archive * VariantType::getArchive() const
{
    GP_ASSERT(type == TYPE_KEYED_ARCHIVE);
    return reinterpret_cast<class Archive *>(pointerValue);
}

inline bool VariantType::operator==(const VariantType& value) const
{
    if (type != value.type)
        return false;

    switch (type)
    {
    case TYPE_BOOLEAN:
        return value.boolValue == boolValue;
    case TYPE_INT32:
        return value.int32Value == int32Value;
    case TYPE_FLOAT:
        return value.floatValue == floatValue;
    case TYPE_STRING:
        return *value.stringValue == *stringValue;
    case TYPE_WIDE_STRING:
        return *value.wideStringValue == *wideStringValue;
    case TYPE_BYTE_ARRAY:
        {
            uint32_t dataSize, otherDataSize;
            const uint8_t * dataBuf = getBlob(&dataSize);
            const uint8_t * otherDataBuf = value.getBlob(&otherDataSize);
            return dataSize == otherDataSize && memcmp(dataBuf, otherDataBuf, dataSize) == 0;
        }
    case TYPE_UINT32:
        return value.uint32Value == uint32Value;
    case TYPE_KEYED_ARCHIVE:
        return false;
    case TYPE_INT64:
        return value.int64Value == int64Value;
    case TYPE_UINT64:
        return value.uint64Value == uint64Value;
    case TYPE_VECTOR2:
        return *value.vector2Value == *vector2Value;
    case TYPE_VECTOR3:
        return *value.vector3Value == *vector3Value;
    case TYPE_VECTOR4:
        return *value.vector4Value == *vector4Value;
    case TYPE_MATRIX3:
        return memcmp(value.matrix3Value, matrix3Value, sizeof(gameplay::Matrix3)) == 0;
    case TYPE_MATRIX4:
        return memcmp(value.matrix4Value, matrix4Value, sizeof(gameplay::Matrix)) == 0;
    case TYPE_COLOR:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case TYPE_FASTNAME:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case TYPE_AABBOX3:
        return *reinterpret_cast<gameplay::BoundingBox *>(value.pointerValue) == *reinterpret_cast<gameplay::BoundingBox *>(pointerValue);
    case TYPE_FILEPATH:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case TYPE_FLOAT64:
        return value.float64Value == float64Value;
    case TYPE_INT8:
        return value.int8Value == int8Value;
    case TYPE_UINT8:
        return value.uint8Value == uint8Value;
    case TYPE_INT16:
        return value.int16Value == int16Value;
    case TYPE_UINT16:
        return value.uint16Value == uint16Value;
    case TYPE_LIST:
        {
            std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(pointerValue);
            std::vector<VariantType> * otherList = reinterpret_cast<std::vector<VariantType> *>(value.pointerValue);
            return list->size() == otherList->size() && std::mismatch(list->begin(), list->end(), otherList->begin()).first == list->end();
        }
    default:
        GP_ASSERT(!"Not implemented yet");
    }
    return false;
}

inline bool VariantType::operator!=(const VariantType& other) const
{
    return !(*this == other);
}

inline bool VariantType::isEmpty() const
{
    return type == TYPE_NONE;
}

inline void VariantType::clear()
{
    release();
    type = TYPE_NONE;
}

template<typename _Type> inline const _Type * VariantType::getBlob() const
{
    uint32_t size;
    const uint8_t * data = getBlob(&size);
    GP_ASSERT(!data || size == sizeof(_Type));

    return reinterpret_cast<const _Type *>(data);
}

template<typename _InputIterator> inline void VariantType::set(_InputIterator begin, _InputIterator end)
{
    if (!valueValidatorSignal.empty())
    {
        VariantType newValue(begin, end);
        if (!valueValidatorSignal(*this, newValue))
            return;
        if (newValue.type != type)
        {
            set(newValue);
            return;
        }

        release();
        type = TYPE_LIST;
        pointerValue = newValue.pointerValue;   // take ownership
        newValue.pointerValue = nullptr;
        valueChangedSignal(*this);
        return;
    }

    std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(pointerValue);

    if (type == TYPE_LIST && list->size() == (size_t)std::distance(begin, end)
        && std::mismatch(list->begin(), list->end(), begin, [&](const VariantType& a, const typename std::iterator_traits<_InputIterator>::value_type& b) { return a == VariantType(b); }).first == list->end())
        return;

    list = new std::vector<VariantType>();
    for (_InputIterator it = begin; it != end; ++it)
        list->push_back(VariantType(*it));

    release();
    type = TYPE_LIST;
    pointerValue = list;

    valueChangedSignal(*this);
}

inline std::vector<VariantType>::iterator VariantType::begin()
{
    GP_ASSERT(type == TYPE_LIST);
    std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(pointerValue);
    return list->begin();
}

inline std::vector<VariantType>::iterator VariantType::end()
{
    GP_ASSERT(type == TYPE_LIST);
    std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(pointerValue);
    return list->end();
}

inline std::vector<VariantType>::const_iterator VariantType::begin() const
{
    GP_ASSERT(type == TYPE_LIST);
    std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(pointerValue);
    return list->begin();
}

inline std::vector<VariantType>::const_iterator VariantType::end() const
{
    GP_ASSERT(type == TYPE_LIST);
    std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(pointerValue);
    return list->end();
}

inline VariantType& VariantType::operator[](unsigned pos)
{
    GP_ASSERT(type == TYPE_LIST);
    std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(pointerValue);
    return (*list)[pos];
}

inline const VariantType& VariantType::operator[](unsigned pos) const
{
    GP_ASSERT(type == TYPE_LIST);
    std::vector<VariantType> * list = reinterpret_cast<std::vector<VariantType> *>(pointerValue);
    return (*list)[pos];
}

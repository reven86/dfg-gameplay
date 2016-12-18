#include "pch.h"
#include "variant.h"
#include "archive.h"



VariantType::VariantType()
    : type(TYPE_NONE)
    , pointerValue(nullptr)
{
}

VariantType::~VariantType()
{
    release();
}

VariantType& VariantType::operator= (const VariantType& other)
{
    set(other);
    return *this;
}

void VariantType::setBlob(const void * data, uint32_t size)
{
    if (type == TYPE_BYTE_ARRAY && (pointerValue == NULL && size == 0 || pointerValue != NULL && size == reinterpret_cast<std::vector<uint8_t> *>(pointerValue)->size() && memcmp(&(*reinterpret_cast<std::vector<uint8_t> *>(pointerValue)->begin()), data, size) == 0))
        return;

    const uint8_t * buf = reinterpret_cast<const uint8_t *>(data);
    if (type == TYPE_BYTE_ARRAY)
    {
        std::vector<uint8_t> * src = reinterpret_cast<std::vector<uint8_t> *>(pointerValue);
        if (src && src->size() == size)
        {
            // copy the data inplace without reallocating the vector
            src->assign(buf, buf + size);
            return;
        }
    }

    release();
    type = TYPE_BYTE_ARRAY;
    pointerValue = size > 0 ? reinterpret_cast<void *>(new std::vector<uint8_t>(buf, buf + size)) : NULL;

    valueChangedSignal(*this);
}

const uint8_t * VariantType::getBlob(uint32_t * size) const
{
    GP_ASSERT(type == TYPE_BYTE_ARRAY);
    std::vector<uint8_t> * buf = reinterpret_cast<std::vector<uint8_t> *>(pointerValue);

    if (!buf)
        return NULL;

    if (size)
        *size = static_cast<uint32_t>(buf->size());        

    return &(*buf->begin());
}

void VariantType::set(const Archive * archive)
{
    if (!valueValidatorSignal.empty())
    {
        VariantType newValue(archive);
        if (!valueValidatorSignal(*this, newValue))
            return;
        if (newValue.type != type)
        {
            set(newValue);
            return;
        }

        release();
        type = TYPE_KEYED_ARCHIVE;
        pointerValue = newValue.get() ? Archive::create(*newValue.get()) : Archive::create();
        valueChangedSignal(*this);
        return;
    }

    if (type == TYPE_KEYED_ARCHIVE && pointerValue == archive)
        return;

    release();
    type = TYPE_KEYED_ARCHIVE;
    pointerValue = archive ? Archive::create(*archive) : Archive::create();
    valueChangedSignal(*this);
}

void VariantType::release()
{
    if (!pointerValue)
        return;

    switch (type)
    {
    case TYPE_VECTOR2:
        SAFE_DELETE(vector2Value);
        return;
    case TYPE_VECTOR3:
        SAFE_DELETE(vector3Value);
        return;
    case TYPE_VECTOR4:
        SAFE_DELETE(vector4Value);
        return;
    case TYPE_MATRIX3:
        SAFE_DELETE(matrix3Value);
        return;
    case TYPE_MATRIX4:
        SAFE_DELETE(matrix4Value);
        return;
    case TYPE_STRING:
        SAFE_DELETE(stringValue);
        return;
    case TYPE_WIDE_STRING:
        SAFE_DELETE(wideStringValue);
        return;
    case TYPE_BYTE_ARRAY:
        delete reinterpret_cast<std::vector<uint8_t> *>(pointerValue);
        pointerValue = NULL;
        return;
    case TYPE_KEYED_ARCHIVE:
        delete reinterpret_cast<class Archive *>(pointerValue);
        pointerValue = NULL;
        return;
    default:
        // do nothing, it's not an error to get here
        break;
    }
}
#include "pch.h"
#include "variant.h"



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
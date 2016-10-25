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

void VariantType::setBlob(const uint8_t * data, uint32_t size)
{
    release();
    type = TYPE_BYTE_ARRAY;
    std::vector<uint8_t> * buf = new std::vector<uint8_t>(data, data + size);
    pointerValue = reinterpret_cast<void *>(buf);
}

const uint8_t * VariantType::getBlob(uint32_t * size) const
{
    GP_ASSERT(type == TYPE_BYTE_ARRAY);
    std::vector<uint8_t> * buf = reinterpret_cast<std::vector<uint8_t> *>(pointerValue);

    if (!buf)
        return false;

    if (size)
        *size = static_cast<uint32_t>(buf->size());        

    return &(*buf->begin());
}
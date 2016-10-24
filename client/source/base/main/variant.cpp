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
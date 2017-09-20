#include "archive.h"



inline bool Archive::hasKey(const char * key) const
{
    return _values.find(key) != _values.end();
}

template<typename _Type> inline const _Type& Archive::get(const char * key, const _Type& defaultValue) const
{
    const auto& it = _values.find(key);
    return it == _values.end() ? defaultValue : (*it).second.get<_Type>();
}

template<> inline const VariantType& Archive::get(const char * key, const VariantType& defaultValue) const
{
    const auto& it = _values.find(key);
    return it == _values.end() ? defaultValue : (*it).second;
}

inline VariantType * Archive::get(const char * key)
{
    auto it = _values.find(key);
    return it == _values.end() ? NULL : &(*it).second;
}

inline const VariantType * Archive::get(const char * key) const
{
    auto it = _values.find(key);
    return it == _values.end() ? NULL : &(*it).second;
}

template<typename _Type> inline VariantType& Archive::set(const char * key, const _Type& value)
{
    VariantType& archMember = _values[key];
    archMember.set(value);
    return archMember;
}

inline void Archive::removeKey(const char * key)
{
    _values.erase(key);
}

inline void Archive::clear()
{
    _values.clear();
}

template<typename _Type> inline const _Type * Archive::getBlob(const char * key) const
{
    uint32_t size;
    const uint8_t * data = getBlob(key, &size);
    GP_ASSERT(!data || size == sizeof(_Type));

    return reinterpret_cast<const _Type *>(data);
}

inline VariantType& Archive::setBlob(const char * key, const void * data, uint32_t size)
{
    VariantType& archMember = _values[key];
    archMember.setBlob(data, size);

    return archMember;
}

inline const uint8_t * Archive::getBlob(const char * key, uint32_t * outSize) const
{
    const auto& it = _values.find(key);
    return it == _values.end() ? NULL : (*it).second.getBlob(outSize);
}
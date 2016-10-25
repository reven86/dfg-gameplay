#include "archive.h"



inline bool Archive::hasKey(const char * key) const
{
    return _values.find(key) != _values.end();
}

template<typename _Type> inline const _Type& Archive::get(const char * key, const _Type& defaultValue) const
{
    auto& it = _values.find(key);
    return it == _values.end() ? defaultValue : (*it).second.get<_Type>();
}

template<typename _Type> inline void Archive::set(const char * key, const _Type& value)
{
    _values[key].set(value);
}

inline void Archive::removeKey(const char * key)
{
    _values.erase(key);
}

inline void Archive::clear()
{
    _values.clear();
}

template<typename _Type> inline const _Type * Archive::getBlob(const char * key)
{
    uint32_t size;
    const uint8_t * data = getBlob(key, &size);
    GP_ASSERT(!data || size == sizeof(_Type));

    return reinterpret_cast<const _Type *>(data);
}

inline void Archive::setBlob(const char * key, const uint8_t * data, uint32_t size)
{
    _values[key].setBlob(data, size);
}

inline const uint8_t * Archive::getBlob(const char * key, uint32_t * outSize)
{
    auto& it = _values.find(key);
    return it == _values.end() ? NULL : (*it).second.getBlob(outSize);
}
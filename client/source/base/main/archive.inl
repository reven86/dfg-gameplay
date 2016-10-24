#include "archive.h"



inline bool Archive::hasKey(const char * key) const
{
    return _values.find(key) != _values.end();
}

template<typename _Type> inline const _Type& Archive::get(const char * key, const _Type& defaultValue) const
{
    auto& it = _values.find(key);
    return it == _values.end() ? defaultValue : (*it).second;
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
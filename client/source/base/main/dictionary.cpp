#include "pch.h"
#include "dictionary.h"







Dictionary::Dictionary()
{
}

Dictionary::~Dictionary()
{
}

void Dictionary::create(gameplay::Properties * properties)
{
    const char * key = NULL;
    while ((key = properties->getNextProperty()) != NULL)
    {
        const char * value = properties->getString(NULL, key); // utf8

        GP_ASSERT(_dictionary.find(key) == _dictionary.end());

        _dictionary.insert(std::make_pair(key, Utils::UTF8ToWCS(value)));
        _dictionaryUTF8.insert(std::make_pair(key, value));
    }
}

const std::wstring& Dictionary::lookup(const char * key) const
{
    std::unordered_map< std::string, std::wstring >::const_iterator it = _dictionary.find(key);

    GP_ASSERT(it != _dictionary.end());
    if (it == _dictionary.end())
    {
        static std::wstring res(L"<not localized>");
        return res;
    }

    return (*it).second;
}

const std::string& Dictionary::lookupUTF8(const char * key) const
{
    std::unordered_map< std::string, std::string >::const_iterator it = _dictionaryUTF8.find(key);

    GP_ASSERT(it != _dictionaryUTF8.end());
    if (it == _dictionaryUTF8.end())
    {
        static std::string res("<not localized>");
        return res;
    }

    return (*it).second;
}

bool Dictionary::hasEntry(const char * key) const
{
    return _dictionary.find(key) != _dictionary.end();
}
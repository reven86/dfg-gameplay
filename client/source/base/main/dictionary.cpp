#include "pch.h"
#include "dictionary.h"
#include <unicode/ustring.h>







Dictionary::Dictionary()
{
}

Dictionary::~Dictionary()
{
}

void Dictionary::create(gameplay::Properties * properties)
{
    gameplay::Properties* subProperties = NULL;
    while ((subProperties = properties->getNextNamespace()) != NULL)
    {
        if (strcmp(subProperties->getNamespace(), "entry") == 0)
        {
            const char * key = subProperties->getString("key");
            const char * value = subProperties->getString("value"); // utf8

            GP_ASSERT(_dictionary.find(key) == _dictionary.end());

            UChar valueUni[1024];
            int32_t length = 0;
            UErrorCode error = U_ZERO_ERROR;
            u_strFromUTF8(valueUni, 1024, &length, value, -1, &error);

            if (error != U_ZERO_ERROR)
                GP_WARN("Can't read dictionary entry: %s", key);
            else
            {
                error = U_ZERO_ERROR;
                wchar_t valueW[1024];
                u_strToWCS(valueW, 1024, &length, valueUni, -1, &error);

                if (error == U_ZERO_ERROR)
                    _dictionary.insert(std::make_pair(key, valueW));
            }

            _dictionaryUTF8.insert(std::make_pair(key, value));
        }
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
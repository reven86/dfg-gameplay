#ifndef __DFG_DICTIONARY__
#define __DFG_DICTIONARY__






/** Dictionary maps key-strings (tech names) to UTF8/wchar localized text.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class Dictionary
{
    std::unordered_map< std::string, std::wstring > _dictionary;
    std::unordered_map< std::string, std::string > _dictionaryUTF8;

public:
    Dictionary();
    ~Dictionary();

    /*! Creates dictionary from Properties object.
     *
     *  Properties object should contain a list of key-value pairs,
     *  the value is expected to be an UTF8 encoded string.
     */
    void create(gameplay::Properties * properties);

    //! Lookup localized string from dictionary.
    const std::wstring& lookup(const char * key) const;

    //! Lookup localized string from dictionary. Returns UTF8 encoded string.
    const std::string& lookupUTF8(const char * key) const;

    //! Does dictionary contain entry.
    bool hasEntry(const char * key) const;
};




#endif

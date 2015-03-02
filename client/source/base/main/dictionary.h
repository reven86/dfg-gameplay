#ifndef __DFG_DICTIONARY__
#define __DFG_DICTIONARY__






/*! Dictionary strings (tech names) to localized objects.
 *
 *  For now, only string-string mapping is supported.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class Dictionary
{
    std::unordered_map< std::string, std::wstring > _dictionary;
    std::unordered_map< std::string, std::string > _dictionaryUTF8;

public:
    Dictionary( );
    ~Dictionary( );

    /*! Creates dictionary from Properties object.
     *
     *  Properties object should contain a list of key-value pairs,
     *  the value expected to be an UTF8 encoded string.
     */
    void Create( gameplay::Properties * properties );

    //! Lookup localized string from dictionary.
    const std::wstring& Lookup( const char * key ) const;

    //! Lookup localized string from dictionary. Returns UTF8 encoded string.
    const std::string& LookupUTF8( const char * key ) const;

    //! Does dictionary contain entry.
    bool HasEntry( const char * key ) const;
};




#endif

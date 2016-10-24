#pragma once

#ifndef __DFG_ARCHIVE_H__
#define __DFG_ARCHIVE_H__




/**
 * Class used for serialization and deserialization.
 * The internal structure and layout is copied from DAVA Framework KeyedArchive class
 * (https://github.com/dava/dava.engine/blob/development/Sources/Internal/FileSystem/KeyedArchive.h)
 * Thus, the produced archive files are fully compatible with DAVA KeyedArchive files.
 *
 * It's different from gameplay::Properties in the purpose of usage. Properties are mainly used
 * for human-readable config files, they contain untyped values and can't be saved. Archives,
 * on the other hand, are used to serialize and deserialize typed data in binary form.
 *
 * \note Archives are not platform independent!
 *
 * TODO: Add an Archive constructor that takes gameplay::Properties as an input.
 */

class Archive : Noncopyable
{
public:
    virtual ~Archive();

    /**
     * Create Archive class for a given stream.
     */
    static Archive * create();

    template<typename _Type> inline void set(const char * key, const _Type& value);
    template<typename _Type> inline const _Type& get(const char * key, const _Type& defaultValue = _Type()) const;

    /**
     * \brief Function to check if key is available in this archive.
     * \param[in] key string key
     * \returns true if key available
	 */
    inline bool hasKey(const char * key) const;

    /**
     * \brief Remove key from archive.
     * \param[in] key name of the key to delete
     */
    inline void removeKey(const char * key);

    /**
     * \brief Clear arhive.
     */
    inline void clear();

    /**
     * Serialize Archive to stream.
     *
     * \param stream Stream to serialize to.
     * \return True if archive has been successfully serialized to stream;
     */
    bool serialize(gameplay::Stream * stream) const;

    /**
     * Deserialize Archive from stream.
     *
     * \param stream Stream to deserialize from.
     * \return True if archive has been successfully loaded from the stream.
     */
    bool deserialize(gameplay::Stream * stream);

protected:
    Archive();

    bool serializeVariant(gameplay::Stream * stream, const VariantType& value) const;
    bool deserializeVariant(gameplay::Stream * stream, VariantType * out);

    std::unordered_map<std::string, VariantType> _values;
};



#include "archive.inl"


#endif // __DFG_ARCHIVE_H__

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

    /**
     * Set arbitrary POD data for a given key.
     *
     * \param key String key.
     * \param value POD value, should be one of supported by VariantType.
     * \return Reference to newly added/changed underlying VariantType instance.
     *
     * \see VariantType.
     */
    template<typename _Type> inline VariantType& set(const char * key, const _Type& value);

    /**
     * Get data from archive for a given key.
     *
     * \param key String key.
     * \param defaultValue Default value when key is not found.
     *
     * \return Value from archive for a given key or default one if key is not present.
     */
    template<typename _Type> inline const _Type& get(const char * key, const _Type& defaultValue = _Type()) const;

    /**
     * Get underlying variant type for a given key.
     * Allows you to save and modify the value later without looking up it again.
     *
     * \param key String key.
     * \param defaultValue Default value when key is not found.
     *
     * \return Value from archive for a given key or NULL.
     */
    inline VariantType * get(const char * key);

    /**
     * Insert byte array (blob) into the archive for a given key.
     *
     * \param key String key.
     * \param data Source data.
     * \param size Data size.
     */
    inline VariantType& setBlob(const char * key, const void * data, uint32_t size);

    /**
     * Get byte array (blob) from the archive for a given key.
     *
     * \param key String key.
     * \param[out] outSize Receives the size of the blob.
     * \return Pointer to first byte in the blob or NULL if key is not found.
     */
    inline const uint8_t * getBlob(const char * key, uint32_t * outSize) const;

    /**
     * Helper function to get blob and convert it to a given data type.
     *
     * \param key String key.
     * \return Pointer to a first byte of blob, converted to a given type.
     */
    template<typename _Type> inline const _Type* getBlob(const char * key) const;

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

    /**
     * Get list of keys that present in both archives.
     * Useful for merging or updating one archive with contents of the other.
     *
     * \param other Second archive.
     * \param[out] outKeyList List of keys common for both archives.
     */
    void getCommonKeys(const Archive& other, std::vector<std::string> * outKeyList) const;

protected:
    Archive();

    bool serializeVariant(gameplay::Stream * stream, const VariantType& value) const;
    bool deserializeVariant(gameplay::Stream * stream, VariantType * out);

    std::unordered_map<std::string, VariantType> _values;
};



#include "archive.inl"


#endif // __DFG_ARCHIVE_H__

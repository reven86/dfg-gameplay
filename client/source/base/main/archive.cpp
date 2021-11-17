#include "pch.h"
#include "archive.h"
#include "memory_stream.h"




Archive::Archive()
{
}

Archive::~Archive()
{
}

Archive * Archive::create()
{
    return new Archive();
}

Archive * Archive::create(const Archive& other)
{
    Archive * res = new Archive();
    for (const auto& kv : other._values)
        res->set(kv.first.c_str(), VariantType(kv.second));
    return res;
}

bool Archive::serialize(gameplay::Stream * stream) const
{
    uint8_t header[2] = { 'K', 'A' };
    uint16_t version = 1;
    uint32_t itemsCount = static_cast<uint32_t>(_values.size());

    if (stream->write(header, 1, 2) != 2)
        return false;
    if (stream->write(&version, sizeof(version), 1) != 1)
        return false;
    if (stream->write(&itemsCount, sizeof(itemsCount), 1) != 1)
        return false;

    for (const auto& item : _values)
    {
        if (!serializeVariant(stream, VariantType(item.first)))
            return false;
        if (!serializeVariant(stream, item.second))
            return false;
    }
    return true;
}

bool Archive::deserialize(gameplay::Stream * stream, const Archive * dictionary)
{
    uint8_t header[2];
    if (stream->read(header, 1, 2) != 2)
        return false;

    if (header[0] != 'K' || header[1] != 'A')
        return false;

    uint16_t version;
    if (stream->read(&version, sizeof(version), 1) != 1)
        return false;

    if (version == 1)
    {
        uint32_t itemsCount;
        if (stream->read(&itemsCount, sizeof(itemsCount), 1) != 1)
            return false;

        for (uint32_t i = 0; i < itemsCount; i++)
        {
            VariantType key, value;

            if (stream->eof())
                return true;

            if (!deserializeVariant(stream, &key) || !deserializeVariant(stream, &value))
            {
                clear();
                return false;
            }

            _values[key.get<std::string>()] = value;
        }
    }
    else if (version == 0x0002)
    {
        uint32_t itemsCount;
        if (stream->read(&itemsCount, sizeof(itemsCount), 1) != 1)
            return false;

        std::vector<std::string> keys;

        uint32_t i;
        for (i = 0; i < itemsCount; i++)
        {
            if (stream->eof())
                return true;

            uint16_t len;
            if (stream->read(&len, sizeof(len), 1) != 1)
                return false;
            std::unique_ptr<char[]> buf(new char[len + 1]);
            if (stream->read(buf.get(), 1, len) != len)
                return false;
            buf[len] = '\0';
            keys.push_back(std::string(buf.get()));
        }

        for (i = 0; i < itemsCount; i++)
        {
            if (stream->eof())
                return true;

            char hash[4];
            if (stream->read(hash, 4, 1) != 1)
                return false;

            _values[std::string(hash, 4)].set(keys[i]);
        }
    }
    else if (version == 0x0102)
    {
        // need to use the hash-to-string dictionary
        GP_ASSERT(dictionary);

        uint32_t itemsCount;
        if (stream->read(&itemsCount, sizeof(itemsCount), 1) != 1)
            return false;

        for (uint32_t i = 0; i < itemsCount; i++)
        {
            if (stream->eof())
                return true;

            char keyHash[4];
            if (stream->read(keyHash, 4, 1) != 1)
                return false;

            const std::string& key = (*dictionary->_values.find(std::string(keyHash, 4))).second.get<std::string>();

            if (!deserializeVariant(stream, &_values[key], dictionary))
            {
                clear();
                return false;
            }
        }
    }
    else if (version == 0xFF02)
    {
        // empty archive
        return true;
    }
    else
    {
        GP_ASSERT(!"Unknown Archive version");
        return false;
    }

    return true;
}

bool Archive::serializeVariant(gameplay::Stream * stream, const VariantType& value) const
{
    VariantType::Type type = value.getType();
    if (stream->write(&type, 1, 1) != 1)
        return false;

    switch (type)
    {
    case VariantType::TYPE_BOOLEAN:
        return stream->write(&value.get<bool>(), 1, 1) == 1;
    case VariantType::TYPE_INT8:
        return stream->write(&value.get<int8_t>(), 1, 1) == 1;
    case VariantType::TYPE_UINT8:
        return stream->write(&value.get<uint8_t>(), 1, 1) == 1;
    case VariantType::TYPE_INT16:
        return stream->write(&value.get<int16_t>(), 2, 1) == 1;
    case VariantType::TYPE_UINT16:
        return stream->write(&value.get<uint16_t>(), 2, 1) == 1;
    case VariantType::TYPE_INT32:
        return stream->write(&value.get<int32_t>(), 4, 1) == 1;
    case VariantType::TYPE_UINT32:
        return stream->write(&value.get<uint32_t>(), 4, 1) == 1;
    case VariantType::TYPE_INT64:
        return stream->write(&value.get<int64_t>(), 8, 1) == 1;
    case VariantType::TYPE_UINT64:
        return stream->write(&value.get<uint64_t>(), 8, 1) == 1;
    case VariantType::TYPE_FLOAT:
        return stream->write(&value.get<float>(), sizeof(float), 1) == 1;
    case VariantType::TYPE_FLOAT64:
        return stream->write(&value.get<double>(), sizeof(double), 1) == 1;
    case VariantType::TYPE_STRING:
        {
            const std::string& str = value.get<std::string>();
            uint32_t len = static_cast<uint32_t>(str.size());
            return stream->write(&len, sizeof(len), 1) == 1 && stream->write(str.c_str(), 1, len) == len;
        }
    case VariantType::TYPE_WIDE_STRING:
        {
            const std::wstring& str = value.get<std::wstring>();
            uint32_t len = static_cast<uint32_t>(str.size());
            return stream->write(&len, sizeof(len), 1) == 1 && stream->write(str.c_str(), sizeof(wchar_t), len) == len;
        }
    case VariantType::TYPE_BYTE_ARRAY:
        {
            uint32_t size;
            const uint8_t * buf = value.getBlob(&size);
            return stream->write(&size, sizeof(size), 1) == 1 && stream->write(buf, 1, size) == size;
        }
    case VariantType::TYPE_KEYED_ARCHIVE:
        {
            std::unique_ptr<MemoryStream> archiveStream(MemoryStream::create());
            if(!value.getArchive()->serialize(archiveStream.get()))
                return false;

            uint32_t len = archiveStream->length();
            return stream->write(&len, sizeof(len), 1) == 1 && stream->write(archiveStream->getBuffer(), 1, len) == len;
        }
    case VariantType::TYPE_VECTOR2:
        return stream->write(&value.get<gameplay::Vector2>(), sizeof(gameplay::Vector2), 1) == 1;
    case VariantType::TYPE_VECTOR3:
        return stream->write(&value.get<gameplay::Vector3>(), sizeof(gameplay::Vector3), 1) == 1;
    case VariantType::TYPE_VECTOR4:
        return stream->write(&value.get<gameplay::Vector4>(), sizeof(gameplay::Vector4), 1) == 1;
    case VariantType::TYPE_MATRIX2:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_MATRIX3:
        return stream->write(&value.get<gameplay::Matrix3>(), sizeof(gameplay::Matrix3), 1) == 1;
    case VariantType::TYPE_MATRIX4:
        return stream->write(&value.get<gameplay::Matrix>(), sizeof(gameplay::Matrix), 1) == 1;
    case VariantType::TYPE_COLOR:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_FASTNAME:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_AABBOX3:
        return stream->write(&value.get<gameplay::BoundingBox>(), sizeof(gameplay::BoundingBox), 1) == 1;
    case VariantType::TYPE_FILEPATH:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_LIST:
        {
            uint32_t size = std::distance(value.begin(), value.end());
            if (stream->write(&size, sizeof(size), 1) != 1)
                return false;

            for (const VariantType& v : value)
                if (!serializeVariant(stream, v))
                    return false;
        }
        return true;
    default:
        GP_ASSERT(!"Not implemented yet");
    }

    return false;
}

bool Archive::deserializeVariant(gameplay::Stream * stream, VariantType * out, const Archive * dictionary)
{
    VariantType::Type type;
    if (stream->read(&type, 1, 1) != 1)
        return false;

    switch (type)
    {
    case VariantType::TYPE_BOOLEAN:
        {
            bool value;
            if (stream->read(&value, 1, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_INT8:
        {
            int8_t value;
            if (stream->read(&value, 1, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_UINT8:
        {
            uint8_t value;
            if (stream->read(&value, 1, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_INT16:
        {
            int16_t value;
            if (stream->read(&value, 2, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_UINT16:
        {
            uint16_t value;
            if (stream->read(&value, 2, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_INT32:
        {
            int32_t value;
            if (stream->read(&value, 4, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_UINT32:
        {
            uint32_t value;
            if (stream->read(&value, 4, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_INT64:
        {
            int64_t value;
            if (stream->read(&value, 8, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_UINT64:
        {
            uint64_t value;
            if (stream->read(&value, 8, 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_FLOAT:
        {
            float value;
            if (stream->read(&value, sizeof(float), 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_FLOAT64:
        {
            double value;
            if (stream->read(&value, sizeof(double), 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_FASTNAME:
    case VariantType::TYPE_STRING:
        {
            if (dictionary)
            {
                char keyHash[4];
                if (stream->read(keyHash, 4, 1) != 1)
                    return false;

                out->set((*dictionary->_values.find(std::string(keyHash, 4))).second.get<std::string>());
            }
            else
            {
                uint32_t len;
                if (stream->read(&len, sizeof(len), 1) != 1)
                    return false;
                std::unique_ptr<char[]> buf(new char[len + 1]);
                if (stream->read(buf.get(), 1, len) != len)
                    return false;
                buf[len] = '\0';
                out->set(std::string(buf.get()));
            }
        }
        return true;
    case VariantType::TYPE_WIDE_STRING:
        {
            uint32_t len;
            if (stream->read(&len, sizeof(len), 1) != 1)
                return false;
            std::unique_ptr<wchar_t[]> buf(new wchar_t[len + 1]);
            if (stream->read(buf.get(), sizeof(wchar_t), len) != len)
                return false;
            buf[len] = L'\0';
            out->set(std::wstring(buf.get()));
        }
        return true;
    case VariantType::TYPE_BYTE_ARRAY:
        {
            uint32_t size;
            if (stream->read(&size, sizeof(size), 1) != 1)
                return false;
            std::unique_ptr<uint8_t[]> buf(new uint8_t[size]);
            if (stream->read(buf.get(), 1, size) != size)
                return false;
            out->setBlob(buf.get(), size);
        }
        return true;
    case VariantType::TYPE_KEYED_ARCHIVE:
        {
            uint32_t size;
            if (stream->read(&size, sizeof(size), 1) != 1)
                return false;

            std::unique_ptr<uint8_t[]> buf(new uint8_t[size]);
            if (stream->read(buf.get(), 1, size) != size)
                return false;

            std::unique_ptr<MemoryStream> archiveStream(MemoryStream::create(buf, size));
            out->setArchive(NULL);
            return out->getArchive()->deserialize(archiveStream.get(), dictionary);
        }
    case VariantType::TYPE_VECTOR2:
        {
            gameplay::Vector2 value;
            if (stream->read(&value, sizeof(gameplay::Vector2), 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_VECTOR3:
        {
            gameplay::Vector3 value;
            if (stream->read(&value, sizeof(gameplay::Vector3), 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_VECTOR4:
        {
            gameplay::Vector4 value;
            if (stream->read(&value, sizeof(gameplay::Vector4), 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_MATRIX2:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_MATRIX3:
        {
            gameplay::Matrix3 value;
            if (stream->read(&value, sizeof(gameplay::Matrix3), 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_MATRIX4:
        {
            gameplay::Matrix value;
            if (stream->read(&value, sizeof(gameplay::Matrix), 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_COLOR:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_AABBOX3:
        {
            gameplay::BoundingBox value;
            if (stream->read(&value, sizeof(gameplay::BoundingBox), 1) != 1)
                return false;
            out->set(value);
        }
        return true;
    case VariantType::TYPE_FILEPATH:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_LIST:
        {
            uint32_t size;
            if (stream->read(&size, sizeof(size), 1) != 1)
                return false;

            auto tmpIt = std::vector<VariantType>().end();
            out->set(tmpIt, tmpIt); // initialize variant as an empty list
            out->getList()->resize(size);
            
            for (VariantType& v : *out->getList())
                if (!deserializeVariant(stream, &v, dictionary))
                    return false;
        }
        return true;
    default:
        break;
    }

    GP_ASSERT(!"Not implemented yet");
    return false;
}

void Archive::getCommonKeys(const Archive& other, std::vector<std::string> * outKeyList) const
{
    GP_ASSERT(outKeyList);

    outKeyList->clear();

    std::set<std::string> keys1;
    std::set<std::string> keys2;

    for (const auto& it1 : _values)
        keys1.insert(it1.first);
    for (const auto& it2 : other._values)
        keys2.insert(it2.first);

    std::set_intersection(keys1.begin(), keys1.end(), keys2.begin(), keys2.end(), std::back_inserter(*outKeyList));
}


void Archive::getKeyList(std::vector<std::string> * out) const
{
    GP_ASSERT(out);

    out->clear();
    for (const auto& it : _values)
        out->push_back(it.first);
}

void debugPrintVariant(const VariantType& v, int ident)
{
    switch (v.getType())
    {
    case VariantType::TYPE_NONE:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "None\n");
        break;
    case VariantType::TYPE_BOOLEAN:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, v.get<bool>() ? "True\n" : "False\n");
        break;
    case VariantType::TYPE_INT32:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(int32) %d\n", v.get<int32_t>());
        break;
    case VariantType::TYPE_FLOAT:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(float) %f\n", v.get<float>());
        break;
    case VariantType::TYPE_STRING:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(string) %s\n", v.get<std::string>().c_str());
        break;
    case VariantType::TYPE_WIDE_STRING:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(wstring) %s\n", v.get<std::wstring>().c_str());
        break;
    case VariantType::TYPE_BYTE_ARRAY:
    {
        uint32_t size = 0;
        v.getBlob(&size);
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(blob) %u bytes\n", size);
    }
    break;
    case VariantType::TYPE_UINT32:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(uint32) %u\n", v.get<uint32_t>());
        break;
    case VariantType::TYPE_KEYED_ARCHIVE:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(archive)\n");
        v.getArchive()->debugPrint(ident + 2);
        break;
    case VariantType::TYPE_INT64:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(int64) %ld\n", v.get<int64_t>());
        break;
    case VariantType::TYPE_UINT64:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(uint64) %lu\n", v.get<uint64_t>());
        break;
    case VariantType::TYPE_VECTOR2:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(vec2) %f %f\n", v.get<gameplay::Vector2>().x, v.get<gameplay::Vector2>().y);
        break;
    case VariantType::TYPE_VECTOR3:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(vec3) %f %f %f\n", v.get<gameplay::Vector3>().x, v.get<gameplay::Vector3>().y, v.get<gameplay::Vector3>().z);
        break;
    case VariantType::TYPE_VECTOR4:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(vec4) %f %f %f %f\n", v.get<gameplay::Vector4>().x, v.get<gameplay::Vector4>().y, v.get<gameplay::Vector4>().z, v.get<gameplay::Vector4>().w);
        break;
    case VariantType::TYPE_MATRIX2:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(mat2)\n");
        break;
    case VariantType::TYPE_MATRIX3:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(mat3)\n");
        break;
    case VariantType::TYPE_MATRIX4:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(mat4)\n");
        break;
    case VariantType::TYPE_COLOR:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(color)\n");
        break;
    case VariantType::TYPE_FASTNAME:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(fastname)\n");
        break;
    case VariantType::TYPE_AABBOX3:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(aabb)\n");
        break;
    case VariantType::TYPE_FILEPATH:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(path)\n");
        break;
    case VariantType::TYPE_FLOAT64:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(double) %f\n", v.get<double>());
        break;
    case VariantType::TYPE_INT8:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(int8) %d\n", v.get<int8_t>());
        break;
    case VariantType::TYPE_UINT8:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(uint8) %d\n", v.get<uint8_t>());
        break;
    case VariantType::TYPE_INT16:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(int16) %d\n", v.get<int16_t>());
        break;
    case VariantType::TYPE_UINT16:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(uint16) %d\n", v.get<uint16_t>());
        break;
    case VariantType::TYPE_UNKNOWN:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(unknown)\n");
        break;
    case VariantType::TYPE_LIST:
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "(list) %d\n", std::distance(v.begin(), v.end()));
        {
            for (auto& it : v)
                debugPrintVariant(it, ident + 2);
        }
        break;
    }
}

void Archive::debugPrint(int ident) const
{
    std::string prefix;
    for (int i = 0; i < ident; i++)
        prefix.push_back(' ');

    for (const auto& it : _values)
    {
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, prefix.c_str());
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "key: %s\n", it.first.c_str());
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, prefix.c_str());
        gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "value: ");

        debugPrintVariant(it.second, ident);
    }
}
#include "pch.h"
#include "archive.h"




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

bool Archive::deserialize(gameplay::Stream * stream)
{
    clear();

    uint8_t header[2];
    if (stream->read(header, 1, 2) != 2)
        return false;

    if (header[0] != 'K' || header[1] != 'A')
        return false;

    uint16_t version;
    if (stream->read(&version, sizeof(version), 1) != 1)
        return false;

    if (version != 1)
        return false;

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
        GP_ASSERT(!"Not implemented yet");
        return false;
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
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_FILEPATH:
        GP_ASSERT(!"Not implemented yet");
        return false;
    default:
        GP_ASSERT(!"Not implemented yet");
    }

    return false;
}

bool Archive::deserializeVariant(gameplay::Stream * stream, VariantType * out)
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
    case VariantType::TYPE_STRING:
        {
            uint32_t len;
            if (stream->read(&len, sizeof(len), 1) != 1)
                return false;
            char * buf = new char[len+1];
            if (stream->read(buf, 1, len) != len)
            {
                SAFE_DELETE(buf);
                return false;
            }
            buf[len] = '\0';
            out->set(std::string(buf));
            SAFE_DELETE(buf);
        }
        return true;
    case VariantType::TYPE_WIDE_STRING:
        {
            uint32_t len;
            if (stream->read(&len, sizeof(len), 1) != 1)
                return false;
            wchar_t * buf = new wchar_t[len+1];
            if (stream->read(buf, sizeof(wchar_t), len) != len)
            {
                SAFE_DELETE(buf);
                return false;
            }
            buf[len] = L'\0';
            out->set(std::wstring(buf));
            SAFE_DELETE(buf);
        }
        return true;
    case VariantType::TYPE_BYTE_ARRAY:
        {
            uint32_t size;
            if (stream->read(&size, sizeof(size), 1) != 1)
                return false;
            uint8_t * buf = new uint8_t[size];
            if (stream->read(buf, 1, size) != size)
            {
                SAFE_DELETE(buf);
                return false;
            }
            out->setBlob(buf, size);
            SAFE_DELETE(buf);
        }
        return true;
    case VariantType::TYPE_KEYED_ARCHIVE:
        GP_ASSERT(!"Not implemented yet");
        return false;
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
    case VariantType::TYPE_FASTNAME:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_AABBOX3:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_FILEPATH:
        GP_ASSERT(!"Not implemented yet");
        return false;
    default:
        GP_ASSERT(!"Not implemented yet");
    }

    return true;
}
#include "pch.h"
#include "variant.h"
#include "archive.h"



VariantType::VariantType()
    : type(TYPE_NONE)
    , pointerValue(nullptr)
{
}

VariantType::~VariantType()
{
    release();
}

VariantType::VariantType(const VariantType& other)
    : type(TYPE_NONE)
    , pointerValue(nullptr)
{
    set(other);
}

VariantType& VariantType::operator= (const VariantType& other)
{
    set(other);
    return *this;
}

void VariantType::setBlob(const void * data, uint32_t size)
{
    if (type == TYPE_BYTE_ARRAY && (pointerValue == NULL && size == 0 || pointerValue != NULL && size == reinterpret_cast<std::vector<uint8_t> *>(pointerValue)->size() && memcmp(&(*reinterpret_cast<std::vector<uint8_t> *>(pointerValue)->begin()), data, size) == 0))
        return;

    const uint8_t * buf = reinterpret_cast<const uint8_t *>(data);
    if (type == TYPE_BYTE_ARRAY)
    {
        std::vector<uint8_t> * src = reinterpret_cast<std::vector<uint8_t> *>(pointerValue);
        if (src && src->size() == size)
        {
            // copy the data inplace without reallocating the vector
            src->assign(buf, buf + size);
            valueChangedSignal(*this);
            return;
        }
    }

    release();
    type = TYPE_BYTE_ARRAY;
    pointerValue = size > 0 ? reinterpret_cast<void *>(new std::vector<uint8_t>(buf, buf + size)) : NULL;

    valueChangedSignal(*this);
}

const uint8_t * VariantType::getBlob(uint32_t * size) const
{
    GP_ASSERT(type == TYPE_BYTE_ARRAY);
    std::vector<uint8_t> * buf = reinterpret_cast<std::vector<uint8_t> *>(pointerValue);

    if (!buf)
    {
        if (size)
            *size = 0;
        return NULL;
    }

    if (size)
        *size = static_cast<uint32_t>(buf->size());        

    return &(*buf->begin());
}

void VariantType::setArchive(const Archive * archive)
{
    if (!valueValidatorSignal.empty())
    {
        VariantType newValue;
        newValue.setArchive(archive);
        if (!valueValidatorSignal(*this, newValue))
            return;
        if (newValue.type != type)
        {
            set(newValue);
            return;
        }

        release();
        type = TYPE_KEYED_ARCHIVE;
        pointerValue = newValue.pointerValue;   // take ownership
        newValue.pointerValue = nullptr;
        valueChangedSignal(*this);
        return;
    }

    // TODO: make a proper Archive comparison
    if (type == TYPE_KEYED_ARCHIVE && pointerValue == archive)
        return;

    release();
    type = TYPE_KEYED_ARCHIVE;
    pointerValue = archive ? Archive::create(*archive) : Archive::create();
    valueChangedSignal(*this);
}

void VariantType::release()
{
    if (!pointerValue)
        return;

    switch (type)
    {
    case TYPE_VECTOR2:
        SAFE_DELETE(vector2Value);
        return;
    case TYPE_VECTOR3:
        SAFE_DELETE(vector3Value);
        return;
    case TYPE_VECTOR4:
        SAFE_DELETE(vector4Value);
        return;
    case TYPE_MATRIX3:
        SAFE_DELETE(matrix3Value);
        return;
    case TYPE_MATRIX4:
        SAFE_DELETE(matrix4Value);
        return;
    case TYPE_STRING:
        SAFE_DELETE(stringValue);
        return;
    case TYPE_WIDE_STRING:
        SAFE_DELETE(wideStringValue);
        return;
    case TYPE_BYTE_ARRAY:
        delete reinterpret_cast<std::vector<uint8_t> *>(pointerValue);
        pointerValue = NULL;
        return;
    case TYPE_KEYED_ARCHIVE:
        delete reinterpret_cast<class Archive *>(pointerValue);
        pointerValue = NULL;
        return;
    case TYPE_LIST:
        delete reinterpret_cast<std::vector<VariantType>*>(pointerValue);
        pointerValue = NULL;
        return;
    case TYPE_AABBOX3:
        delete reinterpret_cast<gameplay::BoundingBox *>(pointerValue);
        pointerValue = NULL;
        return;
    default:
        // do nothing, it's not an error to get here
        break;
    }
}




//
// Unpickler
//

/*
 * Pickle opcodes.  These must be kept in synch with pickle.py.  Extensive
 * docs are in pickletools.py.
 */
#define MARK        '('
#define STOP        '.'
#define POP         '0'
#define POP_MARK    '1'
#define DUP         '2'
#define FLOAT       'F'
#define BINFLOAT    'G'
#define INT         'I'
#define BININT      'J'
#define BININT1     'K'
#define LONG        'L'
#define BININT2     'M'
#define NONE        'N'
#define PERSID      'P'
#define BINPERSID   'Q'
#define REDUCE      'R'
#define STRING      'S'
#define BINSTRING   'T'
#define SHORT_BINSTRING 'U'
#define UNICODE     'V'
#define BINUNICODE  'X'
#define APPEND      'a'
#define BUILD       'b'
#define GLOBAL      'c'
#define DICT        'd'
#define EMPTY_DICT  '}'
#define APPENDS     'e'
#define GET         'g'
#define BINGET      'h'
#define INST        'i'
#define LONG_BINGET 'j'
#define LIST        'l'
#define EMPTY_LIST  ']'
#define OBJ         'o'
#define PUT         'p'
#define BINPUT      'q'
#define LONG_BINPUT 'r'
#define SETITEM     's'
#define TUPLE       't'
#define EMPTY_TUPLE ')'
#define SETITEMS    'u'

/* Protocol 2. */
#define PROTO    '\x80' /* identify pickle protocol */
#define NEWOBJ   '\x81' /* build object by applying cls.__new__ to argtuple */
#define EXT1     '\x82' /* push object from extension registry; 1-byte index */
#define EXT2     '\x83' /* ditto, but 2-byte index */
#define EXT4     '\x84' /* ditto, but 4-byte index */
#define TUPLE1   '\x85' /* build 1-tuple from stack top */
#define TUPLE2   '\x86' /* build 2-tuple from two topmost stack items */
#define TUPLE3   '\x87' /* build 3-tuple from three topmost stack items */
#define NEWTRUE  '\x88' /* push True */
#define NEWFALSE '\x89' /* push False */
#define LONG1    '\x8a' /* push long from < 256 bytes */
#define LONG4    '\x8b' /* push really big long */

// workaround for android gnustl_static which doesn't support std::to_string methods
#if defined(__ANDROID__) && defined(_GLIBCXX_CSTDLIB)
#include <string>
#include <sstream>

namespace std{
template <typename T>
std::string to_string(T value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}
}
#endif

bool VariantType::unpickle(gameplay::Stream * stream)
{
    bool stopped = false;
    std::vector<VariantType> stack;
    char str[1024];
    std::unordered_map<std::string, VariantType> memo;
    std::vector<size_t> marks;

    stack.reserve(128);

    while (!stream->eof() && !stopped)
    {
        char key;
        if (stream->read(&key, sizeof(key), 1) != 1)
            return false;

        switch (key)
        {
        case PROTO:
            {
                uint8_t version;
                if (stream->read(&version, sizeof(version), 1) != 1)
                    return false;

                if (version > 2)
                {
                    GP_WARN("Unsupported pickle protocol %d", version);
                    return false;
                }
            }
            break;
        case PERSID:
            if (stream->readLine(str, 1024) == NULL)
                return false;

            // do nothing with persistent objects, just push None on stack
            stack.push_back(VariantType());
            break;
        case BINPERSID:
            GP_ASSERT(!stack.empty() && stack.back().getType() == TYPE_INT32);
            if (stack.empty() || stack.back().getType() != TYPE_INT32)
                return false;

            // do nothing with persistent objects, just push None on stack
            stack.back().clear();
            break;
        case NONE:
            stack.push_back(VariantType());
            break;
        case NEWFALSE:
            stack.push_back(VariantType(false));
            break;
        case NEWTRUE:
            stack.push_back(VariantType(true));
            break;
        case INT:
            if (stream->readLine(str, 1024) == NULL)
                return false;
            if (strlen(str) == 3)
            {
                if (str[1] == '0' && str[2] == '0')
                {
                    stack.push_back(VariantType(false));
                    break;
                }
                else if (str[1] == '0' && str[2] == '1')
                {
                    stack.push_back(VariantType(true));
                    break;
                }
            }
            stack.push_back(VariantType((int64_t)atoll(str)));
            break;
        case BININT:
            {
                int32_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;
                stack.push_back(VariantType(val));
            }
            break;
        case BININT1:
            {
                int8_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;
                stack.push_back(VariantType(val));
            }
            break;
        case BININT2:
            {
                int16_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;
                stack.push_back(VariantType(val));
            }
            break;
        case LONG:
            {
                if (stream->readLine(str, 1024) == NULL)
                    return false;
                stack.push_back(VariantType((int64_t)atoll(str)));
            }
            break;
        case LONG1:
            {
                int8_t n;
                if (stream->read(&n, sizeof(n), 1) != 1)
                    return false;
                // we don't actually support python's long type if it's larger than 64bits
                if (n > 8)
                {
                    stream->seek(n, SEEK_CUR);
                    stack.push_back(VariantType((int64_t)0));
                    break;
                }

                int64_t val = 0;
                if (stream->read(&val, 1, n) != n)
                    return false;
                stack.push_back(VariantType(val));
            }
            break;
        case LONG4:
            {
                int16_t n;
                if (stream->read(&n, sizeof(n), 1) != 1)
                    return false;
                // we don't actually support python's long type if it's larger than 64bits
                if (n > 8)
                {
                    stream->seek(n, SEEK_CUR);
                    stack.push_back(VariantType((int64_t)0));
                    break;
                }

                int64_t val = 0;
                if (stream->read(&val, 1, n) != n)
                    return false;
                stack.push_back(VariantType(val));
            }
            break;
        case FLOAT:
            {
                if (stream->readLine(str, 1024) == NULL)
                    return false;
                double value;
                fast_float::from_chars(str, str + strlen(str), value);
                stack.push_back(VariantType(value));
            }
            break;
        case BINFLOAT:
            {
                // BINFLOAT stores doubles in big-endian format
                union
                {
                    double val;
                    int8_t b[8];
                } val;

                if (stream->read(&val.val, sizeof(val), 1) != 1)
                    return false;
                std::swap(val.b[0], val.b[7]);
                std::swap(val.b[1], val.b[6]);
                std::swap(val.b[2], val.b[5]);
                std::swap(val.b[3], val.b[4]);
                stack.push_back(VariantType(val.val));
            }
            break;
        case STRING:
            // we don't support quoted strings
            if (stream->readLine(str, 1024) == NULL)
                return false;
            stack.push_back(VariantType(std::string(str)));
            break;
        case BINSTRING:
            {
                uint32_t len;
                if (stream->read(&len, sizeof(len), 1) != 1)
                    return false;
                std::unique_ptr<char[]> buf(new char[len]);
                if (stream->read(buf.get(), 1, len) != len)
                    return false;
                stack.push_back(VariantType(std::string(buf.get(), len)));
            }
            break;
        case UNICODE:
            // we don't support unicode strings in text format
            stack.push_back(VariantType(std::wstring(L"<unsupported>")));
            break;
        case BINUNICODE:
            {
                // BINUNICODE uses utf8

                uint32_t len;
                if (stream->read(&len, sizeof(len), 1) != 1)
                    return false;
                std::unique_ptr<char[]> buf(new char[len]);
                if (stream->read(buf.get(), 1, len) != len)
                    return false;
                stack.push_back(VariantType(Utils::UTF8ToWCS(std::string(buf.get(), len))));
            }
            break;
        case SHORT_BINSTRING:
            {
                uint8_t len;
                if (stream->read(&len, sizeof(len), 1) != 1)
                    return false;
                std::unique_ptr<char[]> buf(new char[len + 1]);
                if (stream->read(buf.get(), 1, len) != len)
                    return false;
                buf[len] = '\0';

                stack.push_back(VariantType(std::string(buf.get(), len)));
            }
            break;
        case TUPLE:
            GP_ASSERT(!marks.empty() && marks.back() < stack.size());
            if (marks.empty() || marks.back() >= stack.size())
                return false;
            stack[marks.back()].set(stack.begin() + marks.back() + 1, stack.end());
            stack.resize(marks.back()+1);
            marks.pop_back();
            break;
        case EMPTY_TUPLE:
        case EMPTY_LIST:
            stack.push_back(VariantType((int *)0, (int *)0));
            break;
        case TUPLE1:
            stack[stack.size() - 1].set(stack.begin() + stack.size() - 1, stack.end());
            break;
        case TUPLE2:
            stack[stack.size() - 2].set(stack.begin() + stack.size() - 2, stack.end());
            stack.pop_back();
            break;
        case TUPLE3:
            stack[stack.size() - 3].set(stack.begin() + stack.size() - 3, stack.end());
            stack.pop_back();
            stack.pop_back();
            break;
        case EMPTY_DICT:
            {
                stack.push_back(VariantType());
                stack.back().setArchive();
            }
            break;
        case LIST:
            GP_ASSERT(!marks.empty() && marks.back() < stack.size());
            if (marks.empty() || marks.back() >= stack.size())
                return false;
            stack[marks.back()].set(stack.begin() + marks.back() + 1, stack.end());
            stack.resize(marks.back() +1);
            marks.pop_back();
            break;
        case DICT:
            {
                GP_ASSERT(!marks.empty() && marks.back() < stack.size());
                if (marks.empty() || marks.back() >= stack.size())
                    return false;

                stack[marks.back()].setArchive();
                for (size_t i = marks.back() + 1; i < stack.size(); i += 2)
                    stack[marks.back()].getArchive()->set(stack[i].get<std::string>().c_str(), stack[i + 1]);
                stack.resize(marks.back() +1);
                marks.pop_back();
            }
            break;
        case INST:
            // we don't support INST opcode, instead we store all objects creation arguments as a list
            {
                std::string module, name;

                if (stream->readLine(str, 1024) == NULL)
                    return false;
                module.assign(str, strlen(str) - 1);
        
                if (stream->readLine(str, 1024) == NULL)
                    return false;
                name.assign(str, strlen(str) - 1);

                GP_ASSERT(!marks.empty() && marks.back() < stack.size());
                if (marks.empty() || marks.back() >= stack.size())
                    return false;

                std::vector<VariantType> args;
                args.push_back(VariantType(module));
                args.push_back(VariantType(name));
                std::copy(stack.begin() + marks.back() + 1, stack.end(), std::back_inserter(args));
                stack[marks.back()].set(args.begin(), args.end());
                stack.resize(marks.back() +1);
                marks.pop_back();
            }
            break;
        case OBJ:
            // we don't support OBJ opcode, instead we store all objects creation arguments as a list
            GP_ASSERT(!marks.empty() && marks.back() < stack.size());
            if (marks.empty() || marks.back() >= stack.size())
                return false;

            stack[marks.back()].set(stack.begin() + marks.back() + 1, stack.end());
            stack.resize(marks.back() +1);
            marks.pop_back();
            break;
        case NEWOBJ:
            stack[stack.size() - 2].set(stack.begin() + stack.size() - 2, stack.end());
            break;
        case GLOBAL:
            {
                std::string module, name;

                if (stream->readLine(str, 1024) == NULL)
                    return false;
                module.assign(str, strlen(str) - 1);
        
                if (stream->readLine(str, 1024) == NULL)
                    return false;
                name.assign(str, strlen(str) - 1);

                std::vector<VariantType> args;
                args.push_back(VariantType(module));
                args.push_back(VariantType(name));
                stack.push_back(VariantType(args.begin(), args.end()));
            }
            break;
        case EXT1:
            {
                int8_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;
                stack.push_back(VariantType());
            }
            break;
        case EXT2:
            {
                int16_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;
                stack.push_back(VariantType());
            }
            break;
        case EXT4:
            {
                int32_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;
                stack.push_back(VariantType());
            }
            break;
        case REDUCE:
            stack[stack.size() - 2].set(stack.begin() + stack.size() - 2, stack.end());
            stack.pop_back();
            break;
        case POP:
            stack.pop_back();
            break;
        case POP_MARK:
            GP_ASSERT(!marks.empty() && marks.back() < stack.size());
            if (marks.empty() || marks.back() >= stack.size())
                return false;
            stack.resize(marks.back());
            marks.pop_back();
            break;
        case DUP:
            stack.push_back(stack.back());
            break;
        case GET:
            if (stream->readLine(str, 1024) == NULL)
                return false;
            if (memo.find(str) == memo.end())
                return false;
            stack.push_back(memo[str]);
            break;
        case BINGET:
            {
                int8_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;

                std::string s = std::to_string(val);
                if (memo.find(s) == memo.end())
                    return false;
                stack.push_back(memo[s]);
            }
            break;
        case LONG_BINGET:
            {
                int32_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;

                std::string s = std::to_string(val);
                if (memo.find(s) == memo.end())
                    return false;
                stack.push_back(memo[s]);
            }
            break;
        case PUT:
            if (stream->readLine(str, 1024) == NULL)
                return false;
            memo[str] = stack.back();
            break;
        case BINPUT:
            {
                int8_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;

                std::string s = std::to_string(val);
                if (stack.empty())
                    return false;
                memo[s] = stack.back();
            }
            break;
        case LONG_BINPUT:
            {
                int32_t val;
                if (stream->read(&val, sizeof(val), 1) != 1)
                    return false;

                std::string s = std::to_string(val);
                if (stack.empty())
                    return false;
                memo[s] = stack.back();
            }
            break;
        case APPEND:
            {
                std::vector<VariantType> list;
                std::copy(stack[stack.size() - 2].begin(), stack[stack.size() - 2].end(), std::back_inserter(list));
                list.push_back(stack.back());
                stack.pop_back();
                stack.back().set(list.begin(), list.end());
            }
            break;
        case APPENDS:
            {
            GP_ASSERT(!marks.empty() && marks.back() < stack.size());
            if (marks.empty() || marks.back() >= stack.size())
                return false;

                std::vector<VariantType> list;
                std::copy(stack[marks.back() - 1].begin(), stack[marks.back() - 1].end(), std::back_inserter(list));
                std::copy(stack.begin() + marks.back() + 1, stack.end(), std::back_inserter(list));
                stack[marks.back() - 1].set(list.begin(), list.end());

                stack.resize(marks.back());
                marks.pop_back();
            }
            break;
        case SETITEM:
            if (stack[stack.size() - 2].getType() == VariantType::TYPE_STRING)
                stack[stack.size() - 3].getArchive()->set(stack[stack.size() - 2].get<std::string>().c_str(), stack[stack.size() - 1]);
            else
                stack[stack.size() - 3].getArchive()->set(std::to_string(stack[stack.size() - 2].get<int32_t>()).c_str(), stack[stack.size() - 1]);
            stack.pop_back();
            stack.pop_back();
            break;
        case SETITEMS:
            {
                GP_ASSERT(!marks.empty() && marks.back() < stack.size());
                if (marks.empty() || marks.back() >= stack.size())
                    return false;

                for (size_t i = marks.back() + 1; i < stack.size(); i += 2)
                {
                    if (stack[i].getType() == VariantType::TYPE_STRING)
                        stack[marks.back() - 1].getArchive()->set(stack[i].get<std::string>().c_str(), stack[i + 1]);
                    else if (stack[i].getType() == VariantType::TYPE_WIDE_STRING)
                        // store keys in utf8 encoding
                        stack[marks.back() - 1].getArchive()->set(Utils::WCSToUTF8(stack[i].get<std::wstring>()).c_str(), stack[i + 1]);
                    else
                        stack[marks.back() - 1].getArchive()->set(std::to_string(stack[i].get<int64_t>()).c_str(), stack[i + 1]);
                }

                stack.resize(marks.back());
                marks.pop_back();
            }
            break;
        case BUILD:
            stack.pop_back();
            break;
        case MARK:
            marks.push_back(stack.size());
            stack.push_back(VariantType());
            break;
        case STOP:
            stopped = true;
            set(stack.back());
            stack.pop_back();
            GP_ASSERT(stack.empty());
            break;
        default:
            GP_WARN("Unsupported pickle opcode 0x%X", key);
            return false;
        }
    }

    return true;
}

bool VariantType::serializeToJSON(std::string * outStr) const
{
    const VariantType& value = *this;
    VariantType::Type type = value.getType();

    switch (type)
    {
    case VariantType::TYPE_NONE:
        *outStr += "null";
        break;
    case VariantType::TYPE_BOOLEAN:
        if (value.get<bool>())
            *outStr += "true";
        else
            *outStr += "false";
        break;
    case VariantType::TYPE_INT8:
        *outStr += fmt::format("{}", value.get<int8_t>());
        break;
    case VariantType::TYPE_UINT8:
        *outStr += fmt::format("{}", value.get<uint8_t>());
        break;
    case VariantType::TYPE_INT16:
        *outStr += fmt::format("{}", value.get<int16_t>());
        break;
    case VariantType::TYPE_UINT16:
        *outStr += fmt::format("{}", value.get<uint16_t>());
        break;
    case VariantType::TYPE_INT32:
        *outStr += fmt::format("{}", value.get<int32_t>());
        break;
    case VariantType::TYPE_UINT32:
        *outStr += fmt::format("{}", value.get<uint32_t>());
        break;
    case VariantType::TYPE_INT64:
        *outStr += fmt::format("{}", value.get<int64_t>());
        break;
    case VariantType::TYPE_UINT64:
        *outStr += fmt::format("{}", value.get<uint64_t>());
        break;
    case VariantType::TYPE_FLOAT:
        *outStr += fmt::format("{}", value.get<float>());
        break;
    case VariantType::TYPE_FLOAT64:
        *outStr += fmt::format("{}", value.get<double>());
        break;
    case VariantType::TYPE_STRING:
        *outStr += '\"';
        *outStr += value.get<std::string>();    // TODO: support escaping of quotes
        *outStr += '\"';
        break;
    case VariantType::TYPE_WIDE_STRING:
        *outStr += '\"';
        *outStr += Utils::WCSToUTF8(value.get<std::wstring>());    // TODO: support escaping of quotes
        *outStr += '\"';
        break;
    case VariantType::TYPE_BYTE_ARRAY:
    {
        uint32_t size;
        const uint8_t * buf = value.getBlob(&size);
        Utils::base64Encode(buf, size, outStr);
    }
    break;
    case VariantType::TYPE_KEYED_ARCHIVE:
        value.getArchive()->serializeToJSON(outStr);
        break;
    case VariantType::TYPE_VECTOR2:
        *outStr += fmt::format("[{}, {}]", value.get<gameplay::Vector2>().x, value.get<gameplay::Vector2>().y);
        break;
    case VariantType::TYPE_VECTOR3:
        *outStr += fmt::format("[{}, {}, {}]", value.get<gameplay::Vector3>().x, value.get<gameplay::Vector3>().y, value.get<gameplay::Vector3>().z);
        break;
    case VariantType::TYPE_VECTOR4:
        *outStr += fmt::format("[{}, {}, {}, {}]", value.get<gameplay::Vector4>().x, value.get<gameplay::Vector4>().y, value.get<gameplay::Vector4>().z, value.get<gameplay::Vector4>().w);
        break;
    case VariantType::TYPE_MATRIX2:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_MATRIX3:
        GP_ASSERT(!"Not implemented yet");
        return false;
    case VariantType::TYPE_MATRIX4:
        GP_ASSERT(!"Not implemented yet");
        return false;
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
    case VariantType::TYPE_LIST:
    {
        uint32_t size = static_cast<uint32_t>(std::distance(value.begin(), value.end()));

        *outStr += '[';
        for (const VariantType& v : value)
        {
            if (!v.serializeToJSON(outStr))
                return false;

            *outStr += ", ";
        }

        if (size)
        {
            outStr->pop_back();
            outStr->pop_back();
        }

        *outStr += ']';
    }
    break;
    default:
        GP_ASSERT(!"Not implemented yet");
        return false;
    }

    return true;
}
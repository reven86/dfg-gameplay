#include "pch.h"
#include "memory_stream.h"




MemoryStream::MemoryStream()
{
}

MemoryStream::~MemoryStream()
{
    close();
}

void MemoryStream::close()
{
    _ownedBuffer.reset();
    _readBuffer = nullptr;
    _writeBuffer = nullptr;
    _cursor = _bufferSize = 0;
    _autoBuffer.clear();
}

MemoryStream * MemoryStream::create(const void * buffer, size_t bufferSize)
{
    if (!buffer)
        return nullptr;

    MemoryStream * res = new MemoryStream();
    res->_readBuffer = reinterpret_cast<const uint8_t *>(buffer);
    res->_writeBuffer = nullptr;
    res->_bufferSize = bufferSize;
    res->_cursor = 0;
    res->_canAllocate = false;

    return res;
}

MemoryStream * MemoryStream::create(void * buffer, size_t bufferSize)
{
    if (!buffer)
        return nullptr;

    MemoryStream * res = new MemoryStream();
    res->_readBuffer = reinterpret_cast<const uint8_t *>(buffer);
    res->_writeBuffer = reinterpret_cast<uint8_t *>(buffer);
    res->_bufferSize = bufferSize;
    res->_cursor = 0;
    res->_canAllocate = false;

    return res;
}

MemoryStream * MemoryStream::create(std::unique_ptr< uint8_t[] >& buffer, size_t bufferSize)
{
    if (!buffer.get())
        return nullptr;

    MemoryStream * res = new MemoryStream();
    res->_ownedBuffer.reset(buffer.release());
    res->_readBuffer = reinterpret_cast<const uint8_t *>(res->_ownedBuffer.get());
    res->_writeBuffer = reinterpret_cast<uint8_t *>(res->_ownedBuffer.get());
    res->_bufferSize = bufferSize;
    res->_cursor = 0;
    res->_canAllocate = false;

    return res;
}

MemoryStream * MemoryStream::create()
{
    MemoryStream * res = new MemoryStream();
    res->_readBuffer = nullptr;
    res->_writeBuffer = nullptr;
    res->_bufferSize = 0;
    res->_cursor = 0;
    res->_canAllocate = true;

    return res;
}

size_t MemoryStream::read(void* ptr, size_t size, size_t count)
{
    if (size == 0)
        return 0;

    size_t maxReadBytes = std::min(_bufferSize - _cursor, size * count);
    size_t maxReadElements = maxReadBytes / size;

    if (maxReadElements == 0)
        return 0;

    maxReadBytes = maxReadElements * size;
    memcpy(ptr, _readBuffer + _cursor, maxReadBytes);
    _cursor += maxReadBytes;
    return maxReadElements;
}

char* MemoryStream::readLine(char* str, int num)
{
    if (num <= 0)
        return NULL;

    size_t maxReadBytes = std::min<size_t>(_bufferSize - _cursor, num);
    char * strSave = str;
    while (maxReadBytes > 0)
    {
        const char& ch = *(_readBuffer + _cursor);
        if (ch == '\r' || ch == '\n')
        {
            *str++ = ch;
            if (str - strSave < num)
                *str++ = '\0';
            return strSave;
        }

        *str++ = ch;
        _cursor++;
        maxReadBytes--;
    }

    if (str - strSave < num)
        *str++ = '\0';
    return strSave;
}

size_t MemoryStream::write(const void* ptr, size_t size, size_t count)
{
    if (!canWrite())
        return 0;

    size_t maxWriteBytes = _canAllocate ? size * count : std::min(_bufferSize - _cursor, size * count);
    size_t maxWriteElements = maxWriteBytes / size;

    if (maxWriteBytes == 0)
        return 0;

    if (_canAllocate && _cursor + maxWriteBytes > _bufferSize)
    {
        _bufferSize = _cursor + maxWriteBytes;
        _autoBuffer.resize(_bufferSize);
        _readBuffer = _writeBuffer = &_autoBuffer.front();
    }

    maxWriteBytes = maxWriteElements * size;
    memcpy(_writeBuffer + _cursor, ptr, maxWriteBytes);
    _cursor += maxWriteBytes;
    return maxWriteElements;
}

bool MemoryStream::seek(long int offset, int origin)
{
    switch (origin)
    {
    case SEEK_SET:
        _cursor = static_cast<size_t>(offset);
        break;
    case SEEK_CUR:
        _cursor = static_cast<size_t>(position() + offset);
        break;
    case SEEK_END:
        _cursor = static_cast<size_t>(static_cast<long int>(_bufferSize)+offset);
        break;
    }

    GP_ASSERT(_cursor <= _bufferSize);
    return _cursor <= _bufferSize;
}
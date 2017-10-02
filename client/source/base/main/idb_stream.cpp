#include "pch.h"
#include "idb_stream.h"

#ifdef __EMSCRIPTEN__




IndexedDBStream::IndexedDBStream()
    : _buffer(NULL)
{
}

IndexedDBStream::~IndexedDBStream()
{
    close();
}

gameplay::Stream * IndexedDBStream::create(const char * dbName, const char * fileName, size_t streamMode)
{
    IndexedDBStream * res = new ZipStream();
    res->_streamMode = streamMode;
    res->_dbName = dbName;
    res->_fileName = fileName;

    if (streamMode == gameplay::FileSystem::READ)
    {
        int size = 0;
        int error = 0;
        emscripten_idb_load(dbName, fileName, &_buffer, &size, &error);
        
        if (!error)
            res->_underlyingStream.reset(MemoryStream::create(_buffer, static_cast<size_t>(size)));
    }
    else
    {
        res->_underlyingStream.reset(MemoryStream::create());
    }
    
    return res;
}

void IndexedDBStream::close()
{
    // flush the contents to DB if mode is WRITE
    if (_streamMode == gameplay::FileSystem::WRITE)
    {
        int error = 0;
        emscripten_idb_store(_dbName.c_str(), _fileName.c_str(), _underlyingBuffer->getBuffer(), _underlyingBuffer->length(), &error);
    }

    _underlyingStream.reset();
    free(_buffer);
}

size_t IndexedDBStream::read(void* ptr, size_t size, size_t count)
{
    if (!_underlyingStream)
        return 0;

    return _underlyingStream->read(ptr, size, count);
}

char* IndexedDBStream::readLine(char* str, int num)
{
    if (!_underlyingStream)
        return nullptr;

    return _underlyingStream->readLine(str, num);
}

size_t IndexedDBStream::write(const void* ptr, size_t size, size_t count)
{
    if (!_underlyingStream)
        return 0;

    return _underlyingStream->write(ptr, size, count);
}

bool IndexedDBStream::seek(long int offset, int origin)
{
    if (!_underlyingStream)
        return false;

    return _underlyingStream->seek(offset, origin);
}

bool IndexedDBStream::rewind()
{
    if (!_underlyingStream)
        return false;

    return _underlyingStream->rewind();
}


#endif // __EMSCRIPTEN__
#include "pch.h"
#include "idb_stream.h"

#ifdef __EMSCRIPTEN__



struct StreamState
{
    std::string dbName;
    std::string filename;
    std::function<void(gameplay::Stream*)> callback;
};

void onStoreStateless(void * arg)
{
    StreamState * stream = reinterpret_cast<StreamState*>(arg);
    if (stream->callback)
        stream->callback(NULL);

    SAFE_DELETE(stream);
}

void onErrorStateless(void * arg)
{
    StreamState * stream = reinterpret_cast<StreamState*>(arg);

    GP_WARN("Failed to %s file %s from IndexedDB %s", "save", stream->filename.c_str(), stream->dbName.c_str());

    if (stream->callback)
        stream->callback(NULL);

    SAFE_DELETE(stream);
}






IndexedDBStream::IndexedDBStream()
{
}

IndexedDBStream::~IndexedDBStream()
{
    // here we need to flush all data to indexed db same way it's done in close method
    // except we can't rely on object passed to callback since it will be destroyd as
    // we exit this function

    // flush the contents to DB if mode is WRITE
    if (_streamMode == gameplay::FileSystem::WRITE && _underlyingStream)
    {
        StreamState * state = new StreamState{ _dbName, _fileName, _callback };
        emscripten_idb_async_store(_dbName.c_str(), _fileName.c_str(), (void*) _underlyingStream->getBuffer(), _underlyingStream->length(), state, &onStoreStateless, &onErrorStateless);
    }
}

void IndexedDBStream::onLoad(void * arg, void * buf, int size)
{
    IndexedDBStream * stream = reinterpret_cast<IndexedDBStream*>(arg);

    std::unique_ptr<uint8_t[]> contents(new uint8_t[size]);
    memcpy(contents.get(), buf, size);

    stream->_underlyingStream.reset(MemoryStream::create(contents, size));
    if (stream->_callback)
        stream->_callback(stream);

    SAFE_RELEASE(stream);
}

void IndexedDBStream::onStore(void * arg)
{
    IndexedDBStream * stream = reinterpret_cast<IndexedDBStream*>(arg);
    if (stream->_callback)
        stream->_callback(stream);

    stream->_underlyingStream.reset();

    SAFE_RELEASE(stream);
}

void IndexedDBStream::onError(void * arg)
{
    IndexedDBStream * stream = reinterpret_cast<IndexedDBStream*>(arg);
    stream->_underlyingStream.reset();

    GP_WARN("Failed to %s file %s from IndexedDB %s", stream->_streamMode == gameplay::FileSystem::READ ? "open" : "save", stream->_fileName.c_str(), stream->_dbName.c_str());

    if (stream->_callback)
        stream->_callback(NULL);

    SAFE_RELEASE(stream);
}

IndexedDBStream * IndexedDBStream::create(const char * dbName, const char * fileName, size_t streamMode, std::function<void(gameplay::Stream*)> callback)
{
    IndexedDBStream * res = new IndexedDBStream();
    res->_streamMode = streamMode;
    res->_dbName = dbName;
    res->_fileName = fileName;
    res->_callback = callback;

    if (streamMode == gameplay::FileSystem::READ)
    {
        res->addRef();
        emscripten_idb_async_load(dbName, fileName, res, &IndexedDBStream::onLoad, &IndexedDBStream::onError);
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
    if (_streamMode == gameplay::FileSystem::WRITE && _underlyingStream)
    {
        addRef();
        emscripten_idb_async_store(_dbName.c_str(), _fileName.c_str(), (void*) _underlyingStream->getBuffer(), _underlyingStream->length(), this, &IndexedDBStream::onStore, &IndexedDBStream::onError);
    }
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
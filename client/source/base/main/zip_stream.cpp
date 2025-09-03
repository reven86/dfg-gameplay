#include "pch.h"
#include "zip_stream.h"
#include "zip_packages.h"
#include <zlib.h>




ZipStream::ZipStream()
{
}

ZipStream::~ZipStream()
{
    close();
}

gameplay::Stream * ZipStream::create(const char * packageName, const char * fileName)
{
    if (packageName == NULL || *packageName == '\0')
        return gameplay::FileSystem::open(fileName, gameplay::FileSystem::READ);
    
    ZipPackage * package = ZipPackagesCache::findOrOpenPackage(packageName);
    if (!package)
        return NULL;

    gameplay::Stream * res = package->open(fileName);
    if (!res)
        GP_WARN("Can't read file %s:%s", packageName, fileName);

    return res;
}

ZipStream * ZipStream::create(gameplay::Stream * compressedStream)
{
    // TODO: it not very efficient at the moment since stream is fully read and decompressed in memory
    // it would be more efficient to decompress by chunks on demand

    if (!compressedStream)
        return NULL;

    size_t compressedLength = compressedStream->length() - compressedStream->position();
    std::unique_ptr<uint8_t[]> compressedData(new uint8_t[compressedLength]);

    if (compressedStream->read(compressedData.get(), 1, compressedLength) != compressedLength)
        return NULL;

    return ZipStream::create(compressedData.get(), compressedLength);
}

ZipStream * ZipStream::create(const void * buffer, size_t bufferSize)
{
    z_stream strm =
    {
        (unsigned char*)(buffer),
        static_cast<uInt>(bufferSize)
    };

    int ret = inflateInit(&strm);

    if (ret != Z_OK)
    {
        GP_WARN("Can't decompress the stream.");
        return NULL;
    }

    const int chunk = 1 * 1024 * 1024;
    std::unique_ptr<uint8_t[]> out(new uint8_t[chunk]);

    ZipStream * res = new ZipStream();
    res->_underlyingStream.reset(MemoryStream::create());

    do
    {
        strm.avail_out = chunk;
        strm.next_out = out.get();
        ret = inflate(&strm, Z_NO_FLUSH);

        assert(ret != Z_STREAM_ERROR);
        switch (ret)
        {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
        }

        int have = chunk - strm.avail_out;

        if (res->_underlyingStream->write(out.get(), 1, have) != have)
            GP_WARN("Can't write decompressed data.");

    } while (strm.avail_out == 0);

    (void)inflateEnd(&strm);

    if (ret != Z_STREAM_END && ret != Z_OK)
        ;// GP_WARN("Error while decompressing the stream.");

    res->_underlyingStream->seek(0, SEEK_SET);
    return res;
}

void ZipStream::close()
{
    _underlyingStream.reset();
}

size_t ZipStream::read(void* ptr, size_t size, size_t count)
{
    if (!_underlyingStream)
        return 0;

    return _underlyingStream->read(ptr, size, count);
}

char* ZipStream::readLine(char* str, int num)
{
    if (!_underlyingStream)
        return nullptr;

    return _underlyingStream->readLine(str, num);
}

size_t ZipStream::write(const void* ptr, size_t size, size_t count)
{
    if (!_underlyingStream)
        return 0;

    return _underlyingStream->write(ptr, size, count);
}

bool ZipStream::seek(long int offset, int origin)
{
    if (!_underlyingStream)
        return false;

    return _underlyingStream->seek(offset, origin);
}

bool ZipStream::rewind()
{
    if (!_underlyingStream)
        return false;

    return _underlyingStream->rewind();
}
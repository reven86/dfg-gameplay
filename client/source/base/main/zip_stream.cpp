#include "pch.h"
#include "zip_stream.h"
#include "zip_packages.h"




ZipStream::ZipStream()
{
}

ZipStream::~ZipStream()
{
}

ZipStream * ZipStream::create(const char * packageName, const char * fileName)
{
    zip * package = ZipPackagesCache::findOrOpenPackage(packageName);
    if (!package)
        return NULL;

    if (zip_name_locate(package, fileName, ZIP_FL_ENC_GUESS | ZIP_FL_NOCASE) < 0)
        return NULL;

    //Search for the file of given name
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(package, fileName, ZIP_FL_NOCASE, &st);

    //Read the compressed file
    zip_file *f = zip_fopen(package, fileName, ZIP_FL_NOCASE);
    if (!f)
        return NULL;

    //Alloc memory for its uncompressed contents
    ZipStream * res = new ZipStream();
    res->_fileContent.reset(new uint8_t[st.size]);

    zip_fread(f, res->_fileContent.get(), st.size);
    zip_fclose(f);

    res->_underlyingStream.reset(MemoryStream::create(res->_fileContent.get(), st.size));

    return res;
}

void ZipStream::close()
{
    _underlyingStream.reset();
    _fileContent.reset();
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
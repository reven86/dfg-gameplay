#include "pch.h"
#include "zip_stream.h"
#include "zip_packages.h"




std::mutex ZipStream::_zipReadMutex;

ZipStream::ZipStream()
{
}

ZipStream::~ZipStream()
{
}

gameplay::Stream * ZipStream::create(const char * packageName, const char * fileName)
{
    if (packageName == NULL || *packageName == '\0')
        return gameplay::FileSystem::open(fileName, gameplay::FileSystem::READ);
    
    zip * package = ZipPackagesCache::findOrOpenPackage(packageName);
    if (!package)
        return NULL;

    //Search for the file of given name
    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat(package, fileName, ZIP_FL_NOCASE, &st) != 0)
        return NULL;

    // make sure we access any zip file only from one thread
    // hint: it would be more convinient to use one mutex per 
    // zip * structure, not one mutex for all zips
    std::unique_lock<std::mutex> guard(_zipReadMutex);

    //Read the compressed file
    zip_file *f = zip_fopen(package, fileName, ZIP_FL_NOCASE);
    if (!f)
        return NULL;

    //Alloc memory for its uncompressed contents
    ZipStream * res = new ZipStream();
    res->_fileContent.reset(new uint8_t[st.size]);

    if (zip_fread(f, res->_fileContent.get(), st.size) != (int)st.size)
        GP_WARN("Can't read file %s:%s", packageName, fileName);
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
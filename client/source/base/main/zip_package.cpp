#include "pch.h"
#include "zip_package.h"
#include "memory_stream.h"




ZipPackage::ZipPackage(struct zip * zipFile)
{
    _zipFile.reset(zipFile, zip_close);
}

ZipPackage::~ZipPackage()
{
}

ZipPackage * ZipPackage::create(const char * zipFile)
{
    int err = 0;
    zip * res = zip_open(zipFile, ZIP_RDONLY, &err);

    return res == NULL ? nullptr : new ZipPackage(res);
}

gameplay::Stream * ZipPackage::open(const char * path, size_t streamMode)
{
    if (streamMode != gameplay::FileSystem::READ)
        return NULL;

    std::string fullPath = gameplay::FileSystem::resolvePath(path);

    //Search for the file of given name
    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat(_zipFile.get(), fullPath.c_str(), ZIP_FL_NOCASE, &st) != 0)
        return NULL;

    std::unique_lock<std::mutex> lock(_zipMutex);

    //Alloc memory for its uncompressed contents
    size_t fileSize = static_cast<size_t>(st.size);
    std::unique_ptr< uint8_t[] >contents(new uint8_t[fileSize]);

    //Read the compressed file
    zip_file *f = zip_fopen(_zipFile.get(), fullPath.c_str(), ZIP_FL_NOCASE);
    if (!f)
        return NULL;

    if (zip_fread(f, contents.get(), fileSize) != (int)fileSize)
    {
        GP_WARN("Can't read from zip package, file %s", path);
    }
    zip_fclose(f);

    return MemoryStream::create(contents, fileSize);
}

bool ZipPackage::fileExists(const char * path)
{
    std::string fullPath = gameplay::FileSystem::resolvePath(path);

    struct zip_stat st;
    zip_stat_init(&st);
    return zip_stat(_zipFile.get(), fullPath.c_str(), ZIP_FL_NOCASE, &st) == 0;
}

void ZipPackage::setPassword(const char * password)
{
    GP_ASSERT(_zipFile.get());
    zip_set_default_password(_zipFile.get(), password);
}
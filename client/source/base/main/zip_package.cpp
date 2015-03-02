#include "pch.h"
#include "zip_package.h"
#include "memory_stream.h"
#include <zip.h>




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
    zip * res = zip_open(zipFile, 0, &err);

    return res == NULL ? nullptr : new ZipPackage(res);
}

gameplay::Stream * ZipPackage::open(const char * path, size_t streamMode)
{
    std::string fullPath = gameplay::FileSystem::resolvePath(path);

    //Search for the file of given name
    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat(_zipFile.get(), fullPath.c_str(), 0, &st) != 0)
    {
        GP_WARN("Can't open file %s", path);
        return false;
    }

    //Alloc memory for its uncompressed contents
    size_t fileSize = static_cast< size_t >(st.size);
    std::unique_ptr< uint8_t[] >contents (new uint8_t[fileSize]);

    //Read the compressed file
    zip_file *f = zip_fopen(_zipFile.get(), fullPath.c_str(), 0);
    if (!f)
    {
        GP_WARN("Can't open file %s", path);
        return false;
    }
    zip_fread(f, contents.get(), fileSize);
    zip_fclose(f);

    return MemoryStream::create(contents, fileSize);
}

bool ZipPackage::fileExists(const char * path)
{
    std::string fullPath = gameplay::FileSystem::resolvePath(path);

    struct zip_stat st;
    zip_stat_init(&st);
    return zip_stat(_zipFile.get(), fullPath.c_str(), 0, &st) == 0;
}
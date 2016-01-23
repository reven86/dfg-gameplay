#include "pch.h"
#include "zip_package.h"
#include "zip_packages.h"
#include "zip_stream.h"




ZipPackage::ZipPackage(const char * packageName)
    : _packageName(packageName)
{
}

ZipPackage::~ZipPackage()
{
}

ZipPackage * ZipPackage::create(const char * zipFile)
{
    if (!gameplay::FileSystem::fileExists(zipFile))
        return NULL;

    return new ZipPackage(zipFile);
}

gameplay::Stream * ZipPackage::open(const char * path, size_t streamMode)
{
    if (streamMode != gameplay::FileSystem::READ)
        return NULL;

    return ZipStream::create(_packageName.c_str(), gameplay::FileSystem::resolvePath(path));
}

bool ZipPackage::fileExists(const char * path)
{
    return ZipPackagesCache::hasFile(_packageName.c_str(), gameplay::FileSystem::resolvePath(path));
}

void ZipPackage::setPassword(const char * password)
{
    zip * zipFile = ZipPackagesCache::findOrOpenPackage(_packageName.c_str());
    if (zipFile)
        zip_set_default_password(zipFile, password);
}
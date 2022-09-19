#include "pch.h"
#include "zip_package.h"
#include "zip_packages.h"
#include "zip_stream.h"




ZipPackage::ZipPackage(const char * packageName, bool ignoreCase)
    : _packageName(packageName)
    , _ignoreCase(ignoreCase)
{
}

ZipPackage::~ZipPackage()
{
    gameplay::FileSystem::unregisterPackage(this);
}

ZipPackage * ZipPackage::create(const char * zipFile, bool ignoreCase)
{
    if (!gameplay::FileSystem::fileExists(zipFile))
        return NULL;

    return new ZipPackage(zipFile, ignoreCase);
}

gameplay::Stream * ZipPackage::open(const char * path, size_t streamMode)
{
    if (streamMode != gameplay::FileSystem::READ)
        return NULL;

    return ZipStream::create(_packageName.c_str(), gameplay::FileSystem::resolvePath(path), _ignoreCase);
}

bool ZipPackage::fileExists(const char * path)
{
    return ZipPackagesCache::hasFile(_packageName.c_str(), gameplay::FileSystem::resolvePath(path), _ignoreCase);
}

void ZipPackage::setPassword(const char * password)
{
    zip * zipFile = ZipPackagesCache::findOrOpenPackage(_packageName.c_str());
    if (zipFile)
        zip_set_default_password(zipFile, password);
}
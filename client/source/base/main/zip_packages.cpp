#include "pch.h"
#include "zip_packages.h"




std::unordered_map< std::string, std::shared_ptr< zip > > ZipPackagesCache::_packages;


/**
 * Gets the fully resolved path.
 * If the path is relative then it will be prefixed with the resource path.
 * Aliases will be converted to a relative path.
 * 
 * @param path The path to resolve.
 * @param fullPath The full resolved path. (out param)
 */
static void getFullPath(const char* path, std::string& fullPath)
{
    if (gameplay::FileSystem::isAbsolutePath(path))
    {
        fullPath.assign(path);
    }
    else
    {
        fullPath.assign(gameplay::FileSystem::getResourcePath());
        fullPath += gameplay::FileSystem::resolvePath(path);
    }
}



zip * ZipPackagesCache::findOrOpenPackage(const char * packageName)
{
    if (packageName == NULL || *packageName == '\0')
        return NULL;
    
    auto package = _packages.find(packageName);
    if (package == _packages.end())
    {
#ifndef __ANDROID__
        std::string fullPath;
        getFullPath(packageName, fullPath);
#else
        std::string fullPath(packageName);
#endif

        int err = 0;
        zip * res = zip_open(fullPath.c_str(), 0, &err);

        if (!res)
        {
            GP_WARN("Can't open package %s", packageName);
            return NULL;
        }

        _packages.insert(std::make_pair(std::string(packageName), std::shared_ptr<zip>(res, zip_close)));
        return res;
    }

    return (*package).second.get();
}

bool ZipPackagesCache::hasFile(const char * packageName, const char * filename, bool ignoreCase)
{
    if (!filename || *filename == 0 || filename[strlen(filename) - 1] == '/')   // ignore empty string, for a directory lookup method always returns false
        return false;

    zip * package = findOrOpenPackage(packageName);
    if (!package)
        return false;

    return zip_name_locate(package, filename, (ignoreCase ? ZIP_FL_NOCASE : 0) | ZIP_FL_ENC_RAW) >= 0;
}

void ZipPackagesCache::closePackage(const char * packageName)
{
    if (packageName == NULL || *packageName == '\0')
        return;

    auto package = _packages.find(packageName);
    if (package != _packages.end())
        _packages.erase(package);
}
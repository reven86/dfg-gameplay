#include "pch.h"
#include "zip_packages.h"




std::unordered_map< std::string, std::shared_ptr< zip > > ZipPackagesCache::_packages;


zip * ZipPackagesCache::findOrOpenPackage(const char * packageName)
{
    if (packageName == NULL || *packageName == '\0')
        return NULL;
    
    auto package = _packages.find(packageName);
    if (package == _packages.end())
    {
#ifndef __ANDROID__
        std::string fullPath(gameplay::FileSystem::getResourcePath());
        fullPath += packageName;
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

bool ZipPackagesCache::hasFile(const char * packageName, const char * filename)
{
    zip * package = findOrOpenPackage(packageName);
    if (!package)
        return gameplay::FileSystem::fileExists(filename);

    return zip_name_locate(package, filename, ZIP_FL_ENC_GUESS | ZIP_FL_NOCASE) >= 0;
}
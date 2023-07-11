#include "pch.h"
#include "zip_packages.h"
#include "zip_stream.h"




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




/** An extension to gameplay::FileSystem to be able
 *  to stream resources from zip packages
 */
class ZipPackage : public gameplay::Package, Noncopyable
{
public:
    virtual ~ZipPackage()
    {
        gameplay::FileSystem::unregisterPackage(this);
    }

    static ZipPackage * create(const char * zipFile, bool ignoreCase = false)
    {
        if (!gameplay::FileSystem::fileExists(zipFile))
            return NULL;

#ifndef __ANDROID__
        std::string fullPath;
        getFullPath(zipFile, fullPath);
#else
        std::string fullPath(zipFile);
#endif

        int err = 0;
        zip * res = zip_open(fullPath.c_str(), 0, &err);

        if (!res)
        {
            GP_WARN("Can't open package %s %d", fullPath.c_str(), err);
            return NULL;
        }

        return new ZipPackage(zipFile, res, ignoreCase);
    }

    virtual gameplay::Stream * open(const char * path, size_t streamMode = gameplay::FileSystem::READ) override
    {
        if (streamMode != gameplay::FileSystem::READ)
            return NULL;

        return ZipStream::create(_packageName.c_str(), gameplay::FileSystem::resolvePath(path), _ignoreCase);
    }

    /**
     * Checks if the file at the given path exists.
     *
     * @param filePath The path to the file.
     *
     * @return <code>true</code> if the file exists; <code>false</code> otherwise.
     */
    virtual bool fileExists(const char* path)
    {
        const char * filename = gameplay::FileSystem::resolvePath(path);
        if (!filename || *filename == 0 || filename[strlen(filename) - 1] == '/')   // ignore empty string, for a directory lookup method always returns false
            return false;

        return zip_name_locate(_zip.get(), filename, (_ignoreCase ? ZIP_FL_NOCASE : 0) | ZIP_FL_ENC_RAW) >= 0;
    }

    /**
     * Set password to access zip package content.
     *
     * @param password Password. Set to NULL to unset password.
     */
    void setPassword(const char * password)
    {
        zip_set_default_password(_zip.get(), password);
    }

    zip * getZip()
    {
        return _zip.get();
    }

protected:
    ZipPackage(const char * packageName, zip * zipObject, bool ignoreCase)
        : _packageName(packageName)
        , _ignoreCase(ignoreCase)
    {
        _zip.reset(zipObject, zip_close);

        gameplay::FileSystem::registerPackage(this);
    }

private:
    std::string _packageName;
    bool _ignoreCase;
    std::shared_ptr<zip> _zip;
};



static bool __finilized = false;
std::unordered_map<std::string, std::unique_ptr<ZipPackage>> ZipPackagesCache::__packages;




void ZipPackagesCache::finalize()
{
    GP_ASSERT(__packages.empty());
    __packages.clear();

    __finilized = true;
}

zip * ZipPackagesCache::findOrOpenPackage(const char * packageName, bool ignoreCase)
{
    if (__finilized || packageName == NULL || *packageName == '\0')
        return NULL;
    
    auto package = __packages.find(packageName);
    if (package != __packages.end())
        return (*package).second.get()->getZip();

    ZipPackage * res = ZipPackage::create(packageName, ignoreCase);
    if (!res)
        return NULL;

    __packages.emplace(packageName, res);
    return res->getZip();
}

void ZipPackagesCache::closePackage(const char * packageName)
{
    if (packageName == NULL || *packageName == '\0')
        return;

    auto package = __packages.find(packageName);
    if (package != __packages.end())
        __packages.erase(package);
}

void ZipPackagesCache::setPassword(const char * packageName, const char * password)
{
    auto package = __packages.find(packageName);
    if (package != __packages.end())
        (*package).second->setPassword(password);
}

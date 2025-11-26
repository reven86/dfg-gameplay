#include "pch.h"
#include "zip_packages.h"
#include "zip_stream.h"
#include <zip.h>




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



//
// ZipPackage
//

ZipPackage::ZipPackage(const char* packageName, zip* zipObject)
    : _packageName(packageName)
{
    _zip.reset(zipObject, zip_close);

    zip_int64_t num_entries = zip_get_num_entries(zipObject, 0);
    if (num_entries < 0)
        return;

    for (zip_uint64_t i = 0; i < static_cast<zip_uint64_t>(num_entries); i++)
    {
        const char* name = zip_get_name(zipObject, i, ZIP_FL_ENC_RAW);
        if (!name)
            continue;

        // Skip directory entries (they typically end with '/')
        if (name[strlen(name) - 1] == '/')
            continue;

        // Get file stats
        struct zip_stat st;
        zip_stat_init(&st);
        if (zip_stat_index(zipObject, i, 0, &st) == 0)
        {
            // Convert filename to lowercase
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                [](unsigned char c) { return std::tolower(c); });

            // Store in mapping
            _files[lowerName] = { st.index, st.size };
        }
    }
}

ZipPackage * ZipPackage::create(const char * zipFile)
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

    return new ZipPackage(zipFile, res);
}

gameplay::Stream * ZipPackage::open(const char * path, size_t streamMode)
{
    if (streamMode != gameplay::FileSystem::READ)
        return NULL;

    // make sure we access any zip file only from one thread
    std::unique_lock<std::mutex> guard(_zipReadMutex);

    std::string lowerName = gameplay::FileSystem::resolvePath(path);
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
        [](unsigned char c) { return std::tolower(c); });

    auto it = _files.find(lowerName);
    if (it == _files.end())
        return NULL;

    //Read the compressed file
    zip_file* f = zip_fopen_index(_zip.get(), (*it).second.index, ZIP_FL_ENC_RAW);
    if (!f)
        return NULL;

    //Alloc memory for its uncompressed contents
    std::unique_ptr<uint8_t[]> fileContent(new uint8_t[(*it).second.size]);

    if (zip_fread(f, fileContent.get(), (*it).second.size) != (*it).second.size)
    {
        zip_fclose(f);
        return NULL;
    }

    zip_fclose(f);
    return MemoryStream::create(fileContent, static_cast<size_t>((*it).second.size));
}

bool ZipPackage::fileExists(const char* path)
{
    const char * filename = gameplay::FileSystem::resolvePath(path);
    if (!filename || *filename == 0 || filename[strlen(filename) - 1] == '/')   // ignore empty string, for a directory lookup method always returns false
        return false;

    std::string lowerName = path;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
        [](unsigned char c) { return std::tolower(c); });

    return _files.find(lowerName) != _files.end();
}

void ZipPackage::setPassword(const char * password)
{
    zip_set_default_password(_zip.get(), password);
}




static bool __finilized = false;
std::unordered_map<std::string, std::unique_ptr<ZipPackage>> ZipPackagesCache::__packages;




void ZipPackagesCache::finalize()
{
    GP_ASSERT(__packages.empty());

    for (auto& it : __packages)
        gameplay::FileSystem::unregisterPackage(it.second.get());

    __packages.clear();

    __finilized = true;
}

ZipPackage * ZipPackagesCache::findOrOpenPackage(const char * packageName)
{
    if (__finilized || packageName == NULL || *packageName == '\0')
        return NULL;
    
    auto package = __packages.find(packageName);
    if (package != __packages.end())
        return (*package).second.get();

    ZipPackage * res = ZipPackage::create(packageName);
    if (!res)
        return NULL;

    __packages.emplace(packageName, res);
    gameplay::FileSystem::registerPackage(res);
    return res;
}

void ZipPackagesCache::closePackage(const char * packageName)
{
    if (packageName == NULL || *packageName == '\0')
        return;

    auto package = __packages.find(packageName);
    if (package != __packages.end())
    {
        gameplay::FileSystem::unregisterPackage((*package).second.get());
        __packages.erase(package);
    }
}

void ZipPackagesCache::setPassword(const char * packageName, const char * password)
{
    auto package = __packages.find(packageName);
    if (package != __packages.end())
        (*package).second->setPassword(password);
}

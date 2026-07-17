#include "pch.h"
#include "zip_packages.h"
#include "zip_stream.h"
#include <zip.h>

#ifdef __ANDROID__
#include <android/asset_manager.h>
extern AAssetManager* __assetManager;
#endif




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



#ifdef __ANDROID__

struct AAssetZipContext
{
    AAsset* asset = nullptr;
    zip_uint64_t size = 0;
    zip_uint64_t position = 0;
    zip_error_t error;
};

static zip_int64_t aassetZipSourceCallback(void* userdata, void* data, zip_uint64_t length, zip_source_cmd_t command)
{
    AAssetZipContext* ctx = static_cast<AAssetZipContext*>(userdata);

    switch (command)
    {
    case ZIP_SOURCE_OPEN:
        ctx->position = 0;
        if (AAsset_seek(ctx->asset, 0, SEEK_SET) < 0)
        {
            zip_error_set(&ctx->error, ZIP_ER_SEEK, 0);
            return -1;
        }
        return 0;

    case ZIP_SOURCE_CLOSE:
        return 0;

    case ZIP_SOURCE_READ:
    {
        if (length > ZIP_INT64_MAX)
        {
            zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
            return -1;
        }

        zip_int64_t n = AAsset_read(ctx->asset, data, static_cast<size_t>(length));
        if (n < 0)
        {
            zip_error_set(&ctx->error, ZIP_ER_READ, 0);
            return -1;
        }
        ctx->position += static_cast<zip_uint64_t>(n);
        return n;
    }

    case ZIP_SOURCE_SEEK:
    {
        zip_source_args_seek_t* args = ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t, data, length, &ctx->error);
        if (args == nullptr)
            return -1;

        zip_int64_t newPos = 0;
        switch (args->whence)
        {
        case SEEK_SET:
            newPos = args->offset;
            break;
        case SEEK_CUR:
            newPos = static_cast<zip_int64_t>(ctx->position) + args->offset;
            break;
        case SEEK_END:
            newPos = static_cast<zip_int64_t>(ctx->size) + args->offset;
            break;
        default:
            zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
            return -1;
        }

        if (newPos < 0 || static_cast<zip_uint64_t>(newPos) > ctx->size)
        {
            zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
            return -1;
        }

        if (AAsset_seek(ctx->asset, newPos, SEEK_SET) < 0)
        {
            zip_error_set(&ctx->error, ZIP_ER_SEEK, 0);
            return -1;
        }

        ctx->position = static_cast<zip_uint64_t>(newPos);
        return 0;
    }

    case ZIP_SOURCE_TELL:
        return static_cast<zip_int64_t>(ctx->position);

    case ZIP_SOURCE_STAT:
    {
        zip_stat_t* st = ZIP_SOURCE_GET_ARGS(zip_stat_t, data, length, &ctx->error);
        if (st == nullptr)
            return -1;

        zip_stat_init(st);
        st->valid = ZIP_STAT_SIZE;
        st->size = ctx->size;
        return 0;
    }

    case ZIP_SOURCE_SUPPORTS:
        return zip_source_make_command_bitmap(
            ZIP_SOURCE_OPEN, ZIP_SOURCE_CLOSE, ZIP_SOURCE_READ,
            ZIP_SOURCE_SEEK, ZIP_SOURCE_TELL, ZIP_SOURCE_STAT,
            ZIP_SOURCE_ERROR, ZIP_SOURCE_FREE, -1);

    case ZIP_SOURCE_ERROR:
        return zip_error_to_data(&ctx->error, data, length);

    case ZIP_SOURCE_FREE:
        if (ctx->asset)
        {
            AAsset_close(ctx->asset);
            ctx->asset = nullptr;
        }
        zip_error_fini(&ctx->error);
        delete ctx;
        return 0;

    default:
        zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
        return -1;
    }
}

static zip* openZipFromAsset(const char* zipFile)
{
    if (!__assetManager)
        return nullptr;

    std::string assetPath = gameplay::FileSystem::getAssetPath();
    assetPath += gameplay::FileSystem::resolvePath(zipFile);

    AAsset* asset = AAssetManager_open(__assetManager, assetPath.c_str(), AASSET_MODE_RANDOM);
    if (!asset)
        return nullptr;

    off_t length = AAsset_getLength(asset);
    if (length <= 0)
    {
        AAsset_close(asset);
        return nullptr;
    }

    AAssetZipContext* ctx = new AAssetZipContext();
    zip_error_init(&ctx->error);
    ctx->asset = asset;
    ctx->size = static_cast<zip_uint64_t>(length);
    ctx->position = 0;

    zip_error_t error;
    zip_error_init(&error);
    zip_source_t* source = zip_source_function_create(aassetZipSourceCallback, ctx, &error);
    if (!source)
    {
        AAsset_close(asset);
        zip_error_fini(&ctx->error);
        delete ctx;
        zip_error_fini(&error);
        return nullptr;
    }

    zip* archive = zip_open_from_source(source, ZIP_RDONLY, &error);
    if (!archive)
    {
        // Frees source and invokes ZIP_SOURCE_FREE (closes asset, deletes ctx)
        zip_source_free(source);
        zip_error_fini(&error);
        return nullptr;
    }

    zip_error_fini(&error);
    return archive;
}

#endif // __ANDROID__



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

    std::string fullPath;
    getFullPath(zipFile, fullPath);

    int err = 0;
    zip * res = zip_open(fullPath.c_str(), 0, &err);

#ifdef __ANDROID__
    if (!res)
        res = openZipFromAsset(zipFile);
#endif

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

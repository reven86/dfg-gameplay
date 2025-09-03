#pragma once

#include <zip.h>




/** An extension to gameplay::FileSystem helping
 *  to stream resources from zip packages
 */
class ZipPackage : public gameplay::Package, Noncopyable
{
public:
    static ZipPackage* create(const char* zipFile);

    virtual gameplay::Stream* open(const char* path, size_t streamMode = gameplay::FileSystem::READ) override;

    /**
     * Checks if the file at the given path exists.
     *
     * @param filePath The path to the file.
     *
     * @return <code>true</code> if the file exists; <code>false</code> otherwise.
     */
    virtual bool fileExists(const char* path) override;

    /**
     * Set password to access zip package content.
     *
     * @param password Password. Set to NULL to unset password.
     */
    void setPassword(const char* password);

protected:
    ZipPackage(const char* packageName, zip* zipObject);

private:
    std::string _packageName;
    std::shared_ptr<struct zip> _zip;

    struct FileInfo 
    {
        zip_uint64_t index;
        zip_uint64_t size;
    };
    std::unordered_map<std::string, FileInfo> _files;
    std::mutex _zipReadMutex;
};



/** ZipPackagesCache handles streaming from zip files and
 *  provides native zip (struct zip*) access point.
 */
class ZipPackagesCache : Noncopyable
{
public:
    static ZipPackage* findOrOpenPackage(const char * packageName);
    static void closePackage(const char * packageName);

    /**
     * Set password to access zip package content.
     *
     * @param password Password. Set to NULL to unset password.
     */
    static void setPassword(const char * packageName, const char * password);

    /**
     * Close all packages during shutdown. Normally at this point all packages already 
     * should be closed by the client app.
     */
    static void finalize();

protected:
    ZipPackagesCache() {};
    ~ZipPackagesCache() {};

private:
    static std::unordered_map<std::string, std::unique_ptr<class ZipPackage>> __packages;
};

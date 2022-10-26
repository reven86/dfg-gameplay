#pragma once

#ifndef __ZIP_PACKAGES__
#define __ZIP_PACKAGES__

#include <zip.h>



/** ZipPackagesCache handles streaming from zip files and
 *  provides native zip (struct zip*) access point.
 */
class ZipPackagesCache : Noncopyable
{
public:
    static zip * findOrOpenPackage(const char * packageName, bool ignoreCase = false);
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
    static void finilize();

protected:
    ZipPackagesCache() {};
    ~ZipPackagesCache() {};

private:
    static std::unordered_map<std::string, std::unique_ptr<class ZipPackage>> __packages;
};


#endif
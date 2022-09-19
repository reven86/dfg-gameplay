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
    static zip * findOrOpenPackage(const char * packageName);
    static void closePackage(const char * packageName);
    static bool hasFile(const char * packageName, const char * filename, bool ignoreCase);

protected:
    ZipPackagesCache() {};
    ~ZipPackagesCache() {};

private:
    static std::unordered_map< std::string, std::shared_ptr< zip > > _packages;
};


#endif
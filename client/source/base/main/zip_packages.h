#pragma once

#ifndef __ZIP_PACKAGES__
#define __ZIP_PACKAGES__

#include <zip.h>



/** ZipPackagesCache handles streaming from zip files and
 *  provides native zip (struct zip*) access point.
 */
class ZipPackagesCache : Noncopyable
{
    static std::unordered_map< std::string, std::shared_ptr< zip > > _packages;

protected:
    ZipPackagesCache() {};
    ~ZipPackagesCache() {};

public:
    static zip * findOrOpenPackage(const char * packageName);
    static bool hasFile(const char * packageName, const char * filename);
};


#endif
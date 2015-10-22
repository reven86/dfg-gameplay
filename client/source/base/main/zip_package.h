#ifndef __DFG_ZIP_PACKAGE__
#define __DFG_ZIP_PACKAGE__




/** An extension to gameplay::FileSystem to be able
 *  to stream resources from zip packages
 */
class ZipPackage : public gameplay::Package, Noncopyable
{
public:
    virtual ~ZipPackage();

    static ZipPackage * create(const char * zipFile);

    virtual gameplay::Stream * open(const char * path, size_t streamMode = gameplay::FileSystem::READ);

    /**
     * Checks if the file at the given path exists.
     *
     * @param filePath The path to the file.
     *
     * @return <code>true</code> if the file exists; <code>false</code> otherwise.
     */
    virtual bool fileExists(const char* filePath);

    /**
     * Set password to access zip package content.
     *
     * @param password Password. Set to NULL to unset password.
     */
    void setPassword(const char * password);

protected:
    ZipPackage(struct zip * zipFile);

private:
    std::shared_ptr< struct zip > _zipFile;
};



#endif // __DFG_ZIP_PACKAGE__

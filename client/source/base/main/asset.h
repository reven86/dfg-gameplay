#ifndef __DFG_ASSET__
#define __DFG_ASSET__




/** @brief Base resource class.
 *
 *	Base asset object.
 *
 *	@author Andrew "RevEn" Karpushin
 */

class Asset : public virtual gameplay::Ref, Noncopyable
{
public:
    /**
     * Destructs resource.
     */
    virtual ~Asset();

    /**
     * Get resource name.
     */
    virtual const char * getTypeName() const = 0;

    /**
     * Get resource URL.
     */
    const char * getURL() const { return _url.c_str(); };

    /** 
     * Reload asset.
     *
     * @return true when asset was reloaded successfully.
     */
    virtual bool reload() = 0;

    /**
     * Returns string representation.
     */
    std::string toString() const { return "<" + std::string(getTypeName()) + " from '" + (getURL() == NULL ? "memory" : getURL()) + "'>"; };

protected:
    Asset();

    void setURL(const char * url) { _url = url; };

private:
    std::string _url;
};





#endif // __DFG_ASSET__
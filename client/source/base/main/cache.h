#ifndef __DFG_CACHE__
#define __DFG_CACHE__

#include <locale>




class CacheBase : Noncopyable
{
public:
    virtual ~CacheBase() { _cachesConnection.disconnect(); };

    //! Remove all assets.
    virtual void clear() = 0;

    //! Reload all asset.
    virtual void reloadAll() = 0;

private:
    sigc::connection _cachesConnection;
};



/** @brief %Caches holder.
 *
 *	Holds all caches and manages them.
 *
 *	@author Andrew "RevEn" Karpushin
 */

class Caches : public Singleton< Caches >
{
    friend class Singleton< Caches >;

public:
    /**
     * Unload resources from all caches.
     */
    void flushAll()
    {
        std::for_each(_registeredCaches.begin(), _registeredCaches.end(), std::mem_fun(&CacheBase::clear));
    }

    /**
     * Register new cache object.
     */
    void registerCacheObject(CacheBase * obj)
    {
        GP_ASSERT(std::find(_registeredCaches.begin(), _registeredCaches.end(), obj) == _registeredCaches.end());
        _registeredCaches.push_back(obj);
    }

    /**
     * Destroy all caches.
     * As a result all caches with their assets will be unloaded and memory will be freed.
     * This method must be called at the finilization state of the program to avoid memory leaks.
     */
    void destroyAll()
    {
        for (CacheBase * cache : _registeredCaches)
            SAFE_DELETE(cache);

        _registeredCaches.clear();
    }

private:
    Caches() {};
    ~Caches() {};

    std::vector< CacheBase * > _registeredCaches;
};



/*! \brief %Cache class.
 *
 *	Caches assets loaded from disk.
 *
 *	\author Andrew "RevEn" Karpushin
 */

template< class T >
class Cache : public CacheBase
{
public:
    static Cache<T> * create()
    {
        Cache<T> * res = new Cache<T>();
        Caches::getInstance()->registerCacheObject(res);
        return res;
    }

    /**	@brief Register new asset.
     *
     *	Register new asset from file or get existing one.
     *  Assets loaded by this function are constant by default.
     *  If you need to alter asset, make a copy from this one.
     *
     *	@param	url Asset's URL.
     *	@return	Returns shared pointer to resource.
     */
    RefPtr< const T > load(const char * url)
    {
        std::string lowerName(url);
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
            std::bind2nd(std::ptr_fun(&std::tolower< char >), std::locale("")));
        typename ResourcesType::iterator it = _resources.find(lowerName);

        RefPtr< const T > res;
        if (it == _resources.end())
        {
            T * r = T::create(url);
            res.reset(r);

            if (!r)
            {
                GP_WARN("Can't load asset: %s", lowerName.c_str());
                return RefPtr< const T >();
            }

            _resources.insert(std::make_pair(lowerName, res));
            return res;
        }

        return (*it).second;
    };

    /**	@brief Unregister asset.
     *
     *	Removes asset from cache.
     *
     *	@param	object	Asset object.
     */
    void remove(RefPtr< T >& object)
    {
        // Linear search used due to rarely function using.
        typename ResourcesType::iterator it = _resources.begin();
        while (it != _resources.end())
        {
            if ((*it).second == object)
            {
                _resources.erase(it);
                break;
            }

            ++it;
        }
    };

    /**
     * Remove all assets.
     */
    virtual void clear()
    {
        _resources.clear();
    };

    /**
     * Reload all asset.
     */
    virtual void reloadAll()
    {
        for (typename ResourcesType::iterator it = _resources.begin(), end_it = _resources.end(); it != end_it; it++)
            const_cast<T *>((*it).second.get())->reload();
    }

private:
    Cache() { };

    typedef std::unordered_map< std::string, RefPtr< const T > > ResourcesType;
    ResourcesType _resources;
};





#endif // __DFG_CACHE__
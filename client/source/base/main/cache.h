#ifndef __DFG_CACHE__
#define __DFG_CACHE__

#include <locale>




/*! \brief %Caches holder.
 *
 *	Holds all caches and manages them.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class Caches : public Singleton< Caches >
{
    friend class Singleton< Caches >;

public:
    //! Clear callback function.
    typedef sigc::signal< void > ClearCallbacksType;

private:
    ClearCallbacksType _clearCallbacks;

private:
    Caches() { };
    ~Caches() { };

public:
    //! Get/Set clear callbacks.
    ClearCallbacksType& clearCallbacks() { return _clearCallbacks; };

public:
    //! Flush all caches.
    void flushAll() { _clearCallbacks(); };
};





/*! \brief %Cache class.
 *
 *	Caches assets loaded from disk.
 *
 *	\author Andrew "RevEn" Karpushin
 */

template< class T >
class Cache : Noncopyable
{
    typedef std::unordered_map< std::string, RefPtr< const T > > ResourcesType;
    ResourcesType _resources;

    sigc::connection _cachesConnection;

public:
    //! Constructs empty cache.
    Cache() {};

    //! Destructs cache object and unloads all loaded resources.
    ~Cache() { clear(); _cachesConnection.disconnect(); };

    /*!	\brief Register new asset.
     *
     *	Register new asset from file or get existing one.
     *  Assets loaded by this function are constant by default.
     *  If you need to alter asset, make a copy from this one.
     *
     *	\param	url Asset's URL.
     *	\return	Returns shared pointer to resource.
     */
    RefPtr< const T > load(const char * url)
    {
        if (!_cachesConnection.connected())
            _cachesConnection = Caches::getInstance()->clearCallbacks().connect(
            sigc::mem_fun(this, &Cache< T >::clear));

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

    /*!	\brief Unregister asset.
    *
    *	Removes asset from cache.
    *
    *	\param	object	Asset object.
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

    //! Remove all assets.
    void clear()
    {
        _resources.clear();
    };

    //! Reload all asset.
    void reloadAll()
    {
        for (typename ResourcesType::iterator it = _resources.begin(), end_it = _resources.end(); it != end_it; it++)
            const_cast<T *>((*it).second.get())->reload();
    }
};




#endif // __DFG_CACHE__
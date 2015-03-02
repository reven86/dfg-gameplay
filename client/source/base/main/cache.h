#ifndef __DFG_CACHE__
#define __DFG_CACHE__

#include <locale>




/*! \brief %Caches holder.
 *	
 *	Holds all caches and control for clearing them.
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
    Caches ( ) { };
    ~Caches ( ) { };

public:
    //! Get/Set clear callbacks.
    ClearCallbacksType& ClearCallbacks ( ) { return _clearCallbacks; };

    //! Get clear callbacks.
    const ClearCallbacksType& ClearCallbacks ( ) const { return _clearCallbacks; };

public:
    //! Flush all caches by invoking registered clear callbacks.
    void FlushAll ( ) { _clearCallbacks( ); };
};





/*! \brief %Cache class.
 *	
 *	Holds registered resources in one instance and provides access to newly loaded resources.
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
    Cache ( ) { };

    //! Destructs cache object and unloads all loaded resources.
    ~Cache ( ) { Clear( ); _cachesConnection.disconnect( ); };

    /*!	\brief Register new resource.
     *	
     *	Register new resource from file or get existing one.
     *  Assets loaded by this function are constant by default.
     *  If you need to alter asset, make a copy from this one.
     *
     *	\param	filename	Resource filename.
     *	\return	Returns shared pointer to resource.
     */
    RefPtr< const T > Register ( const char * filename )
    {
        if( !_cachesConnection.connected( ) )
            _cachesConnection = Caches::Instance( ).ClearCallbacks( ).connect(
                sigc::mem_fun( this, &Cache< T >::Clear ) );

        std::string lowerName( filename );
        std::transform( lowerName.begin( ), lowerName.end( ), lowerName.begin( ),
            std::bind2nd( std::ptr_fun( &std::tolower< char > ), std::locale( "" ) ) );
        typename ResourcesType::iterator it = _resources.find( lowerName );

        RefPtr< const T > res;
        if( it == _resources.end( ) )
        {
            T * r = new T( );
            res.reset( r );

            if( !r->LoadFromFile( lowerName.c_str( ) ) )
            {
                GP_WARN( "Can't load resource: %s", lowerName.c_str( ) );
                return RefPtr< const T >( );
            }

            _resources.insert( std::make_pair( lowerName, res ) );
            return res;
        }

        return ( *it ).second;
    };

    /*!	\brief Unregister resource.
    *	
    *	Unregister previously registered resource.
    *
    *	\param	object	Resource pointer object.
    */
    void Unregister ( RefPtr< T >& object )
    {
        // Linear search used due to rarely function using.
        typename ResourcesType::iterator it = _resources.begin( );

        while( it != _resources.end( ) )
        {
            if( ( *it ).second == object )
            {
                _resources.erase( it );
                break;
            }

            ++it;
        }
    };

    //! Unregister all registered resources.
    void Clear ( )
    {
        _resources.clear( );
    };

    //! Reload all resources.
    void ReloadAll ( )
    {
        for( typename ResourcesType::iterator it = _resources.begin( ), end_it = _resources.end( ); it != end_it; it++ )
            const_cast< T * >( ( *it ).second.get( ) )->LoadFromFile( ( *it ).first.c_str( ) );
    }
};




#endif // __DFG_CACHE__
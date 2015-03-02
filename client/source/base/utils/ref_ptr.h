#ifndef __DFG_REF_PTR__
#define __DFG_REF_PTR__





/*! \brief Smart pointer to Ref objects.
 *
 *	\author Andrew "RevEn" Karpushin
 */

template< class _Type >
class RefPtr
{
    _Type * _pointer;

public:
    RefPtr( )
        : _pointer( NULL )
    {
    };

    explicit RefPtr( _Type * data )
        : _pointer( data )
    {
    };

    ~RefPtr( )
    {
        reset( );
    };

    // copy constructor and assignment operator shares ownership of pointee object

    RefPtr( const RefPtr& a )
        : _pointer( a._pointer )
    {
        if( _pointer )
            const_cast< typename std::remove_const< _Type >::type * >( _pointer )->addRef( );
    };

    RefPtr& operator = ( const RefPtr& a )
    {
        reset( a._pointer );
        if( _pointer )
            const_cast< typename std::remove_const< _Type >::type * >( _pointer )->addRef( );

        return *this;
    };

    _Type * get( )
    {
        return _pointer;
    };

    _Type * release( )
    {
        _Type * pointer = _pointer;
        _pointer = NULL;
        return pointer;
    };

    const _Type * get( ) const
    {
        return _pointer;
    };

    _Type * operator -> ( )
    {
        return _pointer;
    };

    const _Type * operator -> ( ) const
    {
        return _pointer;
    };

    _Type& operator * ( )
    {
        return *_pointer;
    };

    const _Type& operator * ( ) const
    {
        return *_pointer;
    };

    //! Resets smart pointer. Takes ownership of 'data'
    void reset( _Type * data = NULL )
    {
        if( _pointer )
            const_cast< typename std::remove_const< _Type >::type * >( _pointer )->release( );
        _pointer = data;
    };

    operator bool ( ) const
    {
        return _pointer != NULL;
    };

    operator _Type * ( )
    {
        return _pointer;
    };

    operator const _Type * ( ) const
    {
        return _pointer;
    };
};

template< class _Type >
inline bool operator < ( const RefPtr< _Type >& a, const RefPtr< _Type >& b ) { return a.get( ) < b.get( ); };

template< class _Type >
inline bool operator == ( const RefPtr< _Type >& a, const RefPtr< _Type >& b ) { return a.get( ) == b.get( ); };


#endif // __DFG_REF_PTR__
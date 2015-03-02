#ifndef __DFG_GAMEPLAY_ASSETS__
#define __DFG_GAMEPLAY_ASSETS__





/*! Asset wrapper for gameplay::Properties.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class PropertiesAsset : public Asset
{
    gameplay::Properties * _properties;

    static Cache< PropertiesAsset > _sCache;

public:
    PropertiesAsset( );
    virtual ~PropertiesAsset( );

    static Cache< PropertiesAsset >& Cache( ) { return _sCache; };

    //! Load resource from file.
    virtual bool LoadFromFile ( const char * filename );

    //! Load resource from stream.
    virtual bool LoadFromStream ( gameplay::Stream * stream );

    operator gameplay::Properties * ( ) const
    {
        return _properties;
    }
};




/*! Asset wrapper for gameplay::Material.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class MaterialAsset : public Asset
{
    RefPtr< gameplay::Material > _material;

    static Cache< MaterialAsset > _sCache;

public:
    MaterialAsset( );
    virtual ~MaterialAsset( );

    static Cache< MaterialAsset >& Cache( ) { return _sCache; };

    //! Load resource from file.
    virtual bool LoadFromFile ( const char * filename );

    //! Load resource from stream.
    virtual bool LoadFromStream ( gameplay::Stream * stream );

    gameplay::Material * ShareAsset( ) const { gameplay::Material * res = const_cast< gameplay::Material * >( _material.get( ) ); res->addRef( ); return res; };

    const gameplay::Material * GetAsset( ) const { return _material.get( ); };
};




/*! Asset wrapper for gameplay::AudioSource.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class AudioSourceAsset : public Asset
{
    RefPtr< gameplay::AudioSource > _audioSource;

    static Cache< AudioSourceAsset > _sCache;

public:
    AudioSourceAsset( );
    virtual ~AudioSourceAsset( );

    static Cache< AudioSourceAsset >& Cache( ) { return _sCache; };

    //! Load resource from file.
    virtual bool LoadFromFile ( const char * filename );

    //! Load resource from stream.
    virtual bool LoadFromStream ( gameplay::Stream * stream );

    gameplay::AudioSource * ShareAsset( ) const { gameplay::AudioSource * res = const_cast< gameplay::AudioSource * >( _audioSource.get( ) ); res->addRef( ); return res; };

    const gameplay::AudioSource * GetAsset( ) const { return _audioSource.get( ); };
};




/*! Asset wrapper for gameplay::Font.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class FontAsset : public Asset
{
    RefPtr< gameplay::Font > _font;

    static Cache< FontAsset > _sCache;

public:
    FontAsset( );
    virtual ~FontAsset( );

    static Cache< FontAsset >& Cache( ) { return _sCache; };

    //! Load resource from file.
    virtual bool LoadFromFile ( const char * filename );

    //! Load resource from stream.
    virtual bool LoadFromStream ( gameplay::Stream * stream );

    gameplay::Font * ShareAsset( ) const { gameplay::Font * res = const_cast< gameplay::Font * >( _font.get( ) ); res->addRef( ); return res; };

    const gameplay::Font * GetAsset( ) const { return _font.get( ); };
};




#endif // __DFG_GAMEPLAY_ASSETS__
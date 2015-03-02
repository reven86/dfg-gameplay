#include "pch.h"
#include "gameplay_assets.h"






//
// PropertiesAsset
//

Cache< PropertiesAsset > PropertiesAsset::_sCache;

PropertiesAsset::PropertiesAsset( )
    : Asset( "Properties" )
    , _properties( NULL )
{
}

PropertiesAsset::~PropertiesAsset( )
{
    SAFE_DELETE( _properties );
}

bool PropertiesAsset::LoadFromFile( const char * filename )
{
    SAFE_DELETE( _properties );
    _properties = gameplay::Properties::create( filename );

    return _properties != NULL;
}

bool PropertiesAsset::LoadFromStream( gameplay::Stream * /*stream*/ )
{
    GP_ASSERT( !"PropertiesAsset::LoadFromStream is not yet implemented!");

    return false;
}



//
// MaterialAsset
//

Cache< MaterialAsset > MaterialAsset::_sCache;

MaterialAsset::MaterialAsset( )
    : Asset( "Material" )
{
}

MaterialAsset::~MaterialAsset( )
{
}

bool MaterialAsset::LoadFromFile( const char * filename )
{
    _material.reset( gameplay::Material::create( filename ) );

    return _material.get( ) != NULL;
}

bool MaterialAsset::LoadFromStream( gameplay::Stream * /*stream*/ )
{
    GP_ASSERT( !"MaterialAsset::LoadFromStream is not yet implemented!");

    return false;
}



//
// AudioSourceAsset
//

Cache< AudioSourceAsset > AudioSourceAsset::_sCache;

AudioSourceAsset::AudioSourceAsset( )
    : Asset( "AudioSource" )
{
}

AudioSourceAsset::~AudioSourceAsset( )
{
}

bool AudioSourceAsset::LoadFromFile( const char * filename )
{
    _audioSource.reset( gameplay::AudioSource::create( filename ) );

    return _audioSource.get( ) != NULL;
}

bool AudioSourceAsset::LoadFromStream( gameplay::Stream * /*stream*/ )
{
    GP_ASSERT( !"AudioSourceAsset::LoadFromStream is not yet implemented!");

    return false;
}




//
// FontAsset
//

Cache< FontAsset > FontAsset::_sCache;

FontAsset::FontAsset( )
    : Asset( "Font" )
{
}

FontAsset::~FontAsset( )
{
}

bool FontAsset::LoadFromFile( const char * filename )
{
    _font.reset( gameplay::Font::create( filename ) );

    return _font.get( ) != NULL;
}

bool FontAsset::LoadFromStream( gameplay::Stream * stream )
{
    stream;
    GP_ASSERT( !"FontAsset::LoadFromStream is not yet implemented!");

    return false;
}
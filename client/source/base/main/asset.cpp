#include "pch.h"
#include "asset.h"





//
// Asset
//

bool Asset::LoadFromFile( const char * file )
{
	_filename = file;

    std::auto_ptr< gameplay::Stream > stream( gameplay::FileSystem::open(file) );

	return LoadFromStream( stream.get( ) );
}

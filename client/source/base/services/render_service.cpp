#include "pch.h"
#include "render_service.h"
#include "service_manager.h"







RenderService::RenderService( const ServiceManager * manager )
    : Service( manager )
	, _activeSpriteBatch( NULL )
    , _activeMeshBatch( NULL )
{
}

RenderService::~RenderService( )
{
}

bool RenderService::OnInit( )
{
	return true;
}

bool RenderService::OnShutdown( )
{
	for( SpriteBatchRegistryType::iterator it = _spriteBatchRegistry.begin( ), end_it = _spriteBatchRegistry.end( ); it != end_it; it++ )
    {
        ( *it ).first->release( );
		SAFE_DELETE( ( *it ).second );
    }

    for( MeshBatchRegistryType::iterator it = _meshBatchRegistry.begin( ), end_it = _meshBatchRegistry.end( ); it != end_it; it++ )
    {
        std::get< 2 >( ( *it ).first )->release( );
        SAFE_DELETE( ( *it ).second );
    }

    _renderSteps.clear( );
    _spriteBatchRegistry.clear( );
    _meshBatchRegistry.clear( );

    return true;
}

RenderStep * RenderService::CreateRenderStep( const char * name, RenderStep * insert_after )
{
	RenderStepsType::iterator it = _renderSteps.begin( ), end_it = _renderSteps.end( );

	while( it != end_it && ( *it ).get( ) != insert_after)
		it++;

	if( it != end_it )
		it++;

	RenderStep * new_step = new RenderStep( name );
	_renderSteps.insert( it, RefPtr< RenderStep >( new_step ) );

    new_step->addRef( );
	return new_step;
}

RenderStep * RenderService::FindRenderStep( const char * name )
{
	for( RenderStepsType::iterator it = _renderSteps.begin( ), end_it = _renderSteps.end( ); it != end_it; it++ )
		if( !strcmp( ( *it )->GetName( ), name ) )
			return ( *it ).get( );

	return NULL;
}

void RenderService::RenderFrame( )
{
	for( RenderStepsType::const_iterator it = _renderSteps.begin( ), end_it = _renderSteps.end( ); it != end_it; it++ )
		( *it )->Render( );

    FlushAll( );
}

gameplay::SpriteBatch * RenderService::RegisterSpriteTexture( gameplay::Texture * texture )
{
	SpriteBatchRegistryType::iterator it = _spriteBatchRegistry.find( texture );
	if( it != _spriteBatchRegistry.end( ) )
    {
        SAFE_RELEASE( texture );
		return ( *it ).second;
    }

	gameplay::SpriteBatch * res = gameplay::SpriteBatch::create( texture );
	res->getStateBlock( )->setBlendSrc( gameplay::RenderState::BLEND_ONE );
	res->getSampler( )->setWrapMode( gameplay::Texture::CLAMP, gameplay::Texture::CLAMP );

	_spriteBatchRegistry[ texture ] = res;

	return res;
}

gameplay::SpriteBatch * RenderService::RegisterSpriteMaterial( gameplay::Material * material )
{
	SpriteBatchRegistryType::iterator it = _spriteBatchRegistry.find( material );
	if( it != _spriteBatchRegistry.end( ) )
    {
        SAFE_RELEASE( material );
		return ( *it ).second;
    }

	gameplay::SpriteBatch * res = gameplay::SpriteBatch::create( material );
	_spriteBatchRegistry[ material ] = res;

	return res;
}

gameplay::MeshBatch * RenderService::RegisterMeshBatch( const gameplay::VertexFormat& vertexFormat, 
    const gameplay::Mesh::PrimitiveType& primitiveType, gameplay::Material * material, bool indexed )
{
    MeshBatchItem infoTuple = std::make_tuple( vertexFormat, primitiveType, material, indexed );
    MeshBatchRegistryType::iterator it, end_it;
    for( it = _meshBatchRegistry.begin( ), end_it = _meshBatchRegistry.end( ); it != end_it; it++)
        if( ( *it ).first == infoTuple )
            break;

	if( it != _meshBatchRegistry.end( ) )
    {
        SAFE_RELEASE( material );
		return ( *it ).second;
    }

    gameplay::MeshBatch * res = gameplay::MeshBatch::create( vertexFormat, primitiveType, material, indexed, 256, 256 );
    _meshBatchRegistry.push_back( std::make_pair( infoTuple, res ) );

    return res;
}

void RenderService::BeginSpritesRendering( gameplay::SpriteBatch * batch )
{
	if( _activeSpriteBatch == batch )
		return;

	FlushAll( );

	_activeSpriteBatch = batch;
	_activeSpriteBatch->start( );
}

void RenderService::BeginMeshesRendering( gameplay::MeshBatch * batch )
{
	if( _activeMeshBatch == batch )
		return;

	FlushAll( );

	_activeMeshBatch = batch;
	_activeMeshBatch->start( );
}

void RenderService::FlushAll( )
{
    PROFILE( "RenderService::FlushAll", "Render" );

	if( _activeSpriteBatch )
    {
	    _activeSpriteBatch->finish( );
	    _activeSpriteBatch = NULL;
    }
    if( _activeMeshBatch )
    {
        _activeMeshBatch->finish( );
        _activeMeshBatch->draw( );
        _activeMeshBatch = NULL;
    }
}




//
// RenderClick
//

RenderClick::RenderClick( )
	: _active( true )
{
}

RenderClick::~RenderClick( )
{
}




//
// RenderStep
//

RenderStep::RenderStep( const char * name )
	: _name( name )
{
}

RenderStep::~RenderStep( )
{
}

RenderClick * RenderStep::FindRenderClick( const char * name )
{
	for( RenderClicksType::iterator it = _renderClicks.begin( ), end_it = _renderClicks.end( ); it != end_it; it++ )
		if( !strcmp( ( *it )->GetName( ), name ) )
			return ( *it ).get( );

	return NULL;
}

void RenderStep::AddRenderClick( RenderClick * click, RenderClick * insert_after )
{
	RenderClicksType::iterator it = _renderClicks.begin( ), end_it = _renderClicks.end( );

	while( it != end_it && ( *it ).get( ) != insert_after)
		it++;

	if( it != end_it )
		it++;
	
    click->addRef( );
	_renderClicks.insert( it, RefPtr< RenderClick >( click ) );
}

void RenderStep::Render( ) const
{
	for( RenderClicksType::const_iterator it = _renderClicks.begin( ), end_it = _renderClicks.end( ); it != end_it; it++ )
		if( ( *it )->IsActive( ) )
			( *it )->Render( );
}
#include "pch.h"
#include "profiler.h"
#include "services/render_service.h"
#include "services/service_manager.h"







static unsigned __dfg_profile_id_unprofiled_code;

//
// Profiler Manager
//

Profiler::Profiler( ) :
	_parentsCount( 0 ),
	_registeredGroupsCount( 0 ),
	_registeredObjectsCount( 0 ),
	_frameDelta( 0.01f )
{
    __dfg_profile_id_unprofiled_code = RegisterProfilerObject( "Unprofiled", "General" );
    StartProfiler( __dfg_profile_id_unprofiled_code );
}

Profiler::~Profiler( )
{
    StopProfiler( );
}

unsigned Profiler::RegisterProfilerObject( const char * name, const char * group )
{
	GP_ASSERT( _registeredObjectsCount < MAX_OBJECTS );
	GP_ASSERT( _registeredGroupsCount < MAX_GROUPS );

	unsigned g;
	for( g = 0; g < _registeredGroupsCount; g++ )
		if( _registeredGroups[ g ].first == group )
			break;

	if( g == _registeredGroupsCount )
	{
		if( _registeredGroupsCount == MAX_GROUPS )
			return INVALID_OBJECT_ID;

		_registeredGroups[ _registeredGroupsCount++ ] = std::make_pair( std::string( group ), 0.0f );
	}

	unsigned o;
	for( o = 0; o < _registeredObjectsCount; o++ )
		if( _registeredObjects[ o ].first == name )
			break;

	// this can be if PROFILE macro set into template function
	//GP_ASSERT( o == mRegisteredObjectsCount );

	if( o == _registeredObjectsCount )
	{
		if( _registeredObjectsCount == MAX_OBJECTS )
			return INVALID_OBJECT_ID;

		_registeredObjects[ _registeredObjectsCount++ ] = std::make_pair( std::string( name ), 0.0f );
	}

	return g * MAX_OBJECTS + o;
}

void Profiler::StartProfiler( const unsigned& profile_object_id )
{
	GP_ASSERT( profile_object_id != INVALID_OBJECT_ID );
	GP_ASSERT( _parentsCount < MAX_OBJECTS );

	if( profile_object_id == INVALID_OBJECT_ID )
		return;

	if( _parentsCount >= MAX_OBJECTS )
		return;

    float time = static_cast< float >( gameplay::Game::getInstance( )->getAbsoluteTime( ) );

	_currentParents[ _parentsCount++ ] = std::make_pair( profile_object_id, time );
}

void Profiler::StopProfiler( )
{
    float time = static_cast< float >( gameplay::Game::getInstance( )->getAbsoluteTime( ) );

	GP_ASSERT( _parentsCount > 0 );

	if( _parentsCount == 0 )
		return;

	_parentsCount--;
    float diff = time - _currentParents[ _parentsCount ].second;

	int g, o;

	g = _currentParents[ _parentsCount ].first / MAX_OBJECTS;
	o = _currentParents[ _parentsCount ].first % MAX_OBJECTS;

	_registeredGroups[ g ].second += diff;
	_registeredObjects[ o ].second += diff;

	//GP_ASSERT( _registeredGroups[ g ].second >= 0 );
	//GP_ASSERT( _registeredObjects[ o ].second >= 0 );

	for( unsigned p = 0; p < _parentsCount; p++ )
		_currentParents[ p ].second += diff;
}

void Profiler::SaveProfilerResults( )
{
    static int parents[ MAX_OBJECTS ];
    unsigned saveParentsCount = _parentsCount;
    while( _parentsCount > 0 )
    {
        parents[ _parentsCount - 1 ] = _currentParents[ _parentsCount - 1 ].first;
        StopProfiler( );
    }

	std::copy( _registeredGroups, _registeredGroups + _registeredGroupsCount, _profileGroupsResult );
	std::copy( _registeredObjects, _registeredObjects + _registeredObjectsCount, _profileObjectsResult );

	float dt = _frameDelta > 1.0f ? 1.0f : _frameDelta;

	// reset 
	for( unsigned g = 0; g < _registeredGroupsCount; g++ )
		_registeredGroups[ g ].second *= 1.0f - dt;

	for( unsigned o = 0; o < _registeredObjectsCount; o++ )
		_registeredObjects[ o ].second *= 1.0f - dt;

    while( _parentsCount < saveParentsCount )
        StartProfiler( parents[ _parentsCount ] );
}

void Profiler::DrawPerformanceInfo( const gameplay::Font * fnt, gameplay::SpriteBatch * white_shd, float fontSize ) const
{
    unsigned i;

	float total_obj_time;
	total_obj_time = 0;

    for( i = 0; i < _registeredObjectsCount; i++ )
        total_obj_time += _profileObjectsResult[ i ].second;

	float cur_x = 100.0f;
	float cur_y = 100.0f;
	float bar_w = 150.0f;
	float bar_h	= static_cast< float >( fontSize ) + 8.0f;

    RenderService * renderService = ServiceManager::Instance( ).FindService< RenderService >( );

    renderService->BeginSpritesRendering( white_shd );
    white_shd->start( );

    for( i = 0; i < _registeredObjectsCount; i++ )
	{
        const ObjectInfo& pi = _profileObjectsResult[ i ];
		float percent = total_obj_time == 0 ? 0 : pi.second / total_obj_time * 100.0f;

		if( percent < 3.0f )
			continue;

        white_shd->draw(
			cur_x + bar_w * percent * 0.01f * 0.5f, cur_y + bar_h * 0.5f - 4.0f, 0.0f,
			bar_w * percent * 0.01f, bar_h,
            0.0f, 1.0f, 1.0f, 0.0f,
            gameplay::Vector4( 0.0f, 0.5f, 0.0f, 1.0f ),
            true );

		cur_y += bar_h + 8.0f;
	}

	// draw groups
	float total_group_time;
	total_group_time = 0;

    for( i = 0; i < _registeredGroupsCount; i++ )
        total_group_time += _profileGroupsResult[ i ].second;

	cur_x = 350.0f;
	cur_y = 100.0f;

    for( i = 0; i < _registeredGroupsCount; i++ )
	{
        const ObjectInfo& pi = _profileGroupsResult[ i ];
		float percent = total_group_time == 0 ? 0 : pi.second / total_group_time * 100.0f;

		if( percent < 3.0f )
			continue;

        white_shd->draw(
			cur_x + bar_w * percent * 0.01f * 0.5f, cur_y + bar_h * 0.5f - 4.0f, 0.0f,
			bar_w * percent * 0.01f, bar_h,
            0.0f, 1.0f, 1.0f, 0.0f,
            gameplay::Vector4( 0.0f, 0.5f, 0.0f, 1.0f ),
            true );

		cur_y += bar_h + 8.0f;
	}

    renderService->FlushAll( );
    fnt->start( );

    char buf[ 255 ];

	cur_x = 100.0f;
	cur_y = 100.0f;
    for( i = 0; i < _registeredObjectsCount; i++ )
	{
        const ObjectInfo& pi = _profileObjectsResult[ i ];
		float percent = total_obj_time == 0 ? 0 : pi.second / total_obj_time * 100.0f;

		if( percent < 3.0f )
			continue;

        sprintf( buf, "%s: %.1f", pi.first.c_str( ), pi.second * _frameDelta );

        fnt->drawText( ANSIToWCS( buf ), cur_x, cur_y, gameplay::Vector4::one( ), fontSize );

		cur_y += bar_h + 8.0f;
	}

	cur_x = 350.0f;
	cur_y = 100.0f;
    for( i = 0; i < _registeredGroupsCount; i++ )
	{
        const ObjectInfo& pi = _profileGroupsResult[ i ];
		float percent = total_group_time == 0 ? 0 : pi.second / total_group_time * 100.0f;

		if( percent < 3.0f )
			continue;

        sprintf( buf, "%s: %.1f", pi.first.c_str( ), pi.second * _frameDelta );

        fnt->drawText( ANSIToWCS( buf ), cur_x, cur_y, gameplay::Vector4::one( ), fontSize );

		cur_y += bar_h + 8.0f;
	}

    fnt->finish( );
}

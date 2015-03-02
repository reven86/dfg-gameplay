#include "pch.h"
#include "particle_system.h"
#include "services/render_service.h"
#include "services/service_manager.h"






//
// ParticleSubSystem
//

ParticleSubSystem::ParticleSubSystem( )
    : spawn_count( 0 )
    , align_to_motion( false )
    , spriteBatch( NULL )
    , sourceRect( 0.0f, 0.0f, 1.0f, 1.0f )
    , aspect( 1.0f )
{
}

void ParticleSubSystem::UpdateParticle( Particle& p, float dt ) const
{
	unsigned char normaltime = static_cast< unsigned char >( p.localtime / p.lifetime * 256 );

	p.position += velocity_curve.Key( normaltime ) * dt * p.velocity;
    if( p.acceleration != 0.0f )
        p.velocity += dt * p.acceleration * acceleration_curve.Key( normaltime ) * accelerate_dir;
    if( p.motionrand > 0 )
        p.velocity += dt * p.motionrand * motionrand_curve.Key( normaltime ) * gameplay::Vector3::random( ).normalize( );
    if( p.spin != 0.0f )
    	p.angle += p.spin * spin_curve.Key( normaltime ) * dt;
	p.size = p.start_size * size_curve.Key( normaltime );
	p.color = colors_curve.Key( normaltime );
}

void ParticleSubSystem::SpawnParticle( Particle& p, const gameplay::Matrix& transform ) const
{
	float angle = emissionrange * MATH_RANDOM_MINUS1_1( );
	float cos = cosf( angle );
	float sin = sinf( angle );

    gameplay::Vector3 o;
    gameplay::Vector3::cross( velocity_dir, gameplay::Vector3::random( ), &o );

    p.position = emitter_pos + gameplay::Vector3(
		volume.x * ( 0.5f - MATH_RANDOM_0_1( ) ),
		volume.y * ( 0.5f - MATH_RANDOM_0_1( ) ),
		volume.z * ( 0.5f - MATH_RANDOM_0_1( ) )
		);

    transform.transformPoint( &p.position );

    gameplay::Vector3 dir( velocity_dir * cos + o.normalize( ) * sin );

    p.velocity = dir.normalize( ) * ( velocity + velocity_variation * MATH_RANDOM_MINUS1_1( ) );

    transform.transformVector( &p.velocity );

	p.start_size	= size + size_variation * MATH_RANDOM_MINUS1_1( );
	p.angle			= align_to_motion ? angle : particleangle + particleangle_variation * MATH_RANDOM_MINUS1_1( );
	p.spin			= spin + spin_variation * MATH_RANDOM_MINUS1_1( );
	p.motionrand	= motionrand + motionrand_variation * MATH_RANDOM_MINUS1_1( );
	p.color			= colors_curve.Key( 0 );
	p.size			= p.start_size * size_curve.Key( 0 );
	p.acceleration	= acceleration + accelerate_variation * MATH_RANDOM_MINUS1_1( );

    if( p.lifetime <= 0 )
    {
    	p.lifetime	= lifetime + life_variation * MATH_RANDOM_MINUS1_1( );
        p.localtime	= fmodf( p.localtime, p.lifetime );
    }
    else
    {
        p.localtime	= fmodf( p.localtime, p.lifetime );
    	p.lifetime	= lifetime + life_variation * MATH_RANDOM_MINUS1_1( );
    }

	UpdateParticle( p, p.localtime );
}

bool ParticleSubSystem::LoadFloatCurveFromProperties( gameplay::Properties * properties, Curve< float > * curve ) const
{
    const char* name;
    while( ( name = properties->getNextProperty() ) )
    {
        if( strcmp( name, "key" ) == 0 )
        {
            const char * str = properties->getString( );
            unsigned int key;
            float value;
            if( sscanf( str, "%u, %f", &key, &value ) != 2 )
            {
                GP_WARN( "Failed to read curve key." );
                return false;
            }

            curve->AddKey( static_cast< unsigned char >( key ), value );
        }
    }

    return true;
}

bool ParticleSubSystem::LoadColorCurveFromProperties( gameplay::Properties * properties, Curve< gameplay::Vector4 > * curve ) const
{
    const char* name;
    while( ( name = properties->getNextProperty() ) )
    {
        if( strcmp( name, "key" ) == 0 )
        {
            const char * str = properties->getString( );
            unsigned int key;
            unsigned int value;
            if( sscanf( str, "%u, 0x%x", &key, &value ) != 2 )
            {
                GP_WARN( "Failed to read curve key." );
                return false;
            }

            gameplay::Vector4 color = gameplay::Vector4::fromColor( value );
            // colors for curves stored in ARGB format
            curve->AddKey( static_cast< unsigned char >( key ), gameplay::Vector4( color.y, color.z, color.w, color.x ) );
        }
    }

    return true;
}

bool ParticleSubSystem::LoadFromProperties( gameplay::Properties * properties )
{
    // Check if the Properties is valid and has a valid namespace.
    if( !properties || !( strcmp( properties->getNamespace(), "subsystem" ) == 0 ) )
    {
        GP_WARN( "Properties object must be non-null and have namespace equal to 'subsystem'." );
        return false;
    }

    // Go through all the particle system properties and create subsystem under this system.
    gameplay::Properties* curveProperties = NULL;
    while( curveProperties = properties->getNextNamespace( ) )
    {
        if( strcmp( curveProperties->getNamespace( ), "size_curve" ) == 0 )
        {
            if( !LoadFloatCurveFromProperties( curveProperties, &size_curve ) )
                return false;
        }
        else if( strcmp( curveProperties->getNamespace( ), "velocity_curve" ) == 0 )
        {
            if( !LoadFloatCurveFromProperties( curveProperties, &velocity_curve ) )
                return false;
        }
        else if( strcmp( curveProperties->getNamespace( ), "acceleration_curve" ) == 0 )
        {
            if( !LoadFloatCurveFromProperties( curveProperties, &acceleration_curve ) )
                return false;
        }
        else if( strcmp( curveProperties->getNamespace( ), "spin_curve" ) == 0 )
        {
            if( !LoadFloatCurveFromProperties( curveProperties, &spin_curve ) )
                return false;
        }
        else if( strcmp( curveProperties->getNamespace( ), "motionrand_curve" ) == 0 )
        {
            if( !LoadFloatCurveFromProperties( curveProperties, &motionrand_curve ) )
                return false;
        }
        else if( strcmp( curveProperties->getNamespace( ), "colors_curve" ) == 0 )
        {
            if( !LoadColorCurveFromProperties( curveProperties, &colors_curve ) )
                return false;
        }
    }

    spriteBatch = ServiceManager::Instance( ).FindService< RenderService >( )->RegisterSpriteMaterial( MaterialAsset::Cache( ).Register( properties->getString( "material" ) )->ShareAsset( ) );

    GP_ASSERT( spriteBatch && spriteBatch->getSampler( ) && spriteBatch->getSampler( )->getTexture( ) );

    unsigned texW = spriteBatch->getSampler( )->getTexture( )->getWidth( );
    unsigned texH = spriteBatch->getSampler( )->getTexture( )->getHeight( );

    gameplay::Vector4 sourceRegion;
    if( properties->getVector4( "src_rect", &sourceRegion ) )
    {
        sourceRect.set( sourceRegion.x / texW, sourceRegion.y / texH, sourceRegion.z / texW, sourceRegion.w / texH );
        aspect = static_cast< float >( sourceRegion.z ) / sourceRegion.w;
    }
    else
    {
        sourceRect.set( 0.0f, 0.0f, 1.0f, 1.0f );
        aspect = static_cast< float >( texW ) / texH;
    }

    properties->getVector3( "emitter_pos", &emitter_pos );
    properties->getVector3( "accelerate_dir", &accelerate_dir );
    properties->getVector3( "velocity_dir", &velocity_dir );
    properties->getVector3( "volume", &volume );
    max_particles = static_cast< unsigned >( properties->getInt( "max_particles" ) );
    spawn_count = properties->getInt( "spawn_count" );

    lifetime = properties->getFloat( "lifetime" );
    size = properties->getFloat( "size" );
    velocity = properties->getFloat( "velocity" );
    acceleration = properties->getFloat( "acceleration" );
    emissionrange = MATH_DEG_TO_RAD( properties->getFloat( "emissionrange" ) );
    spin = MATH_DEG_TO_RAD( properties->getFloat( "spin" ) );
    motionrand = properties->getFloat( "motionrand" );
    particleangle = MATH_DEG_TO_RAD( properties->getFloat( "particleangle" ) );
    align_to_motion = properties->getBool( "align_to_motion", false );

    life_variation = properties->getFloat( "life_variation" );
    size_variation = properties->getFloat( "size_variation" );
    velocity_variation = properties->getFloat( "velocity_variation" );
    accelerate_variation = properties->getFloat( "accelerate_variation" );
    spin_variation = MATH_DEG_TO_RAD( properties->getFloat( "spin_variation" ) );
    motionrand_variation = properties->getFloat( "motionrand_variation" );
    particleangle_variation = MATH_DEG_TO_RAD( properties->getFloat( "particleangle_variation" ) );
    starttime_variation = properties->getFloat( "starttime_variation" );
    
    accelerate_dir.normalize( );
	velocity_dir.normalize( );

    return true;
}




//
// BaseParticleSystem
//
BaseParticleSystem::BaseParticleSystem( ) 
    : _isStopped( false )
    , _aliveCount( 0 )
    , _maxParticles( 0 )
    , _emitterTransformation( gameplay::Matrix::identity( ) )
    , _maxParticleSize( 0.0f )
    , _scaler( 1.0f )
{
}

//bool BaseParticleSystem::TestFrustumCulling( const Viewer * viewer, const Mat4& transform ) const
//{
//	Primitives::OBB obb( mAABB );
//	obb.Transform( transform );
//
//	_ASSERTE( viewer );
//
//	if( r_debug_particles->AsInt( ) )
//		Renderer::Instance( ).GetDrawTools( ).PushDrawBox( obb, 0xffffff00 );
//
//	return viewer && viewer->TestFrustumCulling( obb );
//}




//
// ParticleSystem
//

Cache< ParticleSystem > ParticleSystem::_sCache;

ParticleSystem::ParticleSystem( ) 
    : Asset( "ParticleSystem" )
    , _invisibleTimer( 0 )
    , _framesToUpdate( 0 )
    , _flags( 0 )
    , _updatePeriod( 0.025f )
    , _updateTimer( 0 )
{
    _renderService = ServiceManager::Instance( ).FindService< RenderService >( );
}

ParticleSystem::~ParticleSystem( )
{
}

void ParticleSystem::AddSubSystem( const ParticleSubSystem& ps )
{
	_maxParticles += ps.max_particles;

	_systems.push_back( ps );
	_particles.resize( _maxParticles, Particle( ) );
	
	Reset( );
}

void ParticleSystem::RemoveSubSystem( unsigned index )
{
	if( index >= _systems.size( ) )
		return;

    SystemsType::iterator it = _systems.begin( );
    std::advance( it, index );

	_maxParticles -= ( *it ).max_particles;
	_systems.erase( it );

	_particles.resize( _maxParticles, Particle( ) );

	Reset( );
}

const ParticleSubSystem * ParticleSystem::GetSubSystem( unsigned index ) const
{
	if( index >= _systems.size( ) )
		return NULL;

    SystemsType::const_iterator it = _systems.begin( );
    std::advance( it, index );

    return &( *it );
}

Particle * ParticleSystem::GetParticle( unsigned particle_index )
{
	if( particle_index >= _particles.size( ) )
		return NULL;

    ParticlesType::iterator it = _particles.begin( );
    std::advance( it, particle_index );

    return &( *it );
}

const Particle * ParticleSystem::GetParticle( unsigned particle_index ) const
{
	if( particle_index >= _particles.size( ) )
		return NULL;

    ParticlesType::const_iterator it = _particles.begin( );
    std::advance( it, particle_index );

    return &( *it );
}

void ParticleSystem::Reset( )
{
	ParticlesType::iterator pit = _particles.begin( );

	for( SystemsType::iterator sit = _systems.begin( ); sit != _systems.end( ); sit++ )
	{
		float dt = ( *sit ).lifetime / ( *sit ).max_particles * ( *sit ).starttime_variation;
		float t = 0;
		for( unsigned i = 0; i < ( *sit ).max_particles; i++, pit++, t -= dt )
		{
			(*pit).localtime	= t;
			(*pit).lifetime		= 0;
			(*pit).spawn_count	= 0;
            (*pit).visible      = false;
		}
	}
    
	_isStopped = false;
}

void ParticleSystem::Render( const gameplay::Vector4& modulator ) const
{
    PROFILE( "ParticleSystem::Render", "Render" );

    const gameplay::Matrix * transform( &GetTransformation( ) );

	_invisibleTimer = 0;

	float scale_kw = GetScaler( );
	float scale_kh = GetScaler( );
    bool modulateColor = modulator != gameplay::Vector4::one( );

	ParticlesType::const_iterator pit = _particles.begin( );
	for( SystemsType::const_iterator it = _systems.begin( ), end_it = _systems.end( ); it != end_it; it++ )
	{
        const ParticleSubSystem& subSystem = *it;

        GP_ASSERT( subSystem.spriteBatch && "Material is absent!" );

		unsigned max_particles = subSystem.max_particles;
        _renderService->BeginSpritesRendering( subSystem.spriteBatch );

        const float& aspect = subSystem.aspect;

		for( unsigned i = 0; i < max_particles; i++, pit++ )
		{
			const Particle& par = *pit;

            if( par.visible )
			{
                gameplay::Vector3 pos;
                transform->transformPoint( par.position, &pos );

				if( par.color.w != 0 )
                    subSystem.spriteBatch->draw(
                        pos.x, pos.y, pos.z,
                        par.size * scale_kw * aspect, par.size * scale_kh,
                        subSystem.sourceRect.left( ), 1.0f - subSystem.sourceRect.bottom( ), subSystem.sourceRect.right( ), 1.0f - subSystem.sourceRect.top( ),
                        modulateColor ? gameplay::Vector4( modulator.x * par.color.x, modulator.y * par.color.y, modulator.z * par.color.z, modulator.w * par.color.w ) : par.color, 
                        gameplay::Vector2( 0.5f, 0.5f ), par.angle, true );
			}
		}
	}
}

void ParticleSystem::Update( float dt )
{
    PROFILE( "ParticleSystem::Update", "Application" );

    _updateTimer -= dt;
	if( _updateTimer > 0 )
		return;

	if( ( _flags & EFL_HIGH_PRECISION ) != 0 && -_updateTimer > _updatePeriod )
	{
		float delta = -_updateTimer;

		while( delta > _updatePeriod )
		{
			PureUpdate( _updatePeriod );
			delta -= _updatePeriod;
		}

		PureUpdate( _updatePeriod + delta );
		_updateTimer = _updatePeriod;
		return;
	}

	dt = _updatePeriod - _updateTimer;
	_updateTimer = _updatePeriod;

	_invisibleTimer += dt;
	if( _invisibleTimer > 0.5f )
	{
		if( --_framesToUpdate > 0 )
			return;

		_framesToUpdate = 3;
		dt *= 3.0f;
	}

	PureUpdate( dt );
}

void ParticleSystem::PureUpdate( float dt )
{
    PROFILE( "ParticleSystem::PureUpdate", "Application" );

    _aliveCount = 0;
	_maxParticleSize = 0.0f;

	//mAABB =	Primitives::AABB::Null( );

	ParticlesType::iterator pit = _particles.begin( );
	for( SystemsType::const_iterator it = _systems.begin( ); it != _systems.end( ); it++ )
	{
		int max_particles = static_cast< int >( ( *it ).max_particles );

#pragma omp parallel
		{

#pragma omp for
		for( int i = 0; i < max_particles; i++ )
		{
			Particle& p = *( &( *pit ) + i );

			p.localtime += dt;

			if( p.localtime < 0.0f )
			{
			}
			else if( p.localtime > p.lifetime )
            {
                if( !_isStopped && ( ( *it ).spawn_count <= 0 || p.spawn_count++ < ( *it ).spawn_count ) )
			    {
				    ( *it ).SpawnParticle( p, _emitterTransformation );
                    p.visible = true;
			    }
                else
                    p.visible = false;
            }
			else    // 0.0 <= p.localtime <= p.lifetime
			{
				( *it ).UpdateParticle( p, dt );
                p.visible = true;
			}

			if( p.visible )
			{
#pragma omp atomic
				_aliveCount++;

//				if( !mAABB.IsInside( p.position ) )
//				{
//#pragma omp critical ( cs_aabbtest )
//					mAABB.Enlarge( p.position );
//				}

//				if( p.size > _maxParticleSize )
//				{
//#pragma omp critical ( cs_sizetest )
//					if( p.size > _maxParticleSize )
//						_maxParticleSize = p.size;
//				}
			}
		}
		}

		std::advance( pit, max_particles );
	}

	//mAABB.EnlargeSize( Vec3( mMaxParticleSize * GetScaler( ) ) );
}

ParticleSystem * ParticleSystem::Clone( ) const
{
    return new ParticleSystem( *this );
}

bool ParticleSystem::LoadFromFile( const char * filename )
{
    // Load the particle system properties from file.
    gameplay::Properties* properties = gameplay::Properties::create(filename);
    if (properties == NULL)
    {
        GP_WARN("Failed to create particle system from file %s.", filename);
        return false;
    }

    bool res = LoadFromProperties( ( strlen( properties->getNamespace( ) ) > 0 ) ? properties : properties->getNextNamespace( ) );

    SAFE_DELETE(properties);
    return res;
}

bool ParticleSystem::LoadFromStream( gameplay::Stream * )
{
    GP_WARN( "ParticleSystem can't be loaded from Stream at the moment." );

    return false;
}

bool ParticleSystem::LoadFromProperties( gameplay::Properties * properties )
{
    // Check if the Properties is valid and has a valid namespace.
    if( !properties || !( strcmp( properties->getNamespace(), "particle_system" ) == 0 ) )
    {
        GP_WARN( "Properties object must be non-null and have namespace equal to 'particle_system'." );
        return false;
    }

	_systems.clear ();
	ParticlesType( ).swap( _particles );

	_maxParticles = 0;
	_flags = 0;
	_updatePeriod = 0.025f;
	_updateTimer = 0;
    _invisibleTimer = 0;
    _framesToUpdate = 0;

    // Go through all the particle system properties and create subsystem under this system.
    gameplay::Properties* subsystemProperties = NULL;
    while( ( subsystemProperties = properties->getNextNamespace( ) ) )
    {
        if( strcmp( subsystemProperties->getNamespace( ), "subsystem" ) == 0 )
        {
            ParticleSubSystem subsystem;

            if( !subsystem.LoadFromProperties( subsystemProperties ) )
            {
                GP_WARN("Failed to load subsystem for particle system.");
                return false;
            }

            AddSubSystem( subsystem );
        }
    }

    if( properties->getBool( "hi_percision_update" ) )
        _flags |= EFL_HIGH_PRECISION;

    if( properties->exists( "update_period" ) )
        _updatePeriod = properties->getFloat( "update_period" );

    return true;
}
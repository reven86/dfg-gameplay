#ifndef __DFG_PARTICLE_SYSTEM__
#define __DFG_PARTICLE_SYSTEM__

#include "utils/curve.h"






/** 
 * %Particle structure.
 *
 *	Simple 3D particle structure.
 *	
 *	@see ParticleSystem, ParticleSubSystem
 *	@author Andrew "RevEn" Karpushin
 */

struct Particle
{
    gameplay::Vector3   position;						///< Position.
    gameplay::Vector3   velocity;						///< Velocity.
    float               size;							///< Size.
    float				acceleration;					///< Acceleration.
    float       		angle;							///< Roll angle.
    float       		spin;							///< Angle speed.
    float		        motionrand;						///< Motion randomness.
    gameplay::Vector4	color;							///< Particle color.
    int				    spawn_count;					///< Current spawn count.

    float				start_size;						///< Start size.

    float				lifetime;						///< Total life time.
    float				localtime;						///< Local non-normalized time.

    bool                visible;                        ///< Is particle visible (internal state).
};




/** @brief %Particle subsystem structure.
 *	
 *	Stores particles with same type.
 *	
 *	@see ParticleSystem, Particle
 *	@author Andrew "RevEn" Karpushin
 *
 *	Example:
 *
 *	\verbatim
  subsystem:
  - material: textures/sfx/puff2_alpha.material

    emitter_pos: [-100.0, -50.0, 0.0]
    accelerate_dir: [0.0, -1.0, 0.0]
    velocity_dir: [3.0, 2.0, 0.0]
    volume: [10.0, 10.0, 0.0]
    max_particles: 10
    spawn_count: 0

    lifetime: 0.8
    size: 55
    velocity: 80
    acceleration: 10
    emissionrange: 20
    spin: 0
    motionrand: 0
    particleangle: 0

    life_variation: 0.02
    size_variation: 15
    velocity_variation: 10
    accelerate_variation: 5
    spin_variation: 10
    motionrand_variation: 0
    particleangle_variation: 360
    starttime_variation: 1.0

    size_curve:
    - key: [0, 0.5]
    - key: [255, 1.5]

    velocity_curve:
    - key: [0, 1.0]
    - key: [255, 1.0]

    acceleration_curve:
    - key: [0, 1.0]
    - key: [255, 1.0]

    spin_curve:
    - key: [0, 1.0]
    - key: [255, 1.0]

    motionrand_curve:
    - key: [0, 1.0]
    - key: [255, 1.0]

    colors_curve:
    - key: [0, 0x00ffffff]
    - key: [127, 0x40ffffff]
    - key: [255, 0x00ffffff]
\endverbatim
 *	
 *	Description:
 *
 *	Keywords has same meaning as in ParticleSubSystem members.
 *
 *	@}
 */

struct ParticleSubSystem
{
    gameplay::Vector3   emitter_pos;					///< Emitter position.
    gameplay::Vector3   accelerate_dir;					///< \b Normalized accelerate direction.
    gameplay::Vector3   velocity_dir;					///< \b Normalized velocity direction.
    gameplay::Vector3   volume;							///< Spawn volume.
    unsigned			max_particles;					///< Max particles.
    int					spawn_count;					///< Maximum spawn count for each particle.

    float               lifetime;						///< Base particle lifetime.
    float				size;							///< Base particle size.
    float				velocity;						///< Base particle speed.
    float				acceleration;					///< Base particle acceleration.
    float               emissionrange;					///< Emitter emission range from 0 to pi.
    float               spin;							///< Base particle spin.
    float				motionrand;						///< Base particle motion randomness.
    float               particleangle;					///< Base particle angle.
    bool				align_to_motion;				///< Are particles aligned to motion?

    float               life_variation;					///< Particle lifetime variation.
    float				size_variation;					///< Particle size variation.
    float               velocity_variation;				///< Particle speed variation.
    float				accelerate_variation;			///< Particle acceleration variation.
    float       		spin_variation;					///< Particle spin variation.
    float		        motionrand_variation;			///< Particle motion randomness variation.
    float       		particleangle_variation;		///< Particle angle variation.
    float		        starttime_variation;			///< Particle start time variation.

    Curve< float >	size_curve;                         ///< Size function.
    Curve< float >	velocity_curve;                     ///< Speed function.
    Curve< float >	acceleration_curve;                 ///< Acceleration function.
    Curve< float >	spin_curve;                         ///< Spin function.
    Curve< float >	motionrand_curve;                   ///< Motion randomness function.
    Curve< gameplay::Vector4 > colors_curve;            ///< Color function.

    // TODO : think about copy behavior
    gameplay::SpriteBatch * spriteBatch;                ///< Subsystem material.
    gameplay::Rectangle sourceRect;                     ///< Source rectangle on texture (absolute).
    float aspect;                                       ///< Sprite aspect ration, calculated on load.

public:
    ParticleSubSystem();

    /**
     * Update particle by small amount of time.
     */
    void updateParticle(Particle& p, float dt) const;

    /**
     * Spawn new particle.
     */
    void spawnParticle(Particle& p, const gameplay::Matrix& transform) const;

    /**
     * Load subsystem from Properties.
     */
    bool loadFromProperties(gameplay::Properties * properties);

protected:
    bool loadFloatCurveFromProperties(gameplay::Properties * properties, Curve< float > * curve) const;
    bool loadColorCurveFromProperties(gameplay::Properties * properties, Curve< gameplay::Vector4 > * curve) const;
};





/** @brief Base particle system class.
 *
 *	@see ParticleSystem, ParticleSubSystem
 *	@author Andrew "RevEn" Karpushin
 */

class BaseParticleSystem : public gameplay::Drawable
{
public:
    BaseParticleSystem();
    virtual ~BaseParticleSystem() { };

    // compiler-generated copy constructor and assignment operator are fine

    /**
     * Get/Set emitter transformation.
     */
    gameplay::Matrix& getEmitterTransformation() { return _emitterTransformation; };

    /**
     * Get emitter transformation.
     */
    const gameplay::Matrix& getEmitterTransformation() const { return _emitterTransformation; };

    /**
     * Update particle system.
     */
    virtual void update(float dt) = 0;

    /**
     * Stop spawn particles.
     */
    void stopSpawn() { _isStopped = true; };

    /**
     * Get max particles count.
     */
    unsigned getMaxParticlesCount() const { return _maxParticles; };

    /**
     * Get particles alive count.
     */
    unsigned aliveCount() const { return _aliveCount; };

    /**
     * Is any particle alive?
     */
    bool isAlive() const { return _aliveCount != 0; };

    /**
     * Does emitter stop spawn particles?
     */
    bool isStopped() const { return _isStopped; };

    //! Get a local closest AABB.
    //const Primitives::AABB& AABB ( ) const { return _aabb; };

protected:
    unsigned            _maxParticles;						//!< Total particles count.
    gameplay::Matrix    _emitterTransformation;				//!< Emitter transformation.

    bool                _isStopped;							//!< Is spawn stopped?
    unsigned			_aliveCount;						//!< Alive particles count.
    float				_maxParticleSize;					//!< Max particle size.

    //Primitives::AABB	_aabb;								//!< Closest AABB.
};




/** @brief Particle system class.
 *	
 *	Particle system based on texture sprites.
 *
 *	@see ParticleSystem, ParticleSubSystem
 *	@author Andrew "RevEn" Karpushin

 *	Example:
 *
 *	@verbatim
  particle_system:
    update_period: 0.025
    hi_percision_update: true

    subsystem:
      < subsystem parameters >

    < more subsystems >
@endverbatim
 *	
 *	Description:
 *
 *	@b update_period		- Update period, how often particle system will be updates (default is 0.025 which means update is happened no more than 40 times per second). \n
 *	@b hi_percision_update	- High precision update. If frame delta time will be more than update_period then update will be devided into few steps (default: false). \n
 *	@b subsystem			- ParticleSubSystem description.
 *
 *	@}
 */

class ParticleSystem : public BaseParticleSystem, public Asset
{
public:
    virtual ~ParticleSystem();

    // copy constructor and assignment operator are fine.

    static Cache< ParticleSystem >& getCache()
    {
        if (!_cache)
            _cache = Cache< ParticleSystem >::create();
        return *_cache;
    }

    static ParticleSystem * create(const char * url);

    //
    // Inherited from Asset
    //

    /**
     * Get resource name.
     */
    virtual const char * getTypeName() const { return "ParticleSystem"; };

    /**
     * Reload asset.
     *
     * @return true when asset was reloaded successfully.
     * @see Asset::reload
     */
    virtual bool reload();

    /** 
     * Clone asset.
     *
     * @param[in] deepCopy Perform a deep copy (copy every underlying assets as well).
     *
     * @return Newly created asset, a copy of current one.
     */
    ParticleSystem * clone(bool deepCopy = false) const;

    gameplay::Drawable * clone(gameplay::NodeCloneContext& context) { return clone(); };

    bool loadFromProperties(gameplay::Properties * properties);


    //
    // Subsystem management.
    //

    /**
     * Get subsystems count.
     */
    size_t getSubSystemsCount() const { return _systems.size(); };

    /**
     * Add new subsystem to particle system (add as last subsystem).
     */
    void addSubSystem(const ParticleSubSystem& ps);

    /** @brief Get subsystem (const version).
     *
     *	@return NULL if index is invalid.
     */
    const ParticleSubSystem * getSubSystem(unsigned index) const;

    /** 
     * Remove subsystem by index.
     */
    void removeSubSystem(unsigned index);

    /**
     * Get particle (nonconst version).
     */
    Particle * getParticle(unsigned particle_index);

    /**
     * Get particle (const version).
     */
    const Particle * getParticle(unsigned particle_index) const;


    //
    // Behavior
    //

    /**
     * Is high precision update used?
     */
    bool isHighPrecisionUpdate() const { return (_flags & EFL_HIGH_PRECISION) != 0; };

    /**
     * Set high precision update.
     */
    void setHighPrecisionUpdate(bool hi_prec) { if (hi_prec) _flags |= EFL_HIGH_PRECISION; else _flags &= ~EFL_HIGH_PRECISION; };

    /**
     * Get update period.
     */
    float getUpdatePeriod() const { return _updatePeriod; };

    /**
     * Set update period.
     */
    void setUpdatePeriod(float period) { _updatePeriod = period; };



    //
    // General methods.
    //

    /**
     * Kill all particles.
     */
    void reset();

    /**
     * Get color modulator
     */
    const gameplay::Vector4& getColorModulator() const { return _colorModulator; };

    /**
     * Set color modulator
     */
    void setColorModulator(const gameplay::Vector4& col) { _colorModulator = col; };

    /**
     * Render particle system.
     */
    virtual unsigned int draw(bool wireframe = false) const;

    /**
     * Update particle system.
     */
    virtual void update(float dt);

private:
    ParticleSystem();
    void rawUpdate(float dt);


    // list here is more appropriate than vector, since each ParticleSubSystem is large but their count is small.
    typedef std::list< ParticleSubSystem > SystemsType;
    SystemsType _systems;

    typedef std::vector< Particle > ParticlesType;
    ParticlesType _particles;

    //! Time since last Render method was called.
    mutable float _invisibleTimer;
    int _framesToUpdate;

    enum EFlags
    {
        EFL_HIGH_PRECISION = (1 << 0)
    };

    int _flags;

    float _updatePeriod;
    float _updateTimer;
    gameplay::Vector4 _colorModulator;

    static Cache< ParticleSystem > * _cache;

    class RenderService * _renderService;
};




#endif // __DFG_PARTICLE_SYSTEM__

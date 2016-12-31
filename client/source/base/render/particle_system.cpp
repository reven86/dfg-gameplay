#include "pch.h"
#include "particle_system.h"
#include "services/render_service.h"
#include "services/service_manager.h"






//
// ParticleSubSystem
//

ParticleSubSystem::ParticleSubSystem()
    : spawn_count(0)
    , align_to_motion(false)
    , spriteBatch(NULL)
    , sourceRect(0.0f, 0.0f, 1.0f, 1.0f)
    , aspect(1.0f)
{
}

void ParticleSubSystem::updateParticle(Particle& p, float dt) const
{
    unsigned char normaltime = static_cast<unsigned char>(p.localtime / p.lifetime * 256);

    p.position += velocity_curve.key(normaltime) * dt * p.velocity;
    if (p.acceleration != 0.0f)
        p.velocity += dt * p.acceleration * acceleration_curve.key(normaltime) * accelerate_dir;
    if (p.motionrand > 0)
        p.velocity += dt * p.motionrand * motionrand_curve.key(normaltime) * gameplay::Vector3::random().normalize();
    if (p.spin != 0.0f)
        p.angle += p.spin * spin_curve.key(normaltime) * dt;
    p.size = p.start_size * size_curve.key(normaltime);
    p.color = colors_curve.key(normaltime);
}

void ParticleSubSystem::spawnParticle(Particle& p, const gameplay::Matrix& transform) const
{
    float angle = emissionrange * MATH_RANDOM_MINUS1_1();
    float cos = cosf(angle);
    float sin = sinf(angle);

    gameplay::Vector3 o;
    gameplay::Vector3::cross(velocity_dir, gameplay::Vector3::random(), &o);

    p.position = emitter_pos + gameplay::Vector3(
        volume.x * (0.5f - MATH_RANDOM_0_1()),
        volume.y * (0.5f - MATH_RANDOM_0_1()),
        volume.z * (0.5f - MATH_RANDOM_0_1())
        );

    transform.transformPoint(&p.position);

    gameplay::Vector3 dir(velocity_dir * cos + o.normalize() * sin);

    p.velocity = dir.normalize() * (velocity + velocity_variation * MATH_RANDOM_MINUS1_1());

    transform.transformVector(&p.velocity);

    p.start_size = size + size_variation * MATH_RANDOM_MINUS1_1();
    p.angle = align_to_motion ? angle : particleangle + particleangle_variation * MATH_RANDOM_MINUS1_1();
    p.spin = spin + spin_variation * MATH_RANDOM_MINUS1_1();
    p.motionrand = motionrand + motionrand_variation * MATH_RANDOM_MINUS1_1();
    p.color = colors_curve.key(0);
    p.size = p.start_size * size_curve.key(0);
    p.acceleration = acceleration + accelerate_variation * MATH_RANDOM_MINUS1_1();

    if (p.lifetime <= 0)
    {
        p.lifetime = lifetime + life_variation * MATH_RANDOM_MINUS1_1();
        p.localtime = fmodf(p.localtime, p.lifetime);
    }
    else
    {
        p.localtime = fmodf(p.localtime, p.lifetime);
        p.lifetime = lifetime + life_variation * MATH_RANDOM_MINUS1_1();
    }

    updateParticle(p, p.localtime);
}

bool ParticleSubSystem::loadFromProperties(gameplay::Properties * properties)
{
    // Check if the Properties is valid and has a valid namespace.
    if (!properties || !(strcmp(properties->getNamespace(), "subsystem") == 0))
    {
        GP_WARN("Properties object must be non-null and have namespace equal to 'subsystem'.");
        return false;
    }

    // Go through all the particle system properties and create subsystem under this system.
    gameplay::Properties* curveProperties = NULL;
    while ((curveProperties = properties->getNextNamespace()) != 0)
    {
        if (strcmp(curveProperties->getNamespace(), "size_curve") == 0)
        {
            if (!size_curve.initialize(curveProperties))
                return false;
        }
        else if (strcmp(curveProperties->getNamespace(), "velocity_curve") == 0)
        {
            if (!velocity_curve.initialize(curveProperties))
                return false;
        }
        else if (strcmp(curveProperties->getNamespace(), "acceleration_curve") == 0)
        {
            if (!acceleration_curve.initialize(curveProperties))
                return false;
        }
        else if (strcmp(curveProperties->getNamespace(), "spin_curve") == 0)
        {
            if (!spin_curve.initialize(curveProperties))
                return false;
        }
        else if (strcmp(curveProperties->getNamespace(), "motionrand_curve") == 0)
        {
            if (!motionrand_curve.initialize(curveProperties))
                return false;
        }
        else if (strcmp(curveProperties->getNamespace(), "colors_curve") == 0)
        {
            if (!colors_curve.initialize(curveProperties))
                return false;
        }
    }

    RefPtr<const SpriteBatchAsset> spriteBatchAsset = SpriteBatchAsset::getCache().load(properties->getString("material"));
    spriteBatch = spriteBatchAsset ? spriteBatchAsset->get() : (gameplay::SpriteBatch *) NULL;

    GP_ASSERT(spriteBatch && spriteBatch->getSampler() && spriteBatch->getSampler()->getTexture());

    if (spriteBatch && spriteBatch->getSampler() && spriteBatch->getSampler()->getTexture())
    {
        unsigned texW = spriteBatch->getSampler()->getTexture()->getWidth();
        unsigned texH = spriteBatch->getSampler()->getTexture()->getHeight();

        gameplay::Vector4 sourceRegion;
        if (properties->getVector4("src_rect", &sourceRegion))
        {
            sourceRect.set(sourceRegion.x / texW, sourceRegion.y / texH, sourceRegion.z / texW, sourceRegion.w / texH);
            aspect = static_cast<float>(sourceRegion.z) / sourceRegion.w;
        }
        else
        {
            sourceRect.set(0.0f, 0.0f, 1.0f, 1.0f);
            aspect = static_cast<float>(texW) / texH;
        }
    }

    properties->getVector3("emitter_pos", &emitter_pos);
    properties->getVector3("accelerate_dir", &accelerate_dir);
    properties->getVector3("velocity_dir", &velocity_dir);
    properties->getVector3("volume", &volume);
    max_particles = static_cast<unsigned>(properties->getInt("max_particles"));
    spawn_count = properties->getInt("spawn_count");

    lifetime = properties->getFloat("lifetime");
    size = properties->getFloat("size");
    velocity = properties->getFloat("velocity");
    acceleration = properties->getFloat("acceleration");
    emissionrange = MATH_DEG_TO_RAD(properties->getFloat("emissionrange"));
    spin = MATH_DEG_TO_RAD(properties->getFloat("spin"));
    motionrand = properties->getFloat("motionrand");
    particleangle = MATH_DEG_TO_RAD(properties->getFloat("particleangle"));
    align_to_motion = properties->getBool("align_to_motion", false);

    life_variation = properties->getFloat("life_variation");
    size_variation = properties->getFloat("size_variation");
    velocity_variation = properties->getFloat("velocity_variation");
    accelerate_variation = properties->getFloat("accelerate_variation");
    spin_variation = MATH_DEG_TO_RAD(properties->getFloat("spin_variation"));
    motionrand_variation = properties->getFloat("motionrand_variation");
    particleangle_variation = MATH_DEG_TO_RAD(properties->getFloat("particleangle_variation"));
    starttime_variation = properties->getFloat("starttime_variation");

    accelerate_dir.normalize();
    velocity_dir.normalize();

    return true;
}




//
// BaseParticleSystem
//
BaseParticleSystem::BaseParticleSystem()
    : _isStopped(false)
    , _aliveCount(0)
    , _maxParticles(0)
    , _emitterTransformation(gameplay::Matrix::identity())
    , _maxParticleSize(0.0f)
{
}




//
// ParticleSystem
//

Cache< ParticleSystem > * ParticleSystem::_cache = nullptr;

ParticleSystem::ParticleSystem()
    : _invisibleTimer(0)
    , _framesToUpdate(0)
    , _flags(0)
    , _updatePeriod(0.025f)
    , _updateTimer(0)
    , _colorModulator(gameplay::Vector4::one())
{
    _renderService = ServiceManager::getInstance()->findService< RenderService >();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::addSubSystem(const ParticleSubSystem& ps)
{
    _maxParticles += ps.max_particles;

    _systems.push_back(ps);
    _particles.resize(_maxParticles, Particle());

    reset();
}

void ParticleSystem::removeSubSystem(unsigned index)
{
    if (index >= _systems.size())
        return;

    SystemsType::iterator it = _systems.begin();
    std::advance(it, index);

    _maxParticles -= (*it).max_particles;
    _systems.erase(it);

    _particles.resize(_maxParticles, Particle());

    reset();
}

const ParticleSubSystem * ParticleSystem::getSubSystem(unsigned index) const
{
    if (index >= _systems.size())
        return NULL;

    SystemsType::const_iterator it = _systems.begin();
    std::advance(it, index);

    return &(*it);
}

Particle * ParticleSystem::getParticle(unsigned particle_index)
{
    if (particle_index >= _particles.size())
        return NULL;

    ParticlesType::iterator it = _particles.begin();
    std::advance(it, particle_index);

    return &(*it);
}

const Particle * ParticleSystem::getParticle(unsigned particle_index) const
{
    if (particle_index >= _particles.size())
        return NULL;

    ParticlesType::const_iterator it = _particles.begin();
    std::advance(it, particle_index);

    return &(*it);
}

void ParticleSystem::reset()
{
    ParticlesType::iterator pit = _particles.begin();

    for (SystemsType::iterator sit = _systems.begin(); sit != _systems.end(); sit++)
    {
        float dt = (*sit).lifetime / (*sit).max_particles * (*sit).starttime_variation;
        float t = 0;
        for (unsigned i = 0; i < (*sit).max_particles; i++, pit++, t -= dt)
        {
            (*pit).localtime = t;
            (*pit).lifetime = 0;
            (*pit).spawn_count = 0;
            (*pit).visible = false;
        }
    }

    _isStopped = false;
}

unsigned int ParticleSystem::draw(bool wireframe) const
{
    PROFILE("ParticleSystem::draw", "Render");

    static gameplay::Matrix identityTransform;
    const gameplay::Matrix * transform(getNode() ? &getNode()->getWorldViewMatrix() : &identityTransform);

    _invisibleTimer = 0;

    bool modulateColor = _colorModulator != gameplay::Vector4::one();

    ParticlesType::const_iterator pit = _particles.begin();
    for (SystemsType::const_iterator it = _systems.begin(), end_it = _systems.end(); it != end_it; it++)
    {
        const ParticleSubSystem& subSystem = *it;

        GP_ASSERT(subSystem.spriteBatch && "Material is absent!");
        unsigned max_particles = subSystem.max_particles;

        if (_node)
            subSystem.spriteBatch->setProjectionMatrix(_node->getProjectionMatrix());
        subSystem.spriteBatch->start();

        const float& aspect = subSystem.aspect;

        for (unsigned i = 0; i < max_particles; i++, pit++)
        {
            const Particle& par = *pit;

            if (par.visible)
            {
                gameplay::Vector3 pos;
                transform->transformPoint(par.position, &pos);
                
                gameplay::Vector3 sizeVector;
                transform->transformVector(gameplay::Vector3(par.size, par.size, par.size), &sizeVector);
                float size = sizeVector.length() * 0.707106f;   // we need to ignore stretching in one axis because 
                                                                // particles are always faced to camera, so use sqrt(0.5f) instead sqrt(0.3333f)

                if (par.color.w != 0)
                    subSystem.spriteBatch->draw(
                    pos.x, pos.y, pos.z,
                    size * aspect, size,
                    subSystem.sourceRect.left(), 1.0f - subSystem.sourceRect.bottom(), subSystem.sourceRect.right(), 1.0f - subSystem.sourceRect.top(),
                    modulateColor ? gameplay::Vector4(_colorModulator.x * par.color.x, _colorModulator.y * par.color.y, _colorModulator.z * par.color.z, _colorModulator.w * par.color.w) : par.color,
                    gameplay::Vector2(0.5f, 0.5f), par.angle, true);
            }
        }

        subSystem.spriteBatch->finish();
    }

    return static_cast<unsigned int>(_systems.size());
}

void ParticleSystem::update(float dt)
{
    PROFILE("ParticleSystem::Update", "Application");

    _updateTimer -= dt;
    if (_updateTimer > 0)
        return;

    if ((_flags & EFL_HIGH_PRECISION) != 0 && -_updateTimer > _updatePeriod)
    {
        float delta = -_updateTimer;

        while (delta > _updatePeriod)
        {
            rawUpdate(_updatePeriod);
            delta -= _updatePeriod;
        }

        rawUpdate(_updatePeriod + delta);
        _updateTimer = _updatePeriod;
        return;
    }

    dt = _updatePeriod - _updateTimer;
    _updateTimer = _updatePeriod;

    _invisibleTimer += dt;
    if (_invisibleTimer > 0.5f)
    {
        if (--_framesToUpdate > 0)
            return;

        _framesToUpdate = 3;
        dt *= 3.0f;
    }

    rawUpdate(dt);
}

void ParticleSystem::rawUpdate(float dt)
{
    PROFILE("ParticleSystem::PureUpdate", "Application");

    _aliveCount = 0;
    _maxParticleSize = 0.0f;

    //mAABB =	Primitives::AABB::Null( );

    ParticlesType::iterator pit = _particles.begin();
    for (SystemsType::const_iterator it = _systems.begin(); it != _systems.end(); it++)
    {
        int max_particles = static_cast<int>((*it).max_particles);

#pragma omp parallel
        {

#pragma omp for
            for (int i = 0; i < max_particles; i++)
            {
                Particle& p = *(&(*pit) + i);

                p.localtime += dt;

                if (p.localtime < 0.0f)
                {
                }
                else if (p.localtime > p.lifetime)
                {
                    if (!_isStopped && ((*it).spawn_count <= 0 || p.spawn_count++ < (*it).spawn_count))
                    {
                        (*it).spawnParticle(p, _emitterTransformation);
                        p.visible = true;
                    }
                    else
                        p.visible = false;
                }
                else    // 0.0 <= p.localtime <= p.lifetime
                {
                    (*it).updateParticle(p, dt);
                    p.visible = true;
                }

                if (p.visible)
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

        std::advance(pit, max_particles);
    }

    //mAABB.EnlargeSize( Vec3( mMaxParticleSize * GetScaler( ) ) );
}

ParticleSystem * ParticleSystem::create(const char * url)
{
    // Load the particle system properties from file.
    gameplay::Properties* properties = gameplay::Properties::create(url);
    if (properties == NULL)
        return NULL;

    ParticleSystem * res = new ParticleSystem();
    if(!res->loadFromProperties((strlen(properties->getNamespace()) > 0) ? properties : properties->getNextNamespace()))
    {
        SAFE_DELETE(properties);
        SAFE_RELEASE(res);
        return NULL;
    }

    SAFE_DELETE(properties);

    res->setURL(url);
    return res;
}

bool ParticleSystem::reload()
{
    gameplay::Properties* properties = gameplay::Properties::create(getURL());
    if (properties == NULL)
        return false;

    bool res = loadFromProperties((strlen(properties->getNamespace()) > 0) ? properties : properties->getNextNamespace());
    SAFE_DELETE(properties);

    return res;
}

ParticleSystem * ParticleSystem::clone(bool deepClone) const
{
    ParticleSystem * res = new ParticleSystem();
    res->_maxParticles = _maxParticles;
    res->_emitterTransformation = _emitterTransformation;
    res->_isStopped = _isStopped;
    res->_aliveCount = _aliveCount;
    res->_maxParticleSize = _maxParticleSize;

    res->setURL(getURL());
    res->_systems = _systems;
    res->_particles = _particles;
    res->_invisibleTimer = _invisibleTimer;
    res->_framesToUpdate = _framesToUpdate;
    res->_flags = _flags;
    res->_updatePeriod = _updatePeriod;
    res->_updateTimer = _updateTimer;
    res->_renderService = _renderService;
    res->_colorModulator = _colorModulator;

    return res;
}

bool ParticleSystem::loadFromProperties(gameplay::Properties * properties)
{
    // Check if the Properties is valid and has a valid namespace.
    if (!properties || !(strcmp(properties->getNamespace(), "particle_system") == 0))
    {
        GP_WARN("Properties object must be non-null and have namespace equal to 'particle_system'.");
        return false;
    }

    _systems.clear();
    ParticlesType().swap(_particles);

    _maxParticles = 0;
    _flags = 0;
    _updatePeriod = 0.025f;
    _updateTimer = 0;
    _invisibleTimer = 0;
    _framesToUpdate = 0;

    // Go through all the particle system properties and create subsystem under this system.
    gameplay::Properties* subsystemProperties = NULL;
    while ((subsystemProperties = properties->getNextNamespace()) != 0)
    {
        if (strcmp(subsystemProperties->getNamespace(), "subsystem") == 0)
        {
            ParticleSubSystem subsystem;

            if (!subsystem.loadFromProperties(subsystemProperties))
            {
                GP_WARN("Failed to load subsystem for particle system.");
                return false;
            }

            addSubSystem(subsystem);
        }
    }

    if (properties->getBool("hi_precision_update"))
        _flags |= EFL_HIGH_PRECISION;

    if (properties->exists("update_period"))
        _updatePeriod = properties->getFloat("update_period");

    return true;
}
#ifndef __DFG_PROFILER__
#define __DFG_PROFILER__

#include "singleton.h"
#include "noncopyable.h"




/*! \brief Simple lightweight profiler.
*
*/
class Profiler : public Singleton< Profiler >
{
    friend class Singleton< Profiler >;

    typedef std::pair< std::string, float > ObjectInfo;

    enum { MAX_GROUPS = 256 };
    enum { MAX_OBJECTS = 256 };

    enum { INVALID_OBJECT_ID = MAX_GROUPS * MAX_OBJECTS };

    ObjectInfo _registeredGroups[ MAX_GROUPS ];
    ObjectInfo _profileGroupsResult[ MAX_GROUPS ];
    unsigned _registeredGroupsCount;

    ObjectInfo _registeredObjects[ MAX_OBJECTS ];
    ObjectInfo _profileObjectsResult[ MAX_OBJECTS ];
    unsigned _registeredObjectsCount;

    std::pair< unsigned, float > _currentParents[ MAX_OBJECTS ];
    unsigned _parentsCount;

    float _frameDelta;

private:
    Profiler ( );
    ~Profiler ( );

public:
    /*! \brief Register new profile object.
    *
    *	Registration occured once in runtime for every profile object.
    *
    *	\param[in]		name		Profile object name.
    *	\param[in]		group		Profile object group.
    *
    *	\return Profile object unique ID.
    */
    unsigned RegisterProfilerObject ( const char * name, const char * group );

    /*! \brief Start profile.
    *
    *	Stores current time and push parent object onto stack
    *
    *	\param[in]		profile_object_id		Profile object id;
    */
    void StartProfiler ( const unsigned& profile_object_id );

    /*! \brief Stop profile.
    *
    *	Stop profile timer for current object and calculate it self time and all parents self time.
    */
    void StopProfiler ( );

    /*! \brief Save current results and reset profile data for new frame.
    *
    *	Saved results will be used in GetGroupStatistics and GetObjectStatistics.
    */
    void SaveProfilerResults ( );

    /*! \brief Set calculation frame rate.
    *
    *	\note Use this method each frame with current frame time to calculate results within 1 second.
    *
    *	\param[in] dt Frame time (in seconds).
    */
    void SetFrameTime ( const float& dt ) { _frameDelta = dt; };

    /*! \brief Get group statistics.
    *
    *	\param[in]		v			Visitor that accept group name as first argument and group self time.
    */
    template< class _Visitor >
    void GetGroupStatistics ( _Visitor v ) const;

    /*! \brief Get object statistics.
    *
    *	\param[in]		v			Visitor that accept object name as first argument and group self time.
    */
    template< class _Visitor >
    void GetObjectStatistics ( _Visitor v ) const;

    /*! \brief Draw performance information by objects and groups.
    *
    *	\param[in]		fnt			Font object.
    *	\param[in]		white_shd	White shader.
    */
    void DrawPerformanceInfo ( const gameplay::Font * fnt, gameplay::SpriteBatch * white_shd, float fontSize ) const;
};






/*! Start profiler timing in constructor and stop it in destructor.
 *
 */
class ProfilingObject : Noncopyable
{
public:
	ProfilingObject ( const unsigned& id ) { Profiler::Instance( ).StartProfiler( id ); };
	~ProfilingObject ( ) { Profiler::Instance( ).StopProfiler( ); };
};




//! Helper macro to easy to use profile objects
#ifndef __DISABLE_PROFILER__
#define PROFILE( name, group ) \
	static unsigned __dfg_profile_id = Profiler::Instance( ).RegisterProfilerObject( name, group ); \
	ProfilingObject __dfg_profile_obj( __dfg_profile_id );
#else
#define PROFILE( name, group )
#endif






#endif // __DFG_PROFILER__
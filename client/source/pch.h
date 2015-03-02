#ifndef __DFG_PCH__
#define __DFG_PCH__

/*!	\file pch.h 
 *	\brief Precompiled header.
 */

#define __SHIPPING__

#ifdef __SHIPPING__
#define __DISABLE_PROFILER__
#endif


#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <numeric>
#include <iterator>

#include <tuple>
#include <unordered_map>
#include <random>


#pragma warning( disable: 4127 )	// conditional expression is constant

#pragma warning( push )
#pragma warning( disable: 4100 )
#include "gameplay.h"
#include "mathutil.h"
#pragma warning( pop )

#pragma warning( push )
#pragma warning( disable: 4512 )
#include "sigc++.h"
#pragma warning( pop )



#include "utils/ref_ptr.h"
#include "utils/noncopyable.h"
#include "utils/singleton.h"
#include "utils/utils.h"
#include "utils/curve.h"
#include "utils/profiler.h"
#include "main/asset.h"
#include "main/cache.h"
#include "main/gameplay_assets.h"
#include "math/transformable.h"
#include "main/dictionary.h"




extern std::minstd_rand gameRandom;
extern Dictionary gameDictionary;       // global dictionary.

#define GAME_RANDOM_0_1( )   ( static_cast< float >( gameRandom( ) ) / gameRandom.max( ) )



#endif // __DFG_PCH__
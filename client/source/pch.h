#ifndef __DFG_GAMEPLAY_PCH__
#define __DFG_GAMEPLAY_PCH__

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




#pragma warning( disable: 4100 )	// unreferenced formal parameter
#pragma warning( disable: 4127 )	// conditional expression is constant
#pragma warning( disable: 4201 )	// nameless struct/union
#pragma warning( disable: 4458 )	// declaration hides class-member

#include "gameplay.h"

#pragma warning( push )
#pragma warning( disable: 4512 )
#include "sigc++.h"
#pragma warning( pop )

#include <zip.h>


#include "utils/ref_ptr.h"
#include "utils/noncopyable.h"
#include "utils/singleton.h"
#include "utils/utils.h"
#include "utils/curve.h"
#include "utils/profiler.h"
#include "main/asset.h"
#include "main/cache.h"
#include "main/gameplay_assets.h"
#include "main/dictionary.h"




#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef WIN32
#include <direct.h>
#include <Windows.h>
#else
#include <sys/stat.h>
#endif





extern std::minstd_rand gameRandom;
extern Dictionary gameDictionary;       // global dictionary.

#define GAME_RANDOM_0_1( )   ( static_cast< float >( gameRandom( ) ) / gameRandom.max( ) )



#endif // __DFG_GAMEPLAY_PCH__
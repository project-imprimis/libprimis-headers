/**
 * @file cube.h
 * @brief Needed for making games, along with iengine.h
 *
 * This header file, along with iengine.h and should be included when
 * making games. This file is used by both the game and the engine, and includes
 * all of the dependencies required to use the engine, such as STL and stdlib
 * headers and SDL/zlib dependencies.
 */

#ifndef CUBE_H_
#define CUBE_H_

#ifdef WIN32
    #define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <type_traits>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#include <climits>
#include <cfloat>
#include <cmath>
#include <cassert>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <utility>
#include <vector>
#include <functional>
#include <unordered_map>
#include <queue>
#include <stack>

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #ifdef _WIN32_WINNT
        #undef _WIN32_WINNT
    #endif
    #define _WIN32_WINNT 0x0500
    #include "windows.h"
    #ifndef _WINDOWS
        #define _WINDOWS
    #endif
    #ifndef __GNUC__
        #include <eh.h>
        #include <dbghelp.h>
        #include <intrin.h>
    #endif
    #define ZLIB_DLL
#endif


#include <SDL.h>

#ifdef WIN32
    #include <GL/glew.h>
#else
    #include <GL/wglew.h>
#endif

#include <GL/gl.h>


#include <zlib.h>

#include "tools.h"
#include "geom.h"
#include "ents.h"
#include "command.h"
#include "octa.h"
#include "slot.h"

#include "consts.h"

#endif /* CUBE_H_ */


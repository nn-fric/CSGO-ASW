#pragma once

#include <SDL3/SDL_platform.h>
#include <cstdlib>

#ifdef SDL_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <climits>
#define MAX_PATH PATH_MAX
#endif

#pragma once

#include <SDL3/SDL_platform.h>
#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_init.h>
#include <cstdlib>
#include <cstdint>

#ifdef SDL_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <climits>
#define MAX_PATH PATH_MAX
#endif

#define Assert SDL_assert
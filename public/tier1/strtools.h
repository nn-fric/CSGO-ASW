#pragma once

#include <SDL3/SDL_platform.h>
#include <SDL3/SDL_stdinc.h>

#ifdef SDL_PLATFORM_WINDOWS
#define CORRECT_PATH_SEPARATOR '\\'
#define INCORRECT_PATH_SEPARATOR '/'
#else
#define CORRECT_PATH_SEPARATOR '/'
#define INCORRECT_PATH_SEPARATOR '\\'
#endif

#define stricmp SDL_strcasecmp
#define strlcpy SDL_strlcpy
#define strlcat SDL_strlcat
#define strlen SDL_strlen
#define memcpy SDL_memcpy

// Force slashes of either type to be = separator character
void V_FixSlashes( char *pname, char separator = CORRECT_PATH_SEPARATOR );
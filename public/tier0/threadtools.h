#pragma once

#include <tier0/platform.h>
#include <SDL3/SDL_atomic.h>
#include <SDL3/SDL_init.h>
#include <thread>
#include <mutex>
#include <atomic>

#define MAX_THREADS_SUPPORTED 32

inline void ThreadSleep( const uint32_t duration = 0 ) { std::this_thread::sleep_for( std::chrono::milliseconds(duration) ); }
inline bool ThreadInMainThread() { return SDL_IsMainThread(); }
inline void ThreadPause() { SDL_CPUPauseInstruction(); }
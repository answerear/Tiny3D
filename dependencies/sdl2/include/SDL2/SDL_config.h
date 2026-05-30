/*
 * SDL_config.h - Platform bridge
 *
 * This file replaces the CMake-generated SDL_config.h with a platform
 * dispatcher that selects the correct platform-specific config at compile time.
 *
 * When upgrading SDL, do NOT overwrite this file. Instead, rename the
 * generated SDL_config.h to SDL_config_<platform>.h.
 */
#ifndef SDL_config_h_
#define SDL_config_h_

#ifdef __ANDROID__
#include "SDL_config_android.h"
#elif defined(_WIN32)
    #include <winapifamily.h>
    #if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
    #include "SDL_config_uwp.h"
    #else
    #include "SDL_config_windows.h"
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
    #include "SDL_config_ios.h"
    #else
    #include "SDL_config_macosx.h"
    #endif
#elif defined(__linux__)
#include "SDL_config_linux.h"
#elif defined(__EMSCRIPTEN__)
#include "SDL_config_emscripten.h"
#else
#error "Unsupported platform for SDL2"
#endif

#endif /* SDL_config_h_ */

/*
 * Created by switchblade on 2023-02-22.
 */

#pragma once

#include <dpm/debug.hpp>

#ifndef SEK_ASSERT_MSG
#define SEK_ASSERT_MSG(cnd, msg) DPM_ASSERT_MSG(cnd, msg)
#endif
#ifndef SEK_ASSERT
#define SEK_ASSERT(cnd) DPM_ASSERT(cnd)
#endif

#ifndef SEK_UNREACHABLE
#define SEK_UNREACHABLE() DPM_UNREACHABLE()
#endif
#ifndef SEK_FORCEINLINE
#define SEK_FORCEINLINE DPM_FORCEINLINE
#endif
#ifndef SEK_ASSUME
#define SEK_ASSUME(x) DPM_ASSUME(x)
#endif

#ifndef SEK_CXX_VERSION
/* On MSVC __cplusplus value is always fixed and _MSVC_LANG must be used instead. */
#ifdef _MSVC_LANG
#define SEK_CXX_VERSION _MSVC_LANG
#else
#define SEK_CXX_VERSION __cplusplus
#endif
#endif

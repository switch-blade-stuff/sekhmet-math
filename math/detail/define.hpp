/*
 * Created by switchblade on 2023-02-22.
 */

#pragma once

#ifndef DPM_INLINE_EXTENSIONS
#define DPM_INLINE_EXTENSIONS
#endif
#ifndef DPM_HANDLE_ERRORS
#define DPM_HANDLE_ERRORS
#endif

#include <dpm/simd.hpp>

#define SEK_UNREACHABLE() DPM_UNREACHABLE()
#define SEK_FORCEINLINE DPM_FORCEINLINE
#define SEK_ASSUME(x) DPM_ASSUME(x)

/* On MSVC __cplusplus value is always fixed and _MSVC_LANG must be used instead. */
#ifdef _MSVC_LANG
#define SEK_CXX_VERSION _MSVC_LANG
#else
#define SEK_CXX_VERSION __cplusplus
#endif

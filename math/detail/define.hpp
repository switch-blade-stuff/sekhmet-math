/*
 * Created by switchblade on 2023-02-22.
 */

#pragma once

#include <dpm/debug.hpp>
#include "api.hpp"

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

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
#ifdef _POSIX_VERSION
#define SEK_OS_POSIX
#endif
#endif

#ifdef SEK_OS_POSIX
#include <sys/types.h>
#elif SIZE_MAX >= UINT64_MAX
typedef std::int64_t ssize_t;
#elif SIZE_MAX >= UINT32_MAX
typedef std::int32_t ssize_t;
#elif SIZE_MAX >= UINT16_MAX
typedef std::int16_t ssize_t;
#endif

namespace sek
{
	using ssize_t = ::ssize_t;
}
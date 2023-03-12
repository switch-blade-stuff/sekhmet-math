/*
 * Created by switch_blade on 2023-02-10.
 */

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define SEK_MATH_API_HIDDEN
#if defined(_MSC_VER)
#define SEK_MATH_API_EXPORT __declspec(dllexport)
#define SEK_MATH_API_IMPORT __declspec(dllimport)
#elif defined(__clang__) || defined(__GNUC__)
#define SEK_MATH_API_EXPORT __attribute__((dllexport))
#define SEK_MATH_API_IMPORT __attribute__((dllimport))
#endif
#elif __GNUC__ >= 4
#define SEK_MATH_API_HIDDEN __attribute__((visibility("hidden")))
#define SEK_MATH_API_EXPORT __attribute__((visibility("default")))
#define SEK_MATH_API_IMPORT __attribute__((visibility("default")))
#else
#define SEK_MATH_API_HIDDEN
#define SEK_MATH_API_EXPORT
#define SEK_MATH_API_IMPORT
#endif

#if defined(SEK_MATH_EXPORT) || defined(SEK_MATH_LIB_STATIC)
#define SEK_MATH_PUBLIC SEK_MATH_API_EXPORT
#else
#define SEK_MATH_PUBLIC SEK_MATH_API_IMPORT
#endif
#define SEK_MATH_PRIVATE SEK_MATH_API_HIDDEN

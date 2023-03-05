/*
 * Created by switchblade on 2023-02-25.
 */

#pragma once

#include "fwd.hpp"

#include <dpm/math.hpp>

namespace sek::detail
{
	/* Dot product of a vector with itself is assumed to never be negative, as such no error checking is needed. */
#ifdef __SSE__
	[[nodiscard]] inline float sqrt(float dp) noexcept { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(dp))); }
	/* rsqrt breaks constant folding and is less precise than 1 / sqrt(x). */
	//[[nodiscard]] inline float rsqrt(float dp) noexcept { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(dp))); }
#endif
#ifdef __SSE2__
	[[nodiscard]] inline double sqrt(double dp) noexcept
	{
		const auto v_dp = _mm_set_sd(dp);
		return _mm_cvtsd_f64(_mm_sqrt_sd(v_dp, v_dp));
	}
#endif
	template<typename T>
	[[nodiscard]] inline T sqrt(T dp) noexcept { return static_cast<T>(std::sqrt(dp)); }
	template<typename T>
	[[nodiscard]] inline T rsqrt(T dp) noexcept { return static_cast<T>(1) / sqrt(dp); }

	template<typename T>
	[[nodiscard]] inline T fmadd(T a, T b, T c) noexcept { return a * b + c; }
	template<typename T>
	[[nodiscard]] inline T fmsub(T a, T b, T c) noexcept { return a * b - c; }
	template<typename T>
	[[nodiscard]] inline T fnmadd(T a, T b, T c) noexcept { return -(a * b) + c; }
	template<typename T>
	[[nodiscard]] inline T fnmsub(T a, T b, T c) noexcept { return -(a * b) - c; }
#if defined(__FMA__) || (defined(_MSC_VER) && defined(__AVX2__))
	[[nodiscard]] inline float fmadd(float a, float b, float c) noexcept
	{
		const auto va = _mm_set_ss(a);
		const auto vb = _mm_set_ss(b);
		const auto vc = _mm_set_ss(c);
		return _mm_cvtss_f32(_mm_fmadd_ss(va, vb, vc));
	}
	[[nodiscard]] inline float fmsub(float a, float b, float c) noexcept
	{
		const auto va = _mm_set_ss(a);
		const auto vb = _mm_set_ss(b);
		const auto vc = _mm_set_ss(c);
		return _mm_cvtss_f32(_mm_fmsub_ss(va, vb, vc));
	}
	[[nodiscard]] inline float fnmadd(float a, float b, float c) noexcept
	{
		const auto va = _mm_set_ss(a);
		const auto vb = _mm_set_ss(b);
		const auto vc = _mm_set_ss(c);
		return _mm_cvtss_f32(_mm_fnmadd_ss(va, vb, vc));
	}
	[[nodiscard]] inline float fnmsub(float a, float b, float c) noexcept
	{
		const auto va = _mm_set_ss(a);
		const auto vb = _mm_set_ss(b);
		const auto vc = _mm_set_ss(c);
		return _mm_cvtss_f32(_mm_fnmsub_ss(va, vb, vc));
	}

	[[nodiscard]] inline double fmadd(double a, double b, double c) noexcept
	{
		const auto va = _mm_set_sd(a);
		const auto vb = _mm_set_sd(b);
		const auto vc = _mm_set_sd(c);
		return _mm_cvtsd_f64(_mm_fmadd_sd(va, vb, vc));
	}
	[[nodiscard]] inline double fmsub(double a, double b, double c) noexcept
	{
		const auto va = _mm_set_sd(a);
		const auto vb = _mm_set_sd(b);
		const auto vc = _mm_set_sd(c);
		return _mm_cvtsd_f64(_mm_fmsub_sd(va, vb, vc));
	}
	[[nodiscard]] inline double fnmadd(double a, double b, double c) noexcept
	{
		const auto va = _mm_set_sd(a);
		const auto vb = _mm_set_sd(b);
		const auto vc = _mm_set_sd(c);
		return _mm_cvtsd_f64(_mm_fnmadd_sd(va, vb, vc));
	}
	[[nodiscard]] inline double fnmsub(double a, double b, double c) noexcept
	{
		const auto va = _mm_set_sd(a);
		const auto vb = _mm_set_sd(b);
		const auto vc = _mm_set_sd(c);
		return _mm_cvtsd_f64(_mm_fnmsub_sd(va, vb, vc));
	}
#endif

	template<std::integral T>
	[[nodiscard]] constexpr T slow_clz(T x) noexcept
	{
		auto mask = 1ull << std::numeric_limits<T>::digits;
		if (x == 0) [[unlikely]] return static_cast<T>(std::numeric_limits<T>::digits);
		for (T i = 0;; ++i, mask >>= 1) if (x & static_cast<T>(mask)) return i;
		SEK_UNREACHABLE();
	}
	template<std::integral T>
	[[nodiscard]] constexpr T slow_ctz(T x) noexcept
	{
		auto mask = 1ull;
		if (x == 0) [[unlikely]] return static_cast<T>(0);
		for (T i = 0;; ++i, mask <<= 1) if (x & static_cast<T>(mask)) return i;
		SEK_UNREACHABLE();
	}

	template<std::integral T>
	[[nodiscard]] inline T clz(T x) noexcept
	{
#if defined(_MSC_VER)
		[[maybe_unused]] unsigned long idx;
		if constexpr(sizeof(T) <= sizeof(unsigned long))
		{
			const auto off = static_cast<T>(std::numeric_limits<unsigned int>::digits - std::numeric_limits<T>::digits);
			if (!_BitScanReverse(&idx, static_cast<unsigned long>(x)))
				return std::numeric_limits<T>::digits;
			else
				return static_cast<T>(idx) - off;
		}
#if defined(_WIN64)
		else if constexpr(sizeof(T) == sizeof(__int64))
		{
			if (!_BitScanReverse64(&idx, static_cast<__int64>(x)))
				return std::numeric_limits<T>::digits;
			else
				return static_cast<T>(idx);
		}
#endif
#elif defined(__has_builtin) && __has_builtin(__builtin_clz)
		if (x == 0) return std::numeric_limits<T>::digits;
		if constexpr(sizeof(T) <= sizeof(unsigned int))
		{
			const auto off = static_cast<T>(std::numeric_limits<unsigned int>::digits - std::numeric_limits<T>::digits);
			return static_cast<T>(__builtin_clz(static_cast<unsigned int>(x))) - off;
		}
		else if constexpr(sizeof(T) == sizeof(unsigned long))
			return static_cast<T>(__builtin_clzl(static_cast<unsigned long>(x)));
		else if constexpr(sizeof(T) == sizeof(unsigned long long))
			return static_cast<T>(__builtin_clzll(static_cast<unsigned long long>(x)));
#endif
		return slow_clz(x);
	}
	template<std::integral T>
	[[nodiscard]] inline T ctz(T x) noexcept
	{
#if defined(_MSC_VER)
		[[maybe_unused]] unsigned long idx;
		if constexpr(sizeof(T) <= sizeof(unsigned long))
		{
			if (!_BitScanForward(&idx, static_cast<unsigned long>(x)))
				return std::numeric_limits<T>::digits;
			else
				return static_cast<T>(idx);
		}
#if defined(_WIN64)
		else if constexpr(sizeof(T) == sizeof(__int64))
		{
			if (!_BitScanForward64(&idx, static_cast<__int64>(x)))
				return std::numeric_limits<T>::digits;
			else
				return static_cast<T>(idx);
		}
#endif
#elif defined(__has_builtin) && __has_builtin(__builtin_clz)
		if (x == 0) return 0;
		if constexpr(sizeof(T) <= sizeof(unsigned int))
			return static_cast<T>(__builtin_ctz(static_cast<unsigned int>(x)));
		else if constexpr(sizeof(T) == sizeof(unsigned long))
			return static_cast<T>(__builtin_ctzl(static_cast<unsigned long>(x)));
		else if constexpr(sizeof(T) == sizeof(unsigned long long))
			return static_cast<T>(__builtin_ctzll(static_cast<unsigned long long>(x)));
#endif
		return slow_clz(x);
	}

	template<typename T>
	[[nodiscard]] inline std::pair<T, T> sincos(T x) noexcept
	{
		return {std::sin(x), std::cos(x)};
	}

#ifdef __has_builtin
#if __has_builtin(__builtin_sincosf)
	[[nodiscard]] inline std::pair<float, float> sincos(float x) noexcept
	{
		std::pair<float, float> result;
		__builtin_sincosf(x, &result.first, &result.second);
		return result;
	}
#endif
#if __has_builtin(__builtin_sincos)
	[[nodiscard]] inline std::pair<double, double> sincos(double x) noexcept
	{
		std::pair<double, double> result;
		__builtin_sincos(x, &result.first, &result.second);
		return result;
	}
#endif
#if __has_builtin(__builtin_sincosl)
	[[nodiscard]] inline std::pair<long double, long double> sincos(long double x) noexcept
	{
		std::pair<long double, long double> result;
		__builtin_sincosl(x, &result.first, &result.second);
		return result;
	}
#endif
#endif
}
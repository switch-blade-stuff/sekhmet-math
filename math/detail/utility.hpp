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
#if defined(__FMA__) || (defined(_MSC_VER) && defined(__AVX2__))
	[[nodiscard]] inline float fmadd(float a, float b, float c) noexcept
		{
			const auto va = _mm_set_ss(a);
			const auto vb = _mm_set_ss(b);
			const auto vc = _mm_set_ss(c);
			return _mm_cvtss_f32(_mm_fmadd_ss(va, vb, vc));
		}
		[[nodiscard]] inline double fmadd(double a, double b, double c) noexcept
		{
			const auto va = _mm_set_sd(a);
			const auto vb = _mm_set_sd(b);
			const auto vc = _mm_set_sd(c);
			return _mm_cvtsd_f64(_mm_fmadd_sd(va, vb, vc));
		}
		[[nodiscard]] inline float fmsub(float a, float b, float c) noexcept
		{
			const auto va = _mm_set_ss(a);
			const auto vb = _mm_set_ss(b);
			const auto vc = _mm_set_ss(c);
			return _mm_cvtss_f32(_mm_fmsub_ss(va, vb, vc));
		}
		[[nodiscard]] inline double fmsub(double a, double b, double c) noexcept
		{
			const auto va = _mm_set_sd(a);
			const auto vb = _mm_set_sd(b);
			const auto vc = _mm_set_sd(c);
			return _mm_cvtsd_f64(_mm_fmsub_sd(va, vb, vc));
		}
#endif
}
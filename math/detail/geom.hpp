/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "mbase.hpp"
#include "power.hpp"

namespace sek
{
	/** Calculates the dot product of elements in vectors \a a and \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T dot(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return hadd(a * b); }
	/** Calculates the cross product of elements in vectors \a a and \a b. */
	template<typename T, typename A>
	[[nodiscard]] inline basic_vec<T, 3, A> cross(const basic_vec<T, 3, A> &a, const basic_vec<T, 3, A> &b) noexcept
	{
		const auto a231 = shuffle<2, 3, 1>(a);
		const auto b231 = shuffle<2, 3, 1>(b);
		const auto a312 = shuffle<3, 1, 2>(a);
		const auto b312 = shuffle<3, 1, 2>(b);
		return fmsub(a231, b312, a312 * b231);
	}

	namespace detail
	{
		/* Dot product of a vector with itself is assumed to never be negative, as such no error checking is needed. */
		template<typename T>
		[[nodiscard]] inline T do_sqrt(T dp) noexcept { return static_cast<T>(std::sqrt(dp)); }
		template<typename T>
		[[nodiscard]] inline T do_rsqrt(T dp) noexcept { return static_cast<T>(1) / do_sqrt(dp); }
#ifdef __SSE__
		[[nodiscard]] inline float do_sqrt(float dp) noexcept { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(dp))); }
		[[nodiscard]] inline float do_rsqrt(float dp) noexcept { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(dp))); }
#endif
#ifdef __SSE2__
		[[nodiscard]] inline double do_sqrt(double dp) noexcept
		{
			const auto v_dp = _mm_set_sd(dp);
			return _mm_cvtsd_f64(_mm_sqrt_sd(v_dp, v_dp));
		}
#endif

		template<typename T>
		[[nodiscard]] inline T do_fma(T a, T b, T c) noexcept { return a * b + c; }
#if defined(__FMA__) || (defined(_MSC_VER) && defined(__AVX2__))
		[[nodiscard]] inline float do_fma(float a, float b, float c) noexcept
		{
			const auto va = _mm_set_ss(a);
			const auto vb = _mm_set_ss(b);
			const auto vc = _mm_set_ss(c);
			return _mm_cvtss_f32(_mm_fmadd_ss(va, vb, vc));
		}
		[[nodiscard]] inline double do_fma(double a, double b, double c) noexcept
		{
			const auto va = _mm_set_sd(a);
			const auto vb = _mm_set_sd(b);
			const auto vc = _mm_set_sd(c);
			return _mm_cvtsd_f64(_mm_fmadd_sd(va, vb, vc));
		}
#endif
	}

	/** Calculates the magnitude of vector \a x. Equivalent to `std::sqrt(dot(x, x))`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline T magn(const basic_vec<T, N, A> &x) noexcept { return detail::do_sqrt(dot(x, x)); }
	/** Calculates the Euclidean distance between vectors \a a and \a b. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline T dist(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return magn(a - b); }
	/** @copydoc magn
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline typename Promoted::value_type magn(const basic_vec<T, N, A> &x) noexcept { return magn(Promoted{x}); }
	/** @copydoc dist
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline typename Promoted::value_type dist(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return magn(Promoted{a}, Promoted{b}); }

	/** Returns normalized copy (length 1) of vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> normalize(const basic_vec<T, N, A> &x) noexcept { return x * detail::do_rsqrt(dot(x, x)); }
	/** @copydoc normalize
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted normalize(const basic_vec<T, N, A> &x) noexcept { return normalize(Promoted{x}); }

	/** Orients normal vector \a n to point in the direction specified by incident vector \a i and normal reference vector \a r.
	 * @return `dot(r, i) < 0 ? n : -n`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> faceforward(const basic_vec<T, N, A> &n, const basic_vec<T, N, A> &i, const basic_vec<T, N, A> &r) noexcept
	{
		return dot(r, i) < static_cast<T>(0) ? n : -n;
	}
	/** @copydoc normalize
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted faceforward(const basic_vec<T0, N, A> &n, const basic_vec<T1, N, A> &i, const basic_vec<T2, N, A> &r) noexcept
	{
		return faceforward(Promoted{n}, Promoted{i}, Promoted{r});
	}

	/** Calculates reflection direction for incident vector \a i and normal vector \a n.
	 * @return `i - 2 * dot(n, i) * n`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> reflect(const basic_vec<T, N, A> &i, const basic_vec<T, N, A> &n) noexcept
	{
		return fmadd(n, {dot(n, i) * static_cast<T>(-2)}, i);
	}
	/** @copydoc reflect
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted reflect(const basic_vec<T0, N, A> &i, const basic_vec<T1, N, A> &n) noexcept
	{
		return reflect(Promoted{i}, Promoted{n});
	}

	/** Calculates refraction vector for incident vector \a i and normal vector \a n using refraction ratio of indices \a e. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> refract(const basic_vec<T, N, A> &i, const basic_vec<T, N, A> &n, T e) noexcept
	{
		const auto dp = dot(n, i);
		const auto k = detail::do_fma(e * e, detail::do_fma(dp, dp, static_cast<T>(-1)), static_cast<T>(-1));
		return k >= static_cast<T>(0) ? fmsub(i, {e}, n * detail::do_fma(e, dp, detail::do_sqrt(k))) : basic_vec<T, N, A>{0};
	}
	/** @copydoc refract
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted refract(const basic_vec<T0, N, A> &i, const basic_vec<T1, N, A> &n, T2 e) noexcept
	{
		return refract(Promoted{i}, Promoted{n}, static_cast<typename Promoted::value_type>(e));
	}
}
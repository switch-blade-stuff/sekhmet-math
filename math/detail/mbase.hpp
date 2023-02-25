/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Calculates absolute value of elements in vector \a x. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> abs(const basic_vec<T, N, A> &x) noexcept { return {dpm::abs(to_simd(x))}; }
	/** @copydoc abs */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fabs(const basic_vec<T, N, A> &x) noexcept { return {dpm::fabs(to_simd(x))}; }

	/** Calculates floating-point remainder of elements in \a a divided by elements in vector \a b. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fmod(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::fmod(to_simd(a), to_simd(b))}; }
	/** Calculates IEEE remainder of elements in \a a divided by elements in vector \a b. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> remainder(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::remainder(to_simd(a), to_simd(b))}; }
	/** Calculates IEEE remainder of elements in \a a divided by elements in vector \a b, and stores the sign and
	 * at least 3 last bits of the division result in elements of \a quo. */
	template<std::floating_point T, std::size_t N, typename A, typename QA>
	[[nodiscard]] inline basic_vec<T, N, A> remquo(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<int, N, QA> &quo) noexcept
	{
		return {dpm::remquo(to_simd(a), to_simd(b), to_simd(quo))};
	}
	/** @copydoc fmod
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted fmod(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::fmod(to_simd(a), to_simd(b))}; }
	/** @copydoc remainder
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted remainder(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::remainder(to_simd(a), to_simd(b))}; }
	/** @copydoc remquo
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename QA, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted remquo(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<int, N, QA> &quo) noexcept
	{
		return {dpm::remquo(to_simd(a), to_simd(b), to_simd(quo))};
	}

	/** Calculates the maximum of elements in \a a and \a b, respecting the NaN propagation
	 * as specified in IEC 60559 (ordered values are always selected over unordered). */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fmax(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::fmax(to_simd(a), to_simd(b))}; }
	/** Calculates the minimum of elements in \a a and \a b, respecting the NaN propagation
	 * as specified in IEC 60559 (ordered values are always selected over unordered). */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fmin(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::fmin(to_simd(a), to_simd(b))}; }
	/** Returns the positive difference between elements of vectors \a a and \a b. Equivalent to `max(simd<T, Abi>{0}, a - b)`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fdim(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::fdim(to_simd(a), to_simd(b))}; }
	/** @copydoc fmax
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted fmax(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::fmax(to_simd(a), to_simd(b))}; }
	/** @copydoc fmin
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted fmin(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::fmin(to_simd(a), to_simd(b))}; }
	/** @copydoc fdim
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted fdim(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::fdim(to_simd(a), to_simd(b))}; }

	/** Preforms linear interpolation or extrapolation between elements of vectors \a a and \a b using factor \a f */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> lerp(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &f) noexcept
	{
		return {dpm::lerp(to_simd(a), to_simd(b), to_simd(f))};
	}
	/** @copydoc lerp
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted lerp(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &f) noexcept
	{
		return {dpm::lerp(to_simd(a), to_simd(b), to_simd(f))};
	}

	/** Calculates floating-point remainder of elements in vector \a a divided by scalar \a b. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fmod(const basic_vec<T, N, A> &a, T b) noexcept { return {dpm::fmod(to_simd(a), b)}; }
	/** Calculates IEEE remainder of elements in vector \a a divided by scalar \a b. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> remainder(const basic_vec<T, N, A> &a, T b) noexcept { return {dpm::remainder(to_simd(a), b)}; }
	/** @copydoc fmod
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted fmod(const basic_vec<T0, N, A> &a, T1 b) noexcept { return {dpm::fmod(to_simd(a), b)}; }
	/** @copydoc remainder
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted remainder(const basic_vec<T0, N, A> &a, T1 b) noexcept { return {dpm::remainder(to_simd(a), b)}; }

	/** Calculates the maximum of elements in vector \a a and scalar \a b, respecting the NaN propagation
	 * as specified in IEC 60559 (ordered values are always selected over unordered). */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fmax(const basic_vec<T, N, A> &a, T b) noexcept { return {dpm::fmax(to_simd(a), b)}; }
	/** Calculates the minimum of elements in vector \a a and scalar \a b, respecting the NaN propagation
	 * as specified in IEC 60559 (ordered values are always selected over unordered). */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fmin(const basic_vec<T, N, A> &a, T b) noexcept { return {dpm::fmin(to_simd(a), b)}; }
	/** Returns the positive difference between elements of vector \a a and scalar \a b. Equivalent to `max(simd<T, Abi>{0}, a - b)`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fdim(const basic_vec<T, N, A> &a, T b) noexcept { return {dpm::fdim(to_simd(a), b)}; }
	/** @copydoc fmax
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted fmax(const basic_vec<T0, N, A> &a, T1 b) noexcept { return {dpm::fmax(to_simd(a), b)}; }
	/** @copydoc fmin
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted fmin(const basic_vec<T0, N, A> &a, T1 b) noexcept { return {dpm::fmin(to_simd(a), b)}; }
	/** @copydoc fdim
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted fdim(const basic_vec<T0, N, A> &a, T1 b) noexcept { return {dpm::fdim(to_simd(a), b)}; }

	/** Preforms linear interpolation or extrapolation between elements of vectors \a a and \a b using scalar factor \a f */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> lerp(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, T f) noexcept
	{
		return {dpm::lerp(to_simd(a), to_simd(b), f)};
	}
	/** @copydoc lerp
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted lerp(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, T2 f) noexcept
	{
		return {dpm::lerp(to_simd(a), to_simd(b), to_simd(f))};
	}

	/** Returns a result of fused multiply-add operation on elements of \a a, \a b, and \a c. Equivalent to `a * b + c`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fmadd(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &c) noexcept
	{
		return {dpm::fmadd(to_simd(a), to_simd(b), to_simd(c))};
	}
	/** Returns a result of fused multiply-sub operation on elements of \a a, \a b, and \a c. Equivalent to `a * b - c`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fmsub(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &c) noexcept
	{
		return {dpm::fmadd(to_simd(a), to_simd(b), to_simd(c))};
	}
	/** Returns a result of fused negate-multiply-add operation on elements of \a a, \a b, and \a c. Equivalent to `-(a * b) + c`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fnmadd(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &c) noexcept
	{
		return {dpm::fmadd(to_simd(a), to_simd(b), to_simd(c))};
	}
	/** Returns a result of fused negate-multiply-sub operation on elements of \a a, \a b, and \a c. Equivalent to `-(a * b) - c`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fnmsub(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &c) noexcept
	{
		return {dpm::fmadd(to_simd(a), to_simd(b), to_simd(c))};
	}
	/** @copydoc fmadd */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> fma(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &c) noexcept
	{
		return {dpm::fma(to_simd(a), to_simd(b), to_simd(c))};
	}

	/** @copydoc fmadd
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted fmadd(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &c) noexcept
	{
		return {dpm::fmadd(to_simd(a), to_simd(b), to_simd(c))};
	}
	/** @copydoc fmsub
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted fmsub(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &c) noexcept
	{
		return {dpm::fmadd(to_simd(a), to_simd(b), to_simd(c))};
	}
	/** @copydoc fnmadd
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted fnmadd(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &c) noexcept
	{
		return {dpm::fmadd(to_simd(a), to_simd(b), to_simd(c))};
	}
	/** @copydoc fnmsub
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted fnmsub(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &c) noexcept
	{
		return {dpm::fmadd(to_simd(a), to_simd(b), to_simd(c))};
	}
	/** @copydoc fmadd */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted fma(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &c) noexcept
	{
		return {dpm::fma(to_simd(a), to_simd(b), to_simd(c))};
	}
}
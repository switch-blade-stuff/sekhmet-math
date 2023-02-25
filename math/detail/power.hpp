/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Calculates square root of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> sqrt(const basic_vec<T, N, A> &x) noexcept { return {dpm::sqrt(to_simd(x))}; }
	/** Calculates cube root of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> cbrt(const basic_vec<T, N, A> &x) noexcept { return {dpm::cbrt(to_simd(x))}; }
	/** Calculates reciprocal square root of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> rsqrt(const basic_vec<T, N, A> &x) noexcept { return {dpm::rsqrt(to_simd(x))}; }

	/** @copydoc sqrt
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted sqrt(const basic_vec<T, N, A> &x) noexcept { return {dpm::sqrt(to_simd(x))}; }
	/** @copydoc cbrt
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted cbrt(const basic_vec<T, N, A> &x) noexcept { return {dpm::cbrt(to_simd(x))}; }
	/** @copydoc rsqrt
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted rsqrt(const basic_vec<T, N, A> &x) noexcept { return {dpm::rsqrt(to_simd(x))}; }

	/** Calculates square root of the sum of elements in vectors \a a and \a b without causing over or underflow. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> hypot(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::hypot(to_simd(a), to_simd(b))}; }
	/** @copydoc hypot
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted hypot(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::hypot(to_simd(a), to_simd(b))}; }

	/** Raises elements of vector \a x to power specified by elements of vector \a p. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> pow(const basic_vec<T, N, A> &x, const basic_vec<T, N, A> &p) noexcept { return {dpm::pow(to_simd(x), to_simd(p))}; }
	/** Raises elements of vector \a x to power \a p. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> pow(const basic_vec<T, N, A> &x, T p) noexcept { return {dpm::pow(to_simd(x), p)}; }

	/** @copydoc pow
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted pow(const basic_vec<T0, N, A> &x, const basic_vec<T1, N, A> &p) noexcept { return {dpm::pow(to_simd(x), to_simd(p))}; }
	/** @copydoc pow
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted pow(const basic_vec<T0, N, A> &x, T1 p) noexcept { return {dpm::pow(to_simd(x), p)}; }

	/** Calculates reciprocal of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> rcp(const basic_vec<T, N, A> &x) noexcept { return {dpm::rcp(to_simd(x))}; }
	/** @copydoc rcp
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted rcp(const basic_vec<T, N, A> &x) noexcept { return {dpm::rcp(to_simd(x))}; }
}
/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Raises *e* (Euler's number) to the power specified by elements of vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> exp(const basic_vec<T, N, A> &x) noexcept { return {dpm::exp(to_simd(x))}; }
	/** Raises `2` to the power specified by elements of vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> exp2(const basic_vec<T, N, A> &x) noexcept { return {dpm::exp2(to_simd(x))}; }
	/** Raises *e* (Euler's number) to the power specified by elements of vector \a x, and subtracts `1`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> expm1(const basic_vec<T, N, A> &x) noexcept { return {dpm::expm1(to_simd(x))}; }
	/** Calculates natural (base_t *e*) logarithm of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> log(const basic_vec<T, N, A> &x) noexcept { return {dpm::log(to_simd(x))}; }
	/** Calculates natural (base_t *e*) logarithm of elements in vector \a x plus `1`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> log1p(const basic_vec<T, N, A> &x) noexcept { return {dpm::log1p(to_simd(x))}; }
	/** Calculates common (base_t 10) logarithm of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> log10(const basic_vec<T, N, A> &x) noexcept { return {dpm::log10(to_simd(x))}; }
	/** Calculates binary (base_t 2) logarithm of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> log2(const basic_vec<T, N, A> &x) noexcept { return {dpm::log2(to_simd(x))}; }

	/** @copydoc exp
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted exp(const basic_vec<T, N, A> &x) noexcept { return {dpm::exp(to_simd(x))}; }
	/** @copydoc exp2
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted exp2(const basic_vec<T, N, A> &x) noexcept { return {dpm::exp2(to_simd(x))}; }
	/** @copydoc expm1
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted expm1(const basic_vec<T, N, A> &x) noexcept { return {dpm::expm1(to_simd(x))}; }
	/** @copydoc log
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted log(const basic_vec<T, N, A> &x) noexcept { return {dpm::log(to_simd(x))}; }
	/** @copydoc log1p
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted log1p(const basic_vec<T, N, A> &x) noexcept { return {dpm::log1p(to_simd(x))}; }
	/** @copydoc log10
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted log10(const basic_vec<T, N, A> &x) noexcept { return {dpm::log10(to_simd(x))}; }
	/** @copydoc log2
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted log2(const basic_vec<T, N, A> &x) noexcept { return {dpm::log2(to_simd(x))}; }
}
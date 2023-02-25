/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Calculates the error function of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> erf(const basic_vec<T, N, A> &x) noexcept { return {dpm::erf(to_simd(x))}; }
	/** Calculates the complementary error function of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> erfc(const basic_vec<T, N, A> &x) noexcept { return {dpm::erfc(to_simd(x))}; }
	/** Calculates the gamma function of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> tgamma(const basic_vec<T, N, A> &x) noexcept { return {dpm::tgamma(to_simd(x))}; }
	/** Calculates the natural logarithm of the absolute value of the gamma function of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> lgamma(const basic_vec<T, N, A> &x) noexcept { return {dpm::lgamma(to_simd(x))}; }

	/** @copydoc erf
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted erf(const basic_vec<T, N, A> &x) noexcept { return {dpm::erf(to_simd(x))}; }
	/** @copydoc erfc
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted erfc(const basic_vec<T, N, A> &x) noexcept { return {dpm::erfc(to_simd(x))}; }
	/** @copydoc tgamma
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted tgamma(const basic_vec<T, N, A> &x) noexcept { return {dpm::tgamma(to_simd(x))}; }
	/** @copydoc lgamma
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted lgamma(const basic_vec<T, N, A> &x) noexcept { return {dpm::lgamma(to_simd(x))}; }
}
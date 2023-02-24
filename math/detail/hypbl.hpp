/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Calculates hyperbolic sine of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> sinh(const basic_vec<T, N, A> &x) noexcept { return {dpm::sinh(to_simd(x))}; }
	/** Calculates hyperbolic cosine of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> cosh(const basic_vec<T, N, A> &x) noexcept { return {dpm::cosh(to_simd(x))}; }
	/** Calculates hyperbolic tangent of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> tanh(const basic_vec<T, N, A> &x) noexcept { return {dpm::tanh(to_simd(x))}; }
	/** Calculates hyperbolic arc-sine of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> asinh(const basic_vec<T, N, A> &x) noexcept { return {dpm::asinh(to_simd(x))}; }
	/** Calculates hyperbolic arc-cosine of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> acosh(const basic_vec<T, N, A> &x) noexcept { return {dpm::acosh(to_simd(x))}; }
	/** Calculates hyperbolic arc-tangent of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> atanh(const basic_vec<T, N, A> &x) noexcept { return {dpm::atanh(to_simd(x))}; }

	/** @copydoc sinh
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted sinh(const basic_vec<T, N, A> &x) noexcept { return {dpm::sinh(to_simd(x))}; }
	/** @copydoc cosh
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted cosh(const basic_vec<T, N, A> &x) noexcept { return {dpm::cosh(to_simd(x))}; }
	/** @copydoc tanh
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted tanh(const basic_vec<T, N, A> &x) noexcept { return {dpm::tanh(to_simd(x))}; }
	/** @copydoc asinh
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted asinh(const basic_vec<T, N, A> &x) noexcept { return {dpm::asinh(to_simd(x))}; }
	/** @copydoc acosh
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted acosh(const basic_vec<T, N, A> &x) noexcept { return {dpm::acosh(to_simd(x))}; }
	/** @copydoc atanh
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted atanh(const basic_vec<T, N, A> &x) noexcept { return {dpm::atanh(to_simd(x))}; }
}
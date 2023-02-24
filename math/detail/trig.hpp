/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Calculates sine of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> sin(const basic_vec<T, N, A> &x) noexcept { return {dpm::sin(to_simd(x))}; }
	/** Calculates cosine of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> cos(const basic_vec<T, N, A> &x) noexcept { return {dpm::cos(to_simd(x))}; }
	/** Calculates tangent of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> tan(const basic_vec<T, N, A> &x) noexcept { return {dpm::tan(to_simd(x))}; }
	/** Calculates arc-sine of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> asin(const basic_vec<T, N, A> &x) noexcept { return {dpm::asin(to_simd(x))}; }
	/** Calculates arc-cosine of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> acos(const basic_vec<T, N, A> &x) noexcept { return {dpm::acos(to_simd(x))}; }
	/** Calculates arc-tangent of elements in vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> atan(const basic_vec<T, N, A> &x) noexcept { return {dpm::atan(to_simd(x))}; }
	/** Calculates arc-tangent of quotient of elements in vectors \a a and \a b. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> atan2(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::atan2(to_simd(a), to_simd(b))}; }
	/** Calculates sine and cosine of elements in vector \a x, and assigns results to elements of \a out_sin and \a out_cos respectively. */
	template<std::floating_point T, std::size_t N, typename A>
	DPM_FORCEINLINE void sincos(const basic_vec<T, N, A> &x, basic_vec<T, N, A> &out_sin, basic_vec<T, N, A> &out_cos) noexcept { dpm::atan(to_simd(x), to_simd(out_sin), to_simd(out_cos)); }

	/** @copydoc sin
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted sin(const basic_vec<T, N, A> &x) noexcept { return {dpm::sin(to_simd(x))}; }
	/** @copydoc cos
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted cos(const basic_vec<T, N, A> &x) noexcept { return {dpm::cos(to_simd(x))}; }
	/** @copydoc tan
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted tan(const basic_vec<T, N, A> &x) noexcept { return {dpm::tan(to_simd(x))}; }
	/** @copydoc asin
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted asin(const basic_vec<T, N, A> &x) noexcept { return {dpm::asin(to_simd(x))}; }
	/** @copydoc acos
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted acos(const basic_vec<T, N, A> &x) noexcept { return {dpm::acos(to_simd(x))}; }
	/** @copydoc atan
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted atan(const basic_vec<T, N, A> &x) noexcept { return {dpm::atan(to_simd(x))}; }
	/** @copydoc atan
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] DPM_FORCEINLINE Promoted atan2(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::atan2(to_simd(a), to_simd(b))}; }
}
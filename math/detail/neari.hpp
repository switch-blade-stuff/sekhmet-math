/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Rounds elements of vector \a x to nearest integer not less than the element's value. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> ceil(const basic_vec<T, N, A> &x) noexcept { return {dpm::ceil(to_simd(x))}; }
	/** Rounds elements of vector \a x to nearest integer not greater than the element's value. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> floor(const basic_vec<T, N, A> &x) noexcept { return {dpm::floor(to_simd(x))}; }
	/** Rounds elements of vector \a x to integer with truncation. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> trunc(const basic_vec<T, N, A> &x) noexcept { return {dpm::trunc(to_simd(x))}; }
	/** Rounds elements of vector \a x to integer using current rounding mode. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> nearbyint(const basic_vec<T, N, A> &x) noexcept { return {dpm::nearbyint(to_simd(x))}; }

	/** Rounds elements of vector \a x to nearest integer rounding away from zero. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> round(const basic_vec<T, N, A> &x) noexcept { return {dpm::round(to_simd(x))}; }
	/** Casts elements of vector \a x to integer of type \a I rounding away from zero. */
	template<std::signed_integral I, std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<I, N, A> iround(const basic_vec<T, N, A> &x) noexcept { return {dpm::iround<I>(to_simd(x))}; }
	/** Casts elements of vector \a x to `long` rounding away from zero. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<long, N, A> lround(const basic_vec<T, N, A> &x) noexcept { return {dpm::lround(to_simd(x))}; }
	/** Casts elements of vector \a x to `long long` rounding away from zero. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<long long, N, A> llround(const basic_vec<T, N, A> &x) noexcept { return {dpm::llround(to_simd(x))}; }

	/** Rounds elements of vector \a x to nearest integer using current rounding mode with exceptions. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> rint(const basic_vec<T, N, A> &x) noexcept { return {dpm::rint(to_simd(x))}; }
	/** Casts elements of vector \a x to integer of type \a I using current rounding mode with exceptions. */
	template<std::signed_integral I, std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<I, N, A> irint(const basic_vec<T, N, A> &x) noexcept { return {dpm::irint<I>(to_simd(x))}; }
	/** Casts elements of vector \a x to `long` using current rounding mode with exceptions. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<long, N, A> lrint(const basic_vec<T, N, A> &x) noexcept { return {dpm::lrint(to_simd(x))}; }
	/** Casts elements of vector \a x to `long long` using current rounding mode with exceptions. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<long long, N, A> llrint(const basic_vec<T, N, A> &x) noexcept { return {dpm::llrint(to_simd(x))}; }

	/** Casts elements of vector \a x to signed integer type \a I with truncation. */
	template<std::signed_integral I, std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<I, N, A> itrunc(const basic_vec<T, N, A> &x) noexcept { return {dpm::itrunc<I>(to_simd(x))}; }
	/** Casts elements of vector \a x to `long` with truncation. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<long, N, A> ltrunc(const basic_vec<T, N, A> &x) noexcept { return {dpm::ltrunc(to_simd(x))}; }
	/** Casts elements of vector \a x to `long long` with truncation. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE vec<long long, N, A> lltrunc(const basic_vec<T, N, A> &x) noexcept { return {dpm::lltrunc(to_simd(x))}; }
}
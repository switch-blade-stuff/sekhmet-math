/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Classifies elements of vector \a x, returning one of one of `FP_INFINITE`, `FP_NAN`, `FP_NORMAL`, `FP_SUBNORMAL`, `FP_ZERO`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec<int, N, A> fpclassify(const basic_vec<T, N, A> &x) noexcept { return {dpm::fpclassify(to_simd(x))}; }
	/** Determines if elements of \a x are finite. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> isfinite(const basic_vec<T, N, A> &x) noexcept { return {dpm::isfinite(to_simd(x))}; }
	/** Determines if elements of \a x are infinite. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> isinf(const basic_vec<T, N, A> &x) noexcept { return {dpm::isinf(to_simd(x))}; }
	/** Determines if elements of \a x are unordered NaN. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> isnan(const basic_vec<T, N, A> &x) noexcept { return {dpm::isnan(to_simd(x))}; }
	/** Determines if elements of \a x are normal. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> isnormal(const basic_vec<T, N, A> &x) noexcept { return {dpm::isnormal(to_simd(x))}; }
	/** Extracts a vector mask filled with sign bits of elements from vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> signbit(const basic_vec<T, N, A> &x) noexcept { return {dpm::signbit(to_simd(x))}; }

	/** @copydoc fpclassify
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline vec<int, N, A> fpclassify(const basic_vec<T, N, A> &x) noexcept { return {dpm::fpclassify(to_simd(x))}; }
	/** @copydoc isfinite
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T>, N, A> isfinite(const basic_vec<T, N, A> &x) noexcept { return {dpm::isfinite(to_simd(x))}; }
	/** @copydoc isinf
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T>, N, A> isinf(const basic_vec<T, N, A> &x) noexcept { return {dpm::isinf(to_simd(x))}; }
	/** @copydoc isnan
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T>, N, A> isnan(const basic_vec<T, N, A> &x) noexcept { return {dpm::isnan(to_simd(x))}; }
	/** @copydoc isnormal
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T>, N, A> isnormal(const basic_vec<T, N, A> &x) noexcept { return {dpm::isnormal(to_simd(x))}; }
	/** @copydoc signbit
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T>, N, A> signbit(const basic_vec<T, N, A> &x) noexcept { return {dpm::signbit(to_simd(x))}; }

	/** Determines if elements of \a a are greater than elements of \a b without setting floating-point exceptions. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> isgreater(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::isgreater(to_simd(a), to_simd(b))}; }
	/** Determines if elements of \a a are greater than or equal to elements of \a b without setting floating-point exceptions. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> isgreaterequal(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::isgreaterequal(to_simd(a), to_simd(b))}; }
	/** Determines if elements of \a a are less than elements of \a b without setting floating-point exceptions. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> isless(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::isless(to_simd(a), to_simd(b))}; }
	/** Determines if elements of \a a are less than or equal to elements of \a b without setting floating-point exceptions. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> islessequal(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::islessequal(to_simd(a), to_simd(b))}; }
	/** Determines if elements of \a a are less than or greater than elements of \a b without setting floating-point exceptions. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> islessgreater(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::islessgreater(to_simd(a), to_simd(b))}; }
	/** Determines is either elements of \a a or \a b are unordered and returns the resulting mask. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> isunordered(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::isunordered(to_simd(a), to_simd(b))}; }

	/** @copydoc isgreater
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T0, T1>, N, A> isgreater(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::isgreater(to_simd(a), to_simd(b))}; }
	/** @copydoc isgreaterequal
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T0, T1>, N, A> isgreaterequal(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::isgreaterequal(to_simd(a), to_simd(b))}; }
	/** @copydoc isless
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T0, T1>, N, A> isless(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::isless(to_simd(a), to_simd(b))}; }
	/** @copydoc islessequal
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T0, T1>, N, A> islessequal(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::islessequal(to_simd(a), to_simd(b))}; }
	/** @copydoc islessgreater
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T0, T1>, N, A> islessgreater(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::islessgreater(to_simd(a), to_simd(b))}; }
	/** @copydoc isunordered
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<detail::promote_t<T0, T1>, N, A> isunordered(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return {dpm::isunordered(to_simd(a), to_simd(b))}; }
}
/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "math_fwd.hpp"

namespace sek
{
	/** Decomposes elements of vector \a x into a normalized fraction and a power-of-two exponent, stores the exponent in \a exp, and returns the fraction. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> frexp(const basic_vec<T, N, A> &x, const basic_vec<int, N, A> &exp) noexcept { return {dpm::frexp(to_simd(x), to_simd(exp))}; }
	/** Decomposes elements of vector \a x into integral and fractional parts, returning the fractional and storing the integral in \a ip. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> modf(const basic_vec<T, N, A> &x, basic_vec<int, N, A> &ip) noexcept { return {dpm::modf(to_simd(x), to_simd(ip))}; }
	/** @copydoc frexp
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted frexp(const basic_vec<T, N, A> &x, basic_vec<int, N, A> &exp) noexcept { return {dpm::frexp(to_simd(x), to_simd(exp))}; }

	/** Multiplies elements of vector \a x by `2` raised to power specified by elements of vector \a exp. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> ldexp(const basic_vec<T, N, A> &x, const basic_vec<int, N, A> &exp) noexcept { return {dpm::ldexp(to_simd(x), to_simd(exp))}; }
	/** Multiplies elements of vector \a x by `FLT_RADIX` raised to power specified by elements of vector \a exp. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> scalbn(const basic_vec<T, N, A> &x, const basic_vec<int, N, A> &exp) noexcept { return {dpm::scalbn(to_simd(x), to_simd(exp))}; }
	/** @copydoc scalbn */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> scalbln(const basic_vec<T, N, A> &x, const basic_vec<int, N, A> &exp) noexcept { return {dpm::scalbln(to_simd(x), to_simd(exp))}; }
	/** @copydoc ldexp
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted ldexp(const basic_vec<T, N, A> &x, const basic_vec<int, N, A> &exp) noexcept { return {dpm::ldexp(to_simd(x), to_simd(exp))}; }
	/** @copydoc scalbn
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted scalbn(const basic_vec<T, N, A> &x, const basic_vec<int, N, A> &exp) noexcept { return {dpm::scalbn(to_simd(x), to_simd(exp))}; }
	/** @copydoc scalbln
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted scalbln(const basic_vec<T, N, A> &x, const basic_vec<int, N, A> &exp) noexcept { return {dpm::scalbln(to_simd(x), to_simd(exp))}; }

	/** Extracts unbiased exponent of elements in vector \a x as integers. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec<int, N, A> ilogb(const basic_vec<T, N, A> &x) noexcept { return {dpm::ilogb(to_simd(x))}; }
	/** Extracts unbiased exponent of elements in vector \a x as floats. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> logb(const basic_vec<T, N, A> &x) noexcept { return {dpm::logb(to_simd(x))}; }
	/** @copydoc ilogb
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline vec<int, N, A> ilogb(const basic_vec<T, N, A> &x) noexcept { return {dpm::ilogb(to_simd(x))}; }
	/** @copydoc logb
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted logb(const basic_vec<T, N, A> &x) noexcept { return {dpm::logb(to_simd(x))}; }

	/** Finds next representable value from elements of vector \a from to elements of vector \a to. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> nextafter(const basic_vec<T, N, A> &from, const basic_vec<T, N, A> &to) noexcept { return {dpm::nextafter(to_simd(from), to_simd(to))}; }
	/** Finds next representable value from elements of vector \a from to elements of vector \a to without loss of precision. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> nexttoward(const basic_vec<T, N, A> &from, const basic_vec<long double, N, A> &to) noexcept { return {dpm::nexttoward(to_simd(from), to_simd(to))}; }
	/** @copydoc nextafter
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted nextafter(const basic_vec<T0, N, A> &from, const basic_vec<T1, N, A> &to) noexcept { return {dpm::nextafter(to_simd(from), to_simd(to))}; }
	/** @copydoc nextafter
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted nexttoward(const basic_vec<T0, N, A> &from, const basic_vec<long double, N, A> &to) noexcept { return {dpm::nexttoward(to_simd(from), to_simd(to))}; }

	/** Copies sign bit from elements of vector \a sign to elements of vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> copysign(const basic_vec<T, N, A> &x, const basic_vec<T, N, A> &sign) noexcept { return {dpm::copysign(to_simd(x), to_simd(sign))}; }
	/** @copydoc copysign
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted copysign(const basic_vec<T0, N, A> &x, const basic_vec<T1, N, A> &sign) noexcept { return {dpm::copysign(to_simd(x), to_simd(sign))}; }

	/** Multiplies elements of vector \a x by `2` raised to power \a exp. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> ldexp(const basic_vec<T, N, A> &x, int exp) noexcept { return {dpm::ldexp(to_simd(x), exp)}; }
	/** Multiplies elements of vector \a x by `FLT_RADIX` raised to power \a exp. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> scalbn(const basic_vec<T, N, A> &x, int exp) noexcept { return {dpm::scalbn(to_simd(x), exp)}; }
	/** @copydoc scalbn */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> scalbln(const basic_vec<T, N, A> &x, long exp) noexcept { return {dpm::scalbln(to_simd(x), exp)}; }
	/** Copies sign bit from \a sign to elements of vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> copysign(const basic_vec<T, N, A> &x, T sign) noexcept { return {dpm::copysign(to_simd(x), sign)}; }
	/** @copydoc ldexp
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted ldexp(const basic_vec<T, N, A> &x, int exp) noexcept { return {dpm::ldexp(to_simd(x), exp)}; }
	/** @copydoc scalbn
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted scalbn(const basic_vec<T, N, A> &x, int exp) noexcept { return {dpm::scalbn(to_simd(x), exp)}; }
	/** @copydoc scalbln
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted scalbln(const basic_vec<T, N, A> &x, long exp) noexcept { return {dpm::scalbln(to_simd(x), exp)}; }
	/** @copydoc copysign
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted copysign(const basic_vec<T0, N, A> &x, T1 sign) noexcept { return {dpm::copysign(to_simd(x), sign)}; }
}
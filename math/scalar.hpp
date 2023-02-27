/*
 * Created by switchblade on 2023-02-26.
 */

#pragma once

#include "detail/utility.hpp"

namespace sek
{
	/** Returns result of fused multiply-add of \a a, \a b and \a c. Equivalent to `a * b + c`. */
	template<typename T>
	[[nodiscard]] constexpr T fmadd(T a, T b, T c) noexcept
	{
		if (!std::is_constant_evaluated())
			return detail::fmadd(a, b, c);
		else
			return a * b + c;
	}
	/** Returns result of fused multiply-subtract of \a a, \a b and \a c. Equivalent to `a * b - c`. */
	template<typename T>
	[[nodiscard]] constexpr T fmsub(T a, T b, T c) noexcept
	{
		if (!std::is_constant_evaluated())
			return detail::fmsub(a, b, c);
		else
			return a * b - c;
	}
	/** Returns result of fused negate-multiply-add of \a a, \a b and \a c. Equivalent to `-(a * b) + c`. */
	template<typename T>
	[[nodiscard]] constexpr T fnmadd(T a, T b, T c) noexcept
	{
		if (!std::is_constant_evaluated())
			return detail::fnmadd(a, b, c);
		else
			return -(a * b) + c;
	}
	/** Returns result of fused negate-multiply-sub of \a a, \a b and \a c. Equivalent to `-(a * b) - c`. */
	template<typename T>
	[[nodiscard]] constexpr T fnmsub(T a, T b, T c) noexcept
	{
		if (!std::is_constant_evaluated())
			return detail::fnmsub(a, b, c);
		else
			return -(a * b) - c;
	}

	/** Calculates sine and cosine of \a x. */
	template<typename T>
	inline void sincos(T x, T &out_sin, T &out_cos) noexcept { std::bind(out_sin, out_cos) = detail::sincos(x); }

	/** Calculates reciprocal square root of \a x. */
	template<typename T>
	[[nodiscard]] inline T rsqrt(T x) noexcept { return detail::rsqrt(x); }

	/** Converts radian angle \a x to degrees. */
	template<typename T>
	[[nodiscard]] constexpr T deg(T x) noexcept { return x * T{180} / std::numbers::pi_v<T>; }
	/** Converts degree angle \a x to radians. */
	template<typename T>
	[[nodiscard]] constexpr T rad(T x) noexcept { return x * std::numbers::pi_v<T> / T{180}; }

	/** Counts leading zeros of integer \a x */
	template<std::integral T>
	[[nodiscard]] constexpr T clz(T x) noexcept
	{
		if (std::is_constant_evaluated())
			return detail::slow_clz(x);
		else
			return detail::clz(x);
	}
	/** Counts trailing zeros of integer \a x */
	template<std::integral T>
	[[nodiscard]] constexpr T ctz(T x) noexcept
	{
		if (std::is_constant_evaluated())
			return detail::slow_ctz(x);
		else
			return detail::ctz(x);
	}
	/** Calculates base-2 logarithm of integer \a x. If \a x is `0` returns `-1`. */
	template<std::integral T>
	[[nodiscard]] constexpr T ilog2(T x) noexcept { return std::numeric_limits<T>::digits - clz(x) - 1; }
}
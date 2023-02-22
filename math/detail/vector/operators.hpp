/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#include "type.hpp"

namespace sek::math
{
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE vec<T, N, Abi> operator+(const vec<T, N, Abi> &a, const vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) + to_simd(b); })
	{
		return {to_simd(a) + to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE vec<T, N, Abi> operator-(const vec<T, N, Abi> &a, const vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) - to_simd(b); })
	{
		return {to_simd(a) - to_simd(b)};
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE vec<T, N, Abi> operator*(const vec<T, N, Abi> &a, const vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) * to_simd(b); })
	{
		return {to_simd(a) * to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE vec<T, N, Abi> operator/(const vec<T, N, Abi> &a, const vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) / to_simd(b); })
	{
		return {to_simd(a) / to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE vec<T, N, Abi> operator%(const vec<T, N, Abi> &a, const vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) % to_simd(b); })
	{
		return {to_simd(a) % to_simd(b)};
	}
}
/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#include "type.hpp"

namespace sek::math
{
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator+(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) + to_simd(b); })
	{
		return {to_simd(a) + to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator-(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) - to_simd(b); })
	{
		return {to_simd(a) - to_simd(b)};
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator+(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a + basic_vec<T, N, Abi>{b}; }) { return a + basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator-(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a - basic_vec<T, N, Abi>{b}; }) { return a - basic_vec<T, N, Abi>{b}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator*(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) * to_simd(b); })
	{
		return {to_simd(a) * to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator/(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) / to_simd(b); })
	{
		return {to_simd(a) / to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator%(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) % to_simd(b); })
	{
		return {to_simd(a) % to_simd(b)};
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator*(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a * basic_vec<T, N, Abi>{b}; }) { return a * basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator/(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a / basic_vec<T, N, Abi>{b}; }) { return a / basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator%(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a % basic_vec<T, N, Abi>{b}; }) { return a % basic_vec<T, N, Abi>{b}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator==(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) == to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator!=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) != to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator<=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) <= to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator>=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) >= to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator<(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) < to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator>(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) > to_simd(b)};
	}
}
/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#include <utility>

#include "define.hpp"

namespace sek
{
	namespace abi { using namespace dpm::simd_abi; }
	using dpm::is_abi_tag;
	using dpm::is_abi_tag_v;

	template<typename T, std::size_t N, typename Abi>
	class basic_vec_mask;

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_vec_mask<T, N, Abi>::simd_type &to_simd(basic_vec_mask<T, N, Abi> &x) noexcept;
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_vec_mask<T, N, Abi>::simd_type &to_simd(const basic_vec_mask<T, N, Abi> &x) noexcept;

	template<typename T, std::size_t N, typename Abi>
	class basic_vec;

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_vec<T, N, Abi>::simd_type &to_simd(basic_vec<T, N, Abi> &x) noexcept;
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_vec<T, N, Abi>::simd_type &to_simd(const basic_vec<T, N, Abi> &x) noexcept;

	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	class basic_mat;
}

template<typename T, std::size_t N, typename Abi>
struct std::tuple_size<sek::basic_vec_mask<T, N, Abi>> : std::integral_constant<std::size_t, N> {};
template<std::size_t I, typename T, std::size_t N, typename Abi>
struct std::tuple_element<I, sek::basic_vec_mask<T, N, Abi>> { using type = typename sek::basic_vec_mask<T, N, Abi>::value_type; };

template<typename T, std::size_t N, typename Abi>
struct std::tuple_size<sek::basic_vec<T, N, Abi>> : std::integral_constant<std::size_t, N> {};
template<std::size_t I, typename T, std::size_t N, typename Abi>
struct std::tuple_element<I, sek::basic_vec<T, N, Abi>> { using type = typename sek::basic_vec<T, N, Abi>::value_type; };

template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
struct std::tuple_size<sek::basic_mat<T, NCols, NRows, Abi>> : std::integral_constant<std::size_t, NCols> {};
template<std::size_t I, typename T, std::size_t NCols, std::size_t NRows, typename Abi>
struct std::tuple_element<I, sek::basic_mat<T, NCols, NRows, Abi>> { using type = typename sek::basic_mat<T, NCols, NRows, Abi>::col_type; };
/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

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
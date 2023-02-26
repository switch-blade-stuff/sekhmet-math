/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#include <utility>

#ifndef DPM_INLINE_EXTENSIONS
#define DPM_INLINE_EXTENSIONS
#endif
#ifndef DPM_HANDLE_ERRORS
#define DPM_HANDLE_ERRORS
#endif

#include <dpm/type.hpp>

#include "define.hpp"

namespace sek
{
	namespace math_abi { using namespace dpm::simd_abi; }

	/** @brief Helper type used to check if `T` is an ABI tag. */
	template<typename T>
	using is_abi_tag = dpm::is_abi_tag<T>;
	/** @brief Alias for `is_abi_tag<T>::value`. */
	template<typename T>
	inline constexpr auto is_abi_tag_v = is_abi_tag<T>::value;

	/** @brief Helper type used to obtain vector size of the ABI tag. */
	template<typename T, typename Abi>
	using abi_tag_size = dpm::simd_size<T, Abi>;
	/** @brief Alias for `abi_tag_size<T, Abi>::value`. */
	template<typename T, typename Abi>
	inline constexpr auto abi_tag_size_v = abi_tag_size<T, Abi>::value;

	template<typename T, std::size_t N, typename Abi>
	class basic_vec_mask;
	template<typename T, std::size_t N, typename Abi>
	class basic_vec;
	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	class basic_mat;
	template<typename T, typename Abi>
	class basic_quat;
	template<typename T, typename Abi>
	class basic_rect;

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_vec_mask<T, N, Abi>::simd_type &to_simd(basic_vec_mask<T, N, Abi> &x) noexcept;
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_vec_mask<T, N, Abi>::simd_type &to_simd(const basic_vec_mask<T, N, Abi> &x) noexcept;

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_vec<T, N, Abi>::simd_type &to_simd(basic_vec<T, N, Abi> &x) noexcept;
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_vec<T, N, Abi>::simd_type &to_simd(const basic_vec<T, N, Abi> &x) noexcept;
}

template<typename T, std::size_t N, typename Abi>
struct std::tuple_size<sek::basic_vec_mask<T, N, Abi>> : std::integral_constant<std::size_t, N> {};
template<std::size_t I, typename T, std::size_t N, typename Abi> requires(I < N)
struct std::tuple_element<I, sek::basic_vec_mask<T, N, Abi>> { using type = typename sek::basic_vec_mask<T, N, Abi>::value_type; };

template<typename T, std::size_t N, typename Abi>
struct std::tuple_size<sek::basic_vec<T, N, Abi>> : std::integral_constant<std::size_t, N> {};
template<std::size_t I, typename T, std::size_t N, typename Abi> requires(I < N)
struct std::tuple_element<I, sek::basic_vec<T, N, Abi>> { using type = typename sek::basic_vec<T, N, Abi>::value_type; };

template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
struct std::tuple_size<sek::basic_mat<T, NCols, NRows, Abi>> : std::integral_constant<std::size_t, NCols> {};
template<std::size_t I, typename T, std::size_t NCols, std::size_t NRows, typename Abi> requires(I < NCols)
struct std::tuple_element<I, sek::basic_mat<T, NCols, NRows, Abi>> { using type = typename sek::basic_mat<T, NCols, NRows, Abi>::col_type; };

template<typename T, typename Abi>
struct std::tuple_size<sek::basic_rect<T, Abi>> : std::integral_constant<std::size_t, 2> {};
template<std::size_t I, typename T, typename Abi> requires (I < 2)
struct std::tuple_element<I, sek::basic_rect<T, Abi>> { using type = typename sek::basic_rect<T, Abi>::vector_type; };

template<typename T, typename Abi>
struct std::tuple_size<sek::basic_quat<T, Abi>> : std::integral_constant<std::size_t, 4> {};
template<std::size_t I, typename T, typename Abi> requires (I < 2)
struct std::tuple_element<I, sek::basic_quat<T, Abi>> { using type = typename sek::basic_quat<T, Abi>::value_type; };
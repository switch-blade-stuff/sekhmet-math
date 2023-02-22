/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#ifndef DPM_INLINE_EXTENSIONS
#define DPM_INLINE_EXTENSIONS
#endif
#ifndef DPM_HANDLE_ERRORS
#define DPM_HANDLE_ERRORS
#endif

#include <dpm/simd.hpp>

#define SEK_FORCEINLINE DPM_FORCEINLINE

namespace sek::math
{
	namespace abi { using namespace dpm::simd_abi; }
	using dpm::is_abi_tag;
	using dpm::is_abi_tag_v;

	template<typename T, std::size_t N, typename Abi = abi::fixed_size<N>>
	class vec_mask;

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename vec_mask<T, N, Abi>::simd_type &to_simd(vec_mask<T, N, Abi> &x) noexcept;
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename vec_mask<T, N, Abi>::simd_type &to_simd(const vec_mask<T, N, Abi> &x) noexcept;

	template<typename T, std::size_t N, typename Abi = abi::fixed_size<N>>
	class vec;

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename vec<T, N, Abi>::simd_type &to_simd(vec<T, N, Abi> &x) noexcept;
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename vec<T, N, Abi>::simd_type &to_simd(const vec<T, N, Abi> &x) noexcept;
}
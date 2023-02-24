/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "mbase.hpp"

namespace sek
{
	/** Calculates the dot product of elements in vectors \a a and \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE T dot(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return hadd(a * b); }
	/** Calculates the cross product of elements in vectors \a a and \a b. */
	template<typename T, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, 3, A> cross(const basic_vec<T, 3, A> &a, const basic_vec<T, 3, A> &b) noexcept
	{
		const auto a231 = shuffle<2, 3, 1>(a);
		const auto b231 = shuffle<2, 3, 1>(b);
		const auto a312 = shuffle<3, 1, 2>(a);
		const auto b312 = shuffle<3, 1, 2>(b);
		return fmsub(a231, b312, a312 * b231);
	}
}
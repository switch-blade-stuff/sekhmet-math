/*
 * Created by switchblade on 2023-02-24.
 */

#pragma once

#include "math_fwd.hpp"
#include "type_mat.hpp"

namespace sek
{
	/** Creates a 4x4 translation matrix using input matrix \a m and a translation vector \a v.
	 * @param m Input matrix to multiply by the resulting translation.
	 * @param v Translation coordinates vector. */
	template<typename T, typename AM, typename AV>
	[[nodiscard]] inline basic_mat<T, 4, 4, AM> translate(const basic_mat<T, 4, 4, AM> &m, const basic_vec<T, 3, AV> &v)
	{
		auto result = m;
		auto c3 = fmadd(result[2], {v[2]}, result[3]);
		c3 = fmadd(result[1], {v[1]}, c3);
		c3 = fmadd(result[0], {v[0]}, c3);
		result[3] = c3;
		return result;
	}
}
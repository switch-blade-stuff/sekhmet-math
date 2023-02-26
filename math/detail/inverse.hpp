/*
 * Created by switchblade on 2023-02-25.
 */

#pragma once

#include "type_mat.hpp"
#include "utility.hpp"

namespace sek
{
	/** Calculates the 2x2 inverse matrix of \a x. */
	template<typename T, typename A>
	[[nodiscard]] inline basic_mat<T, 2, 2, A> inverse(const basic_mat<T, 2, 2, A> &x) noexcept
	{
		const auto k = static_cast<T>(1) / (detail::fmsub(x[0][0], x[1][1], x[1][0] * x[0][1]));
		const auto c0 = basic_vec<T, 2, A>{x[1][1] * k, -x[0][1] * k};
		const auto c1 = basic_vec<T, 2, A>{-x[1][0] * k, x[0][0] * k};
		return {c0, c1};
	}
	/** Calculates the 3x3 inverse matrix of \a x. */
	template<typename T, typename A>
	[[nodiscard]] inline basic_mat<T, 3, 3, A> inverse(const basic_mat<T, 3, 3, A> &x) noexcept
	{
		const auto a0 = basic_vec<T, 3, A>{x[1][0], x[0][0], x[0][0]};
		const auto a1 = basic_vec<T, 3, A>{x[1][1], x[0][1], x[0][1]};
		const auto a2 = basic_vec<T, 3, A>{x[1][2], x[0][2], x[0][2]};
		const auto b0 = basic_vec<T, 3, A>{x[2][0], x[2][0], x[1][0]};
		const auto b1 = basic_vec<T, 3, A>{x[2][1], x[2][1], x[1][1]};
		const auto b2 = basic_vec<T, 3, A>{x[2][2], x[2][2], x[1][2]};

		const auto vk = detail::fmsub(a1, b2, b1 * a2);
		const auto k = detail::fmsub(x[0][0], vk[0], detail::fmadd(x[1][0], vk[1], x[2][0] * vk[2]));
		const auto ik = static_cast<T>(1) / k;

		basic_mat<T, 3, 3, A> result;
		result[0] = fmsub(a1, b2, b1 * a2) * basic_vec<T, 3, A>{ik, -ik, ik};
		result[1] = fmsub(a0, b2, b0 * a2) * basic_vec<T, 3, A>{-ik, ik, -ik};
		result[2] = fmsub(a0, b1, b0 * a1) * basic_vec<T, 3, A>{ik, -ik, ik};
		return result;
	}
	/** Calculates the 4x4 inverse matrix of \a x. */
	template<typename T, typename A>
	[[nodiscard]] inline basic_mat<T, 4, 4, A> inverse(const basic_mat<T, 4, 4, A> &x) noexcept
	{
		using vec2_t = basic_vec<T, 2, math_abi::deduce_t<T, 2, A>>;

		const auto c1r0 = vec2_t{x[1][0]};
		const auto c1r1 = vec2_t{x[1][1]};
		const auto c1r2 = vec2_t{x[1][2]};
		const auto c1r3 = vec2_t{x[1][3]};

		const auto c2r0 = vec2_t{x[2][0]};
		const auto c2r1 = vec2_t{x[2][1]};
		const auto c2r2 = vec2_t{x[2][2]};
		const auto c2r3 = vec2_t{x[2][3]};

		const auto c3r0 = vec2_t{x[3][0]};
		const auto c3r1 = vec2_t{x[3][1]};
		const auto c3r2 = vec2_t{x[3][2]};
		const auto c3r3 = vec2_t{x[3][3]};

		const auto c32r0 = vec2_t{x[3][0], x[2][0]};
		const auto c32r1 = vec2_t{x[3][1], x[2][1]};
		const auto c32r2 = vec2_t{x[3][2], x[2][2]};
		const auto c32r3 = vec2_t{x[3][3], x[2][3]};

		const auto a0 = fmsub(c2r2, c3r3, c3r2 * c2r3);
		const auto c0 = fmsub(c1r2, c32r3, c32r2 * c1r3);
		const auto f0 = basic_vec<T, 4, A>{a0, c0};

		const auto a1 = fmsub(c2r1, c3r3, c3r1 * c2r3);
		const auto c1 = fmsub(c1r1, c32r2, c32r1 * c1r2);
		const auto f1 = basic_vec<T, 4, A>{a1, c1};

		const auto a2 = fmsub(c2r1, c3r2, c3r1 * c2r2);
		const auto c2 = fmsub(c1r1, c32r3, c32r1 * c1r3);
		const auto f2 = basic_vec<T, 4, A>{a2, c2};

		const auto b0 = fmsub(c2r0, c3r1, c3r0 * c2r1);
		const auto c3 = fmsub(c1r0, c32r3, c32r0 * c1r3);
		const auto f3 = basic_vec<T, 4, A>{b0, c3};

		const auto b1 = fmsub(c2r0, c3r2, c3r0 * c2r2);
		const auto c4 = fmsub(c1r0, c32r2, c32r0 * c1r2);
		const auto f4 = basic_vec<T, 4, A>{b1, c4};

		const auto b2 = fmsub(c2r0, c3r3, c3r0 * c2r3);
		const auto c5 = fmsub(c1r0, c32r1, c32r0 * c1r1);
		const auto f5 = basic_vec<T, 4, A>{b2, c5};

		const auto v0 = basic_vec<T, 4, A>{x[1][0], x[0][0], x[0][0], x[0][0]};
		const auto v1 = basic_vec<T, 4, A>{x[1][1], x[0][1], x[0][1], x[0][1]};
		const auto v2 = basic_vec<T, 4, A>{x[1][2], x[0][2], x[0][2], x[0][2]};
		const auto v3 = basic_vec<T, 4, A>{x[1][3], x[0][3], x[0][3], x[0][3]};

		const auto s0 = basic_vec<T, 4, A>{1, -1, 1, -1};
		const auto s1 = basic_vec<T, 4, A>{-1, 1, -1, 1};
		const auto col0 = fmsub(v1, f0, fmadd(v2, f1, v3 * f2)) * s0;
		const auto col1 = fmsub(v0, f0, fmadd(v2, f3, v3 * f4)) * s1;
		const auto col2 = fmsub(v0, f1, fmadd(v1, f3, v3 * f5)) * s0;
		const auto col3 = fmsub(v0, f2, fmadd(v1, f4, v2 * f5)) * s1;
		const auto row0 = basic_vec<T, 4, A>{col0[0], col1[0], col2[0], col3[0]};

		const auto k = static_cast<T>(1) / hadd(x[0] * row0);
		return {col0 * k, col1 * k, col2 * k, col3 * k};
	}
}

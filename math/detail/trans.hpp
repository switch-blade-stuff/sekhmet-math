/*
 * Created by switchblade on 2023-02-24.
 */

#pragma once

#include "type_mat.hpp"
#include "geom.hpp"

namespace sek
{
	namespace detail
	{
		template<typename T, typename MA, typename VA>
		[[nodiscard]] inline basic_mat<T, 4, 4, MA> impl_look_at_rh(const basic_vec<T, 3, VA> &org, const basic_vec<T, 3, VA> &dir, const basic_vec<T, 3, VA> &up) noexcept
		{
			const auto f = normalize(dir - org);
			const auto s = normalize(cross(f, up));
			const auto u = cross(s, f);
			const auto nf = -f;

			auto result = basic_mat<T, 4, 4, MA>{1};
			result[0] = basic_vec<T, 3, VA>{s[0], u[0], nf[0]};
			result[1] = basic_vec<T, 3, VA>{s[1], u[1], nf[1]};
			result[2] = basic_vec<T, 3, VA>{s[2], u[2], nf[2]};
			result[3][0] = -dot(s, org);
			result[3][1] = -dot(u, org);
			result[3][2] = dot(f, org);
			return result;
		}
		template<typename T, typename MA, typename VA>
		[[nodiscard]] inline basic_mat<T, 4, 4, MA> impl_look_at_lh(const basic_vec<T, 3, VA> &org, const basic_vec<T, 3, VA> &dir, const basic_vec<T, 3, VA> &up) noexcept
		{
			const auto f = normalize(dir - org);
			const auto s = normalize(cross(up, f));
			const auto u = cross(f, s);

			auto result = basic_mat<T, 4, 4, MA>{1};
			result[0] = basic_vec<T, 3, VA>{s[0], u[0], f[0]};
			result[1] = basic_vec<T, 3, VA>{s[1], u[1], f[1]};
			result[2] = basic_vec<T, 3, VA>{s[2], u[2], f[2]};
			result[3][0] = -dot(s, org);
			result[3][1] = -dot(u, org);
			result[3][2] = -dot(f, org);
			return result;
		}
	}

	/** Translates a 4x4 transform matrix \a m using translation vector \a v.
	 * @param m Transform matrix to translate.
	 * @param v Translation vector.
	 * @return 4x4 transform matrix with translation applied. */
	template<typename T, typename AM, typename AV = math_abi::deduce_t<T, 3, AM>>
	[[nodiscard]] inline basic_mat<T, 4, 4, AM> translate(const basic_mat<T, 4, 4, AM> &m, const basic_vec<T, 3, AV> &v) noexcept
	{
		basic_mat<T, 4, 4, AM> result;
		auto c3 = fmadd(m[2], {v[2]}, m[3]);
		c3 = fmadd(m[1], {v[1]}, c3);
		c3 = fmadd(m[0], {v[0]}, c3);
		result[3] = c3;
		return result;
	}
	/** Rotates a 4x4 transform matrix \a m about normalized axis \a v using angle \a a.
	 * @param m Transform matrix to rotate.
	 * @param a Rotation angle.
	 * @param v Rotation axis vector.
	 * @return 4x4 transform matrix with rotation applied.
	 * @note Rotation axis must be normalized. */
	template<typename T, typename AM, typename AV = math_abi::deduce_t<T, 3, AM>>
	[[nodiscard]] inline basic_mat<T, 4, 4, AM> rotate(const basic_mat<T, 4, 4, AM> &m, T a, const basic_vec<T, 3, AV> &v) noexcept
	{
		const auto [a_sin, a_cos] = detail::sincos(a);
		const auto temp = v * (T{1} - a_cos);

		const auto a0 = basic_vec<T, 3, AV>{a_cos, a_sin, -a_sin};
		const auto a1 = basic_vec<T, 3, AV>{-a_sin, a_cos, a_sin};
		const auto a2 = basic_vec<T, 3, AV>{a_sin, -a_sin, a_cos};

		/* {1, axis[2], axis[1]} */
		auto b0 = shuffle<0, 2, 1>(v);
		b0[0] = static_cast<T>(1);
		/* {axis[2], 1, axis[0]} */
		auto b1 = shuffle<2, 1, 0>(v);
		b1[1] = static_cast<T>(1);
		/* {axis[1], axis[0], 1} */
		auto b2 = shuffle<1, 0, 2>(v);
		b2[2] = static_cast<T>(1);

		/* Build a temporary rotation-only matrix. */
		basic_mat<T, 3, 3, AV> rot;
		rot[0] = fmadd(v, {temp[0]}, a0 * b0);
		rot[1] = fmadd(v, {temp[1]}, a1 * b1);
		rot[2] = fmadd(v, {temp[2]}, a2 * b2);

		/* Apply the rotation matrix to m. */
		basic_mat<T, 4, 4, AM> result;
		result[0] = fmadd(m[0], {rot[0][0]}, fmadd(m[1], {rot[0][1]}, m[2] * rot[0][2]));
		result[1] = fmadd(m[0], {rot[1][0]}, fmadd(m[1], {rot[1][1]}, m[2] * rot[1][2]));
		result[2] = fmadd(m[0], {rot[2][0]}, fmadd(m[1], {rot[2][1]}, m[2] * rot[2][2]));
		result[3] = m[3];
		return result;
	}
	/** Scales a 4x4 transform matrix \a m using scale vector \a v.
	 * @param m Transform matrix to scale.
	 * @param v Scale vector.
	 * @return 4x4 transform matrix with scale applied. */
	template<typename T, typename AM, typename AV = math_abi::deduce_t<T, 3, AM>>
	[[nodiscard]] inline basic_mat<T, 4, 4, AM> scale(const basic_mat<T, 4, 4, AM> &m, const basic_vec<T, 3, AV> &v) noexcept
	{
		basic_mat<T, 4, 4, AM> result;
		result[0] = m[0] * v[0];
		result[1] = m[1] * v[1];
		result[2] = m[2] * v[2];
		result[3] = m[3];
		return result;
	}

	/** Applies a shear to a 4x4 transform matrix \a m using 3 shearing points.
	 * @param m Transform matrix to shear.
	 * @param v Reference point vector.
	 * @param rx Matrix projection ratio in the YZ plane.
	 * @param ry Matrix projection ratio in the XZ plane.
	 * @param rz Matrix projection ratio in the XY plane.
	 * @return 4x4 transform matrix with shear applied. */
	template<typename T, typename AM, typename AP = math_abi::deduce_t<T, 3, AM>, typename AS = math_abi::deduce_t<T, 2, AP>>
	[[nodiscard]] inline basic_mat<T, 4, 4, AM> shear(const basic_mat<T, 4, 4, AM> &m, const basic_vec<T, 3, AP> &v, const basic_vec<T, 2, AS> &rx, const basic_vec<T, 2, AS> &ry, const basic_vec<T, 2, AS> &rz) noexcept
	{
		const auto lxy = rx[0];
		const auto lxz = rx[1];
		const auto lyx = ry[0];
		const auto lyz = ry[1];
		const auto lzx = rz[0];
		const auto lzy = rz[1];
		const auto l = basic_vec<T, 3, AP>{lxy + lxz, lyx + lyz, lzx + lzy};

		const auto s0 = basic_vec<T, 4, AM>{1, lyx, lzx, 0};
		const auto s1 = basic_vec<T, 4, AM>{lxy, 1, lzy, 0};
		const auto s2 = basic_vec<T, 4, AM>{lxz, lyz, 1, 0};
		const auto s3 = basic_vec<T, 4, AM>{-l[0] * v[0], -l[1] * v[1], -l[2] * v[2], 1};

		basic_mat<T, 4, 4, AM> result;
		result[0] = fmadd(s0, {m[0][0]}, fmadd(s1, {m[0][1]}, fmadd(s2, {m[0][2]}, s3 * m[0][3])));
		result[1] = fmadd(s0, {m[1][0]}, fmadd(s1, {m[1][1]}, fmadd(s2, {m[1][2]}, s3 * m[1][3])));
		result[2] = fmadd(s0, {m[2][0]}, fmadd(s1, {m[2][1]}, fmadd(s2, {m[2][2]}, s3 * m[2][3])));
		result[3] = fmadd(s0, {m[3][0]}, fmadd(s1, {m[3][1]}, fmadd(s2, {m[3][2]}, s3 * m[3][3])));
		return result;
	}
}
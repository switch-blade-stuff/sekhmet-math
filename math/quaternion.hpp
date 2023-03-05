/*
 * Created by switchblade on 2023-02-26.
 */

#pragma once

#include "vector.hpp"
#include "matrix.hpp"

namespace sek
{
	/** @brief Structure used to define a mathematical quaternion.
	 * @tparam T Value type stored by the quaternion.
	 * @tparam Abi ABI tag used for the underlying storage of the quaternion.
	 * @note The \a Abi tag size must be `4`. */
	template<std::floating_point T, typename Abi>
	class basic_quat
	{
	public:
		using vector_type = basic_vec<T, 4, Abi>;
		using mask_type = basic_vec_mask<T, 4, Abi>;
		using value_type = typename vector_type::value_type;

	private:
		/* Making this a separate function returning a vector results in better codegen. */
		template<std::size_t N, typename A>
		[[nodiscard]] static inline vector_type from_matrix(const basic_mat<T, N, N, A> &x) noexcept
		{
			const auto a = x[0][0] + x[1][1] + x[2][2];
			if (const auto d = x[2][2] - x[0][0] - x[1][1]; d > a)
			{
				const auto max = detail::sqrt(d + T{1}) * T{0.5};
				const auto k = T{0.25} / max;
				return {(x[0][1] - x[1][0]) * k, (x[2][0] + x[0][2]) * k, (x[1][2] + x[2][1]) * k, max};
			}
			if (const auto c = x[1][1] - x[0][0] - x[2][2]; c > a)
			{
				const auto max = detail::sqrt(c + T{1}) * T{0.5};
				const auto k = T{0.25} / max;
				return {(x[2][0] - x[0][2]) * k, (x[0][1] + x[1][0]) * k, max, (x[1][2] + x[2][1]) * k};
			}
			if (const auto b = x[0][0] - x[1][1] - x[2][2]; b > a)
			{
				const auto max = detail::sqrt(b + T{1}) * T{0.5};
				const auto k = T{0.25} / max;
				return {(x[1][2] - x[2][1]) * k, max, (x[0][1] + x[1][0]) * k, (x[2][0] + x[0][2]) * k};
			}

			const auto max = detail::sqrt(a + T{1}) * T{0.5};
			const auto k = T{0.25} / max;
			return {max, (x[1][2] - x[2][1]) * k, (x[2][0] - x[0][2]) * k, (x[0][1] - x[1][0]) * k};
		}

	public:
		/** Creates a quaternion rotation from a vector of euler angles.
		 * @note It is recommended to use `packed_vec` if euler angles are known at compile-time
		 * in order to enable compile-time evaluation of quaternion constants. */
		template<typename A = math_abi::deduce_t<T, 3, Abi>>
		[[nodiscard]] static basic_quat from_euler(const basic_vec<T, 3, A> &angles) noexcept
		{
			basic_vec<T, 3, A> sin_x, cos_x;
			sincos(angles * T{0.5}, sin_x, cos_x);
			const auto a = cos_x[1] * cos_x[2];
			const auto b = sin_x[1] * cos_x[2];
			const auto c = cos_x[1] * sin_x[2];
			const auto d = sin_x[1] * sin_x[2];
			const auto x = detail::fmsub(sin_x[0], a, cos_x[0] * d);
			const auto y = detail::fmadd(cos_x[0], b, sin_x[0] * c);
			const auto z = detail::fmsub(cos_x[0], c, sin_x[0] * b);
			const auto w = detail::fmadd(cos_x[0], a, sin_x[0] * d);
			return {x, y, z, w};
		}
		/** Creates a quaternion rotation from an angle and a normalized rotation axis.
		 * @param angle Angle of the rotation.
		 * @param axis Axis of the rotation. */
		template<typename A = math_abi::deduce_t<T, 3, Abi>>
		[[nodiscard]] static basic_quat angle_axis(T angle, const basic_vec<T, 3, A> &axis) noexcept
		{
			const auto a = angle * T{0.5};
			const auto [s, c] = detail::sincos(a);
			return {axis * s, c};
		}

		/** Creates a quaternion rotation used to rotate object in the look direction \a dir using the up direction \a up with default handedness.
	 	 * @param dir Direction vector to rotate the origin towards.
	 	 * @param up Normalized up vector. */
		template<typename A = math_abi::deduce_t<T, 3, Abi>>
		[[nodiscard]] static SEK_FORCEINLINE basic_quat look_at(const basic_vec<T, 3, A> &dir, const basic_vec<T, 3, A> &up = basic_vec<T, 3, A>::up()) noexcept
		{
#ifndef SEK_FORCE_LEFT_HANDED
			return look_at_rh(dir, up);
#else
			return look_at_lh(dir, up);
#endif
		}
		/** Creates a right-handed quaternion rotation used to rotate an object in the look direction \a dir using the up direction \a up.
		 * @param dir Direction vector to rotate the origin towards.
		 * @param up Normalized up vector. */
		template<typename A = math_abi::deduce_t<T, 3, Abi>>
		[[nodiscard]] static basic_quat look_at_rh(const basic_vec<T, 3, A> &dir, const basic_vec<T, 3, A> &up = basic_vec<T, 3, A>::up()) noexcept
		{
			return look_at_lh(-dir, up);
		}
		/** Creates left-handed quaternion rotation used to rotate an object in the look direction \a dir using the up direction \a up.
		 * @param dir Direction vector to rotate the origin towards.
		 * @param up Normalized up vector. */
		template<typename A = math_abi::deduce_t<T, 3, Abi>>
		[[nodiscard]] static basic_quat look_at_lh(const basic_vec<T, 3, A> &dir, const basic_vec<T, 3, A> &up = basic_vec<T, 3, A>::up()) noexcept
		{
			basic_mat<T, 3, 3, A> rot;
			const auto right = cross(up, dir);
			rot[0] = right * detail::rsqrt(std::max(T{1e-5}, dot(right, right)));
			rot[1] = cross((rot[2] = dir), rot[0]);
			return from_matrix(rot);
		}

	public:
		constexpr basic_quat() noexcept = default;

		/** Initializes the quaternion from 4 components. */
		template<typename U0, typename U1, typename U2, typename U3>
		basic_quat(U0 x, U1 y, U2 z, U3 w) noexcept : m_vector(x, y, z, w) {}
		/** Initializes the quaternion from a 3D vector of imaginary parts and a scalar real part. */
		template<typename U0, typename U1, typename A>
		basic_quat(const basic_vec<U0, 3, A> &i, U1 r) noexcept : m_vector(i, r) {}
		/** Initializes the quaternion from a 4D vector. */
		template<typename U, typename A>
		constexpr basic_quat(const basic_vec<U, 4, A> &x) noexcept : m_vector(x) {}

		/** Initializes the quaternion as rotation between two normalized vectors. */
		template<typename A>
		basic_quat(const basic_vec<T, 3, A> &u, const basic_vec<T, 3, A> &v) noexcept;
		/** Initializes the quaternion from a 3x3 rotation or 4x4 transform matrix. */
		template<std::size_t N, typename A>
		basic_quat(const basic_mat<T, N, N, A> &x) noexcept requires (N == 3 || N == 4) : m_vector(from_matrix(x)) {}

		/** Returns reference to the `i`th element of the quaternion.
		 * @param i Index of the requested element.
		 * @throw std::range_error In case \a i exceeds `4`. */
		[[nodiscard]] auto &at(std::size_t i) { return m_vector.at(i); }
		/** Returns copy of the `i`th element of the quaternion.
		 * @param i Index of the requested element.
		 * @throw std::range_error In case \a i exceeds `4`. */
		[[nodiscard]] value_type at(std::size_t i) const { return m_vector.at(i); }

		/** Returns reference to the `i`th element of the quaternion.
		 * @param i Index of the requested element. */
		[[nodiscard]] auto &operator[](std::size_t i) noexcept { return m_vector.operator[](i); }
		/** Returns copy of the `i`th element of the quaternion.
		 * @param i Index of the requested element. */
		[[nodiscard]] value_type operator[](std::size_t i) const noexcept { return m_vector.operator[](i); }

		/** Returns reference to the first (i) component of the quaternion. */
		[[nodiscard]] auto &x() noexcept { return m_vector.x(); }
		/** Returns copy of the first (i) component of the quaternion. */
		[[nodiscard]] value_type x() const noexcept { return m_vector.x(); }

		/** Returns reference to the second (j) component of the quaternion. */
		[[nodiscard]] auto &y() noexcept { return m_vector.y(); }
		/** Returns copy of the second (j) component of the quaternion. */
		[[nodiscard]] value_type y() const noexcept { return m_vector.y(); }

		/** Returns reference to the third (k) component of the quaternion. */
		[[nodiscard]] auto &z() noexcept { return m_vector.z(); }
		/** Returns copy of the third (k) component of the quaternion. */
		[[nodiscard]] value_type z() const noexcept { return m_vector.z(); }

		/** Returns reference to the fourth (real) component of the quaternion. */
		[[nodiscard]] auto &w() noexcept { return m_vector.w(); }
		/** Returns copy of the fourth (real) component of the quaternion. */
		[[nodiscard]] value_type w() const noexcept { return m_vector.w(); }

		/** Returns a 4D vector of components of the quaternion. */
		[[nodiscard]] constexpr const vector_type &vector() const noexcept { return m_vector; }
		/** Converts quaternion to a 4D vector. */
		template<typename U, typename A>
		[[nodiscard]] constexpr explicit operator basic_vec<U, 4, A>() const noexcept { return {vector()}; }

		/** Returns the X euler rotation (pitch) of the quaternion. */
		[[nodiscard]] value_type pitch() const noexcept
		{
			const auto a = detail::fmadd(y(), z(), w() * x()) * T{2};
			auto b = detail::fmadd(y(), y(), z() * z());
			b = detail::fmadd(x(), x(), b);
			b = detail::fmsub(w(), w(), b);

			/* Handle singularity near 0. */
			if (fcmp_eq(vec2<T, Abi>{b, a}, {0})) [[unlikely]]
				return static_cast<T>(std::atan2(x(), w()) * T{2});
			return static_cast<T>(std::atan2(a, b));
		}
		/** Returns the Y euler rotation (yaw) of the quaternion. */
		[[nodiscard]] value_type yaw() const noexcept
		{
			const auto a = detail::fmsub(x(), z(), w() * y());
			return static_cast<T>(std::asin(std::clamp(a * T{-2}, T{-1}, T{1})));
		}
		/** Returns the Z euler rotation (roll) of the quaternion. */
		[[nodiscard]] value_type roll() const noexcept
		{
			const auto a = detail::fmadd(x(), y(), w() * z());
			const auto b = detail::fmadd(w(), w(), x() * x());
			const auto c = detail::fmsub(y(), y(), z() * z());
			return static_cast<T>(std::atan2(a * T{2}, b - c));
		}
		/** Returns a vector of euler angles (pitch, yaw, roll) of the quaternion. */
		[[nodiscard]] vec3<T, Abi> to_euler() const noexcept { return {pitch(), yaw(), roll()}; }

		/** Returns the angle of the quaternion rotation. */
		[[nodiscard]] value_type angle() const noexcept
		{
			if (constexpr auto cos1o2 = T{0.877582561890372716130286068203503191}; std::abs(w()) > cos1o2)
				return std::asin(detail::sqrt(detail::fmadd(x(), x(), detail::fmadd(y(), y(), z() * z()))) * T{2});
			else
				return std::acos(w()) * T{2};
		}
		/** Returns the axis vector of the quaternion rotation. */
		[[nodiscard]] vec3<T, Abi> axis() const noexcept
		{
			if (const auto a = T{1} - w() * w(); a > T{0})
				return m_vector.xyz() * detail::rsqrt(a);
			else
				return {0, 0, 1};
		}

	private:
		vector_type m_vector;
	};

	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	template<typename A>
	basic_mat<T, NCols, NRows, Abi>::basic_mat(const basic_quat<T, A> &x) noexcept requires (NCols == NRows && (NCols == 3 || NCols == 4))
	{
		if constexpr (NCols == 4) m_data[3] = col_type{0, 0, 0, 1};

		const auto yxx = x.vector().yxx();
		const auto zzy = x.vector().zzy();
		const auto www = x.vector().www();
		const auto xyz = x.vector().xyz();
		const auto a = yxx * zzy;
		const auto b = www * xyz;
		const auto c = yxx * yxx;
		const auto d = zzy * zzy;

		const auto v0 = (a + b) * T{2};
		m_data[1][2] = v0[0];
		m_data[2][0] = v0[1];
		m_data[0][1] = v0[2];

		const auto v1 = (a - b) * T{2};
		m_data[2][1] = v1[0];
		m_data[0][2] = v1[1];
		m_data[1][0] = v1[2];

		const auto v2 = fmadd(c + d, {-2}, {1});
		m_data[0][0] = v2[0];
		m_data[1][1] = v2[1];
		m_data[2][2] = v2[2];
	}

	/** Gets coordinates of the `I`th corner of the quaternion.
	 * @note `I` must either be less than `4`. */
	template<std::size_t I, typename T, typename Abi>
	[[nodiscard]] constexpr typename basic_quat<T, Abi>::vector_type &get(basic_quat<T, Abi> &x) noexcept requires (I < 4) { return x[I]; }
	/** @copydoc get */
	template<std::size_t I, typename T, typename Abi>
	[[nodiscard]] constexpr const typename basic_quat<T, Abi>::vector_type &get(const basic_quat<T, Abi> &x) noexcept requires (I < 4) { return x[I]; }

#pragma region "basic_quat aliases"
	/** Alias for quaternion that uses implementation-defined ABI deduced from it's type and optional ABI hint. */
	template<typename T, typename Abi = math_abi::fixed_size<4>>
	using quat = basic_quat<T, math_abi::deduce_t<T, 4, Abi>>;
	/** Alias for quaternion that uses implementation-defined compatible ABI. */
	template<typename T>
	using compat_quat = basic_quat<T, math_abi::deduce_t<T, 4, math_abi::compatible<T>>>;
	/** Alias for quaternion that uses packed (non-vectorized) ABI. */
	template<typename T>
	using packed_quat = basic_quat<T, math_abi::packed_buffer<4>>;
#pragma endregion

#pragma region "geometric functions"
	/** Calculates the dot product of quaternions \a a and \a b. */
	template<typename T, typename Abi>
	[[nodiscard]] inline T dot(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return dot(a.vector(), b.vector()); }
	/** Calculates the cross product of quaternions \a a and \a b. */
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> cross(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept
	{
		const auto a333 = a.vector().www();
		const auto b333 = b.vector().www();
		const auto a012 = a.vector().xyz();
		const auto b012 = b.vector().xyz();
		const auto a120 = a.vector().yzx();
		const auto b120 = b.vector().yzx();
		const auto a201 = a.vector().zxy();
		const auto b201 = b.vector().zxy();

		const auto xyz = fmadd(a333, b012, fmadd(a012, b333, fmsub(a120, b201, a201 * b120)));
		const auto w = detail::fmsub(a[3], b[3], detail::fmsub(a[0], b[0], detail::fmsub(a[1], b[1], a[2] * b[2])));
		return {xyz, w};
	}

	/** Returns the normalized copy of quaternion \a x. */
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> normalize(const basic_quat<T, Abi> &x) noexcept
	{
		const auto dp = dot(x, x);
		if (dp <= std::numeric_limits<T>::epsilon()) [[unlikely]]
			return {T{0}, T{0}, T{0}, T{1}};
		return x.vector() * detail::rsqrt(dp);
	}
	/** Calculates the conjugate of quaternion \a x. */
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> conjugate(const basic_quat<T, Abi> &x) noexcept { return {-x.vector().xyz(), x.w()}; }
	/** Calculates the inverse of quaternion \a x. */
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> inverse(const basic_quat<T, Abi> &x) noexcept { return conjugate(x).vector() / dot(x, x); }
#pragma endregion

#pragma region "translation functions"
	/** Calculates the linear interpolation between quaternions \a a and \a b using factor \a f.
	 * @note \a f must be in range `[0, 1.0]`. */
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> lerp(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b, T f) noexcept
	{
		SEK_ASSERT(0 <= f && f <= 1.0);
		return {a.vector() * fmsub(b.vector(), {f}, {f - T{1}})};
	}
	/** Calculates the spherical linear interpolation between quaternions \a a and \a b using factor \a f. */
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> slerp(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b, T f) noexcept
	{
		auto va = a.vector();
		auto vb = b.vector();
		auto t = dot(a, b);
		if (t < T{0})
		{
			vb = -vb;
			t = -t;
		}

		/* Use linear interpolation when t is close to 1 */
		if (t > T{1} - std::numeric_limits<T>::epsilon())
			return {lerp(va, vb, f)};
		else
		{
			const auto x = std::acos(t);
			return {fmadd(va, {std::sin(detail::fnmadd(f, x, x))}, vb * std::sin(f * x)) / std::sin(x)};
		}
	}
	/** Calculates the spherical linear interpolation between quaternions \a a and \a b using factor \a f and spin count \a c. */
	template<typename T, typename U, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> slerp(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b, T f, U k) noexcept
	{
		auto va = a.vector();
		auto vb = b.vector();
		auto t = dot(a, b);
		if (t < T{0})
		{
			vb = -vb;
			t = -t;
		}

		/* Use linear interpolation when t is close to 1 */
		if (t > T{1} - std::numeric_limits<T>::epsilon())
			return {lerp(va, vb, f)};
		else
		{
			const auto x = std::acos(t);
			const auto p = x + std::numbers::pi_v<T> * static_cast<T>(k);
			return {fmadd(va, {std::sin(detail::fnmadd(f, p, x))}, vb * std::sin(f * p)) / std::sin(x)};
		}
	}

	/** Rotates a quaternion \a q about normalized axis \a v using angle \a a.
	 * @param q Quaternion to rotate.
	 * @param a Rotation angle.
	 * @param v Rotation axis vector.
	 * @return Quaternion rotated about the axis.
	 * @note Rotation axis must be normalized. */
	template<typename T, typename AQ, typename AV = math_abi::deduce_t<T, 3, AQ>>
	[[nodiscard]] inline basic_quat<T, AQ> rotate(const basic_quat<T, AQ> &q, T a, const basic_vec<T, 3, AV> &v) noexcept
	{
		return q * basic_quat<T, AQ>::angle_axis(a, v);
	}
#pragma endregion

	template<std::floating_point T, typename Abi>
	template<typename A>
	basic_quat<T, Abi>::basic_quat(const basic_vec<T, 3, A> &u, const basic_vec<T, 3, A> &v) noexcept
	{
		basic_vec<T, 4, Abi> t;

		const auto a = detail::sqrt(dot(u, u) * dot(v, v));
		if (const auto b = a + dot(u, v); b >= T{1.e-6} * a)
			t = {cross(u, v), b};
		else if (std::abs(u[0]) > std::abs(u[2]))
			t = {-u[1], u[0], 0, 0};
		else
			t = {0, -u[2], u[1], 0};

		m_vector = normalize(basic_quat{t}).vector();
	}

#pragma region "basic_quat operators"
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> operator+(const basic_quat<T, Abi> &x) noexcept { return +x.vector(); }
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> operator-(const basic_quat<T, Abi> &x) noexcept { return -x.vector(); }

	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> operator+(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return {a.vector() + b.vector()}; }
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> operator-(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return {a.vector() - b.vector()}; }
	template<typename T, typename Abi>
	inline basic_quat<T, Abi> operator+=(basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return a = (a + b); }
	template<typename T, typename Abi>
	inline basic_quat<T, Abi> operator-=(basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return a = (a - b); }

	template<typename T, typename Abi>
	[[nodiscard]] inline basic_quat<T, Abi> operator*(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept
	{
		const auto a3 = a.vector().xyz();
		const auto b3 = b.vector().xyz();
		const auto cp = cross(a3, b3);
		const auto dp = dot(a3, b3);
		return {fmadd(b3, {a.w()}, fmadd(a3, {b.w()}, cp)), detail::fmsub(a.w(), b.w(), dp)};
	}
	template<typename T, typename Abi>
	inline basic_quat<T, Abi> &operator*=(basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return (a = a * b); }

	template<typename T, typename AQ, typename AV = math_abi::deduce_t<T, 3, AQ>>
	[[nodiscard]] inline basic_vec<T, 3, AV> operator*(const basic_quat<T, AQ> &a, const basic_vec<T, 3, AV> &b) noexcept
	{
		const auto a3 = a.vector().xyz();
		const auto u0 = cross(a3, b);
		const auto u1 = cross(a3, u0);
		return fmadd(fmadd(u0, {a.w()}, u1), basic_vec<T, 3, AV>{2}, b);
	}
	template<typename T, typename AV, typename AQ = math_abi::deduce_t<T, 4, AV>>
	[[nodiscard]] inline basic_vec<T, 3, AV> operator*(const basic_vec<T, 3, AV> &a, const basic_quat<T, AQ> &b) noexcept { return inverse(b) * a; }

	template<typename T, typename Abi>
	[[nodiscard]] inline basic_vec<T, 4, Abi> operator*(const basic_quat<T, Abi> &a, const basic_vec<T, 4, Abi> &b) noexcept { return {a * b.xyz(), b.w()}; }
	template<typename T, typename Abi>
	[[nodiscard]] inline basic_vec<T, 4, Abi> operator*(const basic_vec<T, 4, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return inverse(b) * a; }

	template<typename T, typename Abi>
	[[nodiscard]] inline typename basic_quat<T, Abi>::mask_type operator==(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return a.vector() == b.vector(); }
	template<typename T, typename Abi>
	[[nodiscard]] inline typename basic_quat<T, Abi>::mask_type operator!=(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return a.vector() != b.vector(); }
#pragma endregion

#pragma region "comparison functions"
	/** Determines if elements of \a x are infinite. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline typename basic_quat<T, A>::mask_type isinf(const basic_quat<T, A> &x) noexcept { return isinf(x.vector()); }
	/** Determines if elements of \a x are unordered NaN. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline typename basic_quat<T, A>::mask_type isnan(const basic_quat<T, A> &x) noexcept { return isnan(x.vector()); }

	/** Determines if elements of quaternion \a a are within relative epsilon \a e_rel or absolute epsilon \a e_abs of quaternion \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline typename basic_quat<T, A>::mask_type fcmp_eq(const basic_quat<T, A> &a, const basic_quat<T, A> &b, T e_rel, T e_abs) noexcept { return fcmp_eq(a.vector(), b.vector(), e_abs, e_rel); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, typename A, typename Promoted = detail::promote_t<T0, T1, T2, T3>>
	[[nodiscard]] inline vec4_mask<Promoted, A> fcmp_eq(const basic_quat<T0, A> &a, const basic_quat<T1, A> &b, T2 e_rel, T3 e_abs) noexcept { return fcmp_eq(a.vector(), b.vector(), e_abs, e_rel); }

	/** Determines if elements of quaternion \a a are within epsilon \a e of quaternion \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline typename basic_quat<T, A>::mask_type fcmp_eq(const basic_quat<T, A> &a, const basic_quat<T, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_eq(a, b, e, e); }
	/** @copydoc fcmp_eq
 	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename A, typename Promoted = detail::promote_t<T0, T1, T2>>
	[[nodiscard]] inline vec4_mask<Promoted, A> fcmp_eq(const basic_quat<T0, A> &a, const basic_quat<T1, A> &b, T2 e) noexcept { return fcmp_eq(a, b, e, e); }

	/** Determines if elements of quaternion \a a are not within relative epsilon \a e_rel or absolute epsilon \a e_abs of quaternion \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline typename basic_quat<T, A>::mask_type fcmp_ne(const basic_quat<T, A> &a, const basic_quat<T, A> &b, T e_rel, T e_abs) noexcept { return fcmp_ne(a.vector(), b.vector(), e_abs, e_rel); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, typename A, typename Promoted = detail::promote_t<T0, T1, T2, T3>>
	[[nodiscard]] inline vec4_mask<Promoted, A> fcmp_ne(const basic_quat<T0, A> &a, const basic_quat<T1, A> &b, T2 e_rel, T3 e_abs) noexcept { return fcmp_ne(a.vector(), b.vector(), e_abs, e_rel); }

	/** Determines if elements of quaternion \a a are not within epsilon \a e of quaternion \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline typename basic_quat<T, A>::mask_type fcmp_ne(const basic_quat<T, A> &a, const basic_quat<T, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_ne(a, b, e, e); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename A, typename Promoted = detail::promote_t<T0, T1, T2>>
	[[nodiscard]] inline vec4_mask<Promoted, A> fcmp_ne(const basic_quat<T0, A> &a, const basic_quat<T1, A> &b, T2 e) noexcept { return fcmp_ne(a, b, e, e); }
#pragma endregion
}
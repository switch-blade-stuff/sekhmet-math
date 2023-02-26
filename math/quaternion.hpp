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
	template<typename T, typename Abi>
	class basic_quat
	{
	public:
		using vector_type = basic_vec<T, 4, Abi>;
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
		constexpr basic_quat() noexcept = default;

		/** Initializes the quaternion from 4 components. */
		basic_quat(T x, T y, T z, T w) noexcept : m_data(x, y, z, w) {}
		/** Initializes the quaternion from a 4D vector. */
		template<typename U, typename A>
		constexpr basic_quat(const basic_vec<U, 4, A> &x) noexcept : m_data(x) {}

		/** Initializes the quaternion from a 3x3 rotation or 4x4 transform matrix. */
		template<std::size_t N, typename A>
		basic_quat(const basic_mat<T, N, N, A> &x) noexcept requires (N == 3 || N == 4) : m_data(from_matrix(x)) {}

		/** Returns reference to the `i`th element of the quaternion.
		 * @param i Index of the requested element. */
		[[nodiscard]] auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		/** Returns copy of the `i`th element of the quaternion.
		 * @param i Index of the requested element. */
		[[nodiscard]] value_type operator[](std::size_t i) const noexcept { return m_data[i]; }

		/** Returns reference to the first (i) component of the quaternion. */
		[[nodiscard]] auto &x() noexcept { return m_data.x(); }
		/** Returns copy of the first (i) component of the quaternion. */
		[[nodiscard]] value_type x() const noexcept { return m_data.x(); }

		/** Returns reference to the second (j) component of the quaternion. */
		[[nodiscard]] auto &y() noexcept { return m_data.y(); }
		/** Returns copy of the second (j) component of the quaternion. */
		[[nodiscard]] value_type y() const noexcept { return m_data.y(); }

		/** Returns reference to the third (k) component of the quaternion. */
		[[nodiscard]] auto &z() noexcept { return m_data.z(); }
		/** Returns copy of the third (k) component of the quaternion. */
		[[nodiscard]] value_type z() const noexcept { return m_data.z(); }

		/** Returns reference to the fourth (real) component of the quaternion. */
		[[nodiscard]] auto &w() noexcept { return m_data.w(); }
		/** Returns copy of the fourth (real) component of the quaternion. */
		[[nodiscard]] value_type w() const noexcept { return m_data.w(); }

		/** Returns a 4D vector of components of the quaternion. */
		[[nodiscard]] constexpr vector_type vector() const noexcept { return m_data; }
		/** Converts quaternion to a 4D vector. */
		[[nodiscard]] constexpr explicit operator vector_type() const noexcept { return vector(); }

	private:
		vector_type m_data = {};
	};

	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	template<typename A>
	basic_mat<T, NCols, NRows, Abi>::basic_mat(const basic_quat<T, A> &x) noexcept requires (NCols == NRows && (NCols == 3 || NCols == 4))
	{
		if constexpr (NCols == 4) m_data[3] = col_type{0, 0, 0, 1};

		const auto xx = x.x() * x.x();
		const auto yy = x.y() * x.y();
		const auto zz = x.z() * x.z();
		const auto wx = x.w() * x.x();
		const auto xy = x.x() * x.y();
		const auto yz = x.y() * x.z();
		const auto xz = x.x() * x.z();
		const auto wy = x.w() * x.y();
		const auto wz = x.w() * x.z();

		const auto a = vec3<T, A>{yz, xz, xy};
		const auto b = vec3<T, A>{wx, wy, wz};
		const auto c = vec3<T, A>{yy, xx, xx};
		const auto d = vec3<T, A>{zz, zz, yy};

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

	template<typename T, typename Abi>
	[[nodiscard]] inline bool operator==(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return a.vector() == b.vector(); }
	template<typename T, typename Abi>
	[[nodiscard]] inline bool operator!=(const basic_quat<T, Abi> &a, const basic_quat<T, Abi> &b) noexcept { return a.vector() != b.vector(); }

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
	/** Alias for quaternion that uses compatible (implementation-defined) ABI. */
	template<typename T>
	using compat_quat = basic_quat<T, math_abi::deduce_t<T, 4, math_abi::compatible<T>>>;
	/** Alias for quaternion that uses packed (non-vectorized) ABI. */
	template<typename T>
	using packed_quat = basic_quat<T, math_abi::packed_buffer<4>>;
#pragma endregion

#pragma region "comparison functions"
	/** Determines if elements of quaternion \a a are within relative epsilon \a e_rel or absolute epsilon \a e_abs of quaternion \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_quat<T, A> &a, const basic_quat<T, A> &b, T e_rel, T e_abs) noexcept { return fcmp_eq(a.vector(), b.vector(), e_abs, e_rel); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_quat<T0, A> &a, const basic_quat<T1, A> &b, T2 e_rel, T3 e_abs) noexcept { return fcmp_eq(a.vector(), b.vector(), e_abs, e_rel); }

	/** Determines if elements of quaternion \a a are within epsilon \a e of quaternion \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_quat<T, A> &a, const basic_quat<T, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_eq(a, b, e, e); }
	/** @copydoc fcmp_eq
 	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_quat<T0, A> &a, const basic_quat<T1, A> &b, T2 e) noexcept { return fcmp_eq(a, b, e, e); }

	/** Determines if elements of quaternion \a a are not within relative epsilon \a e_rel or absolute epsilon \a e_abs of quaternion \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_quat<T, A> &a, const basic_quat<T, A> &b, T e_rel, T e_abs) noexcept { return fcmp_ne(a.vector(), b.vector(), e_abs, e_rel); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_quat<T0, A> &a, const basic_quat<T1, A> &b, T2 e_rel, T3 e_abs) noexcept { return fcmp_ne(a.vector(), b.vector(), e_abs, e_rel); }

	/** Determines if elements of quaternion \a a are not within epsilon \a e of quaternion \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_quat<T, A> &a, const basic_quat<T, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_ne(a, b, e, e); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_quat<T0, A> &a, const basic_quat<T1, A> &b, T2 e) noexcept { return fcmp_ne(a, b, e, e); }
#pragma endregion
}
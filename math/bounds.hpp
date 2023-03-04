/*
 * Created by switchblade on 2023-03-03.
 */

#pragma once

#include "vector.hpp"
#include "matrix.hpp"

namespace sek
{
	template<std::size_t I, typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_bounds<T, N, Abi>::vector_type &get(basic_bounds<T, N, Abi> &x) noexcept requires (I < 2);
	template<std::size_t I, typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_bounds<T, N, Abi>::vector_type &get(const basic_bounds<T, N, Abi> &x) noexcept requires (I < 2);

	/** @brief Structure used to define an N-dimensional bound volume such as 3D axis-aligned bounding box or 2D rect.
	 * @tparam T Value type of the underlying vectors.
	 * @tparam N Dimension of the bounding volume.
	 * @tparam Abi ABI tag used by the underlying vectors.
	 * @note The \a Abi tag size must be `3`. */
	template<typename T, std::size_t N, typename Abi>
	class basic_bounds
	{
		template<std::size_t I, typename U, std::size_t M, typename A>
		friend constexpr typename basic_bounds<U, M, A>::vector_type &get(basic_bounds<U, M, A> &) noexcept requires (I < 2);
		template<std::size_t I, typename U, std::size_t M, typename A>
		friend constexpr const typename basic_bounds<U, M, A>::vector_type &get(const basic_bounds<U, M, A> &) noexcept requires (I < 2);

	public:
		using vector_type = basic_vec<T, N, Abi>;
		using value_type = typename vector_type::value_type;

	public:
		constexpr basic_bounds() noexcept = default;

		/** Initializes the bounding volume from minimum (top left) & maximum (bottom right) coordinates. */
		constexpr basic_bounds(vector_type min, vector_type max) noexcept : m_min(min), m_max(max) {}

		/** Returns the coordinates of the minimum corner point of the bounding volume. */
		[[nodiscard]] constexpr vector_type min() const noexcept { return m_min; }
		/** Returns the coordinates of the maximum corner point of the bounding volume. */
		[[nodiscard]] constexpr vector_type max() const noexcept { return m_max; }
		/** Sets the coordinates of the minimum corner point of the bounding volume to the point specified by the vector \a value.
		 * @note This function only modifies coordinates of the minimum corner point of the bounding volume,
		 * coordinates of the maximum corner are left intact. */
		constexpr void min(vector_type value) const noexcept { m_min = value; }
		/** Sets the coordinates of the maximum corner point of the bounding volume to the point specified by the vector \a value.
		 * @note This function only modifies coordinates of the maximum corner point of the bounding volume,
		 * coordinates of the minimum corner are left intact. */
		constexpr void max(vector_type value) const noexcept { m_max = value; }

		/** Returns the center coordinates of the bounding volume. */
		[[nodiscard]] vector_type center() const noexcept { return (max() + min()) / static_cast<T>(2); }
		/** Returns the size of the bounding volume. */
		[[nodiscard]] vector_type size() const noexcept { return max() - min(); }

		/** Sets the center coordinates of the bounding volume to the point specified by the vector \a value. */
		void center(vector_type value) noexcept
		{
			const auto so2 = size() / static_cast<T>(2);
			m_min = value - so2;
			m_max = value + so2;
		}
		/** Sets the size of the bounding volume to size specified by the vector \a value. */
		void size(vector_type value) noexcept { m_max = m_min + value; }

	private:
		vector_type m_min = {};
		vector_type m_max = {};
	};

	/** Gets coordinates of the `I`th point of the bounding volume.
	 * @note `I` must either be `0` or `1`. */
	template<std::size_t I, typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_bounds<T, N, Abi>::vector_type &get(basic_bounds<T, N, Abi> &x) noexcept requires (I < 2) { return x.m_data[I]; }
	/** @copydoc get */
	template<std::size_t I, typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_bounds<T, N, Abi>::vector_type &get(const basic_bounds<T, N, Abi> &x) noexcept requires (I < 2) { return x.m_data[I]; }

#pragma region "basic_bounds aliases"
	/** Alias for N-dimensional bounding volume that uses implementation-defined ABI deduced from it's type, size and optional ABI hint. */
	template<typename T, std::size_t N, typename Abi = math_abi::fixed_size<N>>
	using bounds = basic_bounds<T, N, math_abi::deduce_t<T, N, Abi>>;
	/** Alias for N-dimensional bounding volume that uses implementation-defined compatible ABI. */
	template<typename T, std::size_t N>
	using compat_bounds = basic_bounds<T, N, math_abi::deduce_t<T, N, math_abi::compatible<T>>>;
	/** Alias for N-dimensional bounding volume that uses packed (non-vectorized) ABI. */
	template<typename T, std::size_t N>
	using packed_bounds = basic_bounds<T, N, math_abi::packed_buffer<N>>;

	/** Alias for 2D rectangle that uses implementation-defined ABI deduced from it's type and optional ABI hint. */
	template<typename T, typename Abi = math_abi::fixed_size<2>>
	using rect = bounds<T, 2, Abi>;
	/** Alias for 2D rectangle that uses implementation-defined compatible ABI. */
	template<typename T>
	using compat_rect = compat_bounds<T, 2>;
	/** Alias for 2D rectangle that uses packed (non-vectorized) ABI. */
	template<typename T>
	using packed_rect = packed_bounds<T, 2>;

	/** Alias for 3D axis-aligned bounding box that uses implementation-defined ABI deduced from it's type and optional ABI hint. */
	template<typename T, typename Abi = math_abi::fixed_size<3>>
	using bbox = bounds<T, 3, Abi>;
	/** Alias for 3D axis-aligned bounding box that uses implementation-defined compatible ABI. */
	template<typename T>
	using compat_bbox = compat_bounds<T, 3>;
	/** Alias for 3D axis-aligned bounding box that uses packed (non-vectorized) ABI. */
	template<typename T>
	using packed_bbox = packed_bounds<T, 3>;
#pragma endregion

#pragma region "projection functions"
	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	template<typename A>
	basic_mat<T, NCols, NRows, Abi> basic_mat<T, NCols, NRows, Abi>::rect_projection(const basic_bounds<T, 2, A> &sr, const basic_bounds<T, 2, A> &vp) noexcept requires (NCols == NRows && NCols == 4)
	{
		const auto sr_size = sr.size();
		SEK_ASSERT((sr_size > vec2<T, A>{0}));

		const auto tmp = (fmadd({2}, vp.min(), vp.max()) - sr.max() - sr.min()) / sr_size;
		return scale(translate(mat4x4<T, A>::identity(), {tmp, 0}), {vp.max() / sr_size, 1});
	}

	/** Maps object coordinate vector \a obj from world-space coordinates to screen-space coordinates of a viewport specified by rectangle \a vp using model view and projection matrices \a m & \a p.
	 * @param pos World-space coordinates of an object to be projected.
	 * @param m Model view matrix.
	 * @param p Projection matrix.
	 * @param vp Viewport rectangle.
	 * @return Coordinates of the object in viewport space. */
	template<typename T, typename AP, typename AM = math_abi::deduce_t<T, 4, AP>, typename AR = math_abi::deduce_t<T, 2, AM>>
	[[nodiscard]] inline basic_vec<T, 3, AP> project(const basic_vec<T, 3, AP> &pos, const basic_mat<T, 4, 4, AM> &m, const basic_mat<T, 4, 4, AM> &p, const basic_bounds<T, 2, AR> &vp) noexcept
	{
		auto a = p * m * basic_vec<T, 4, AM>{pos, 1};
		a = fmadd((a / a.w()), {0.5}, {0.5});
		return {fmadd(a.xy(), vp.max(), vp.min()), a.z()};
	}
	/** Maps object coordinate vector \a obj from screen-space coordinates of a viewport specified by rectangle \a vp to world-space coordinates using model view and projection matrices \a m & \a p.
	 * @param pos Screen-space coordinates of an object to be un-projected.
	 * @param m Model view matrix.
	 * @param p Projection matrix.
	 * @param vp Viewport rectangle.
	 * @return Coordinates of the object in world space. */
	template<typename T, typename AP, typename AM = math_abi::deduce_t<T, 4, AP>, typename AR = math_abi::deduce_t<T, 2, AM>>
	[[nodiscard]] inline basic_vec<T, 3, AP> unproject(const basic_vec<T, 3, AP> &pos, const basic_mat<T, 4, 4, AM> &m, const basic_mat<T, 4, 4, AM> &p, const basic_bounds<T, 2, AR> &vp) noexcept
	{
		const auto a = (pos.xy() - vp.min()) / vp.max();
		const auto b = basic_vec<T, 4, AM>{a, pos.z(), 1} * T{2} - T{1};
		const auto c = inverse(p * m) * b;
		return c.xyz() / c.w();
	}
#pragma endregion

#pragma region "basic_bounds operators"
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_bounds<T, N, Abi> operator+(const basic_bounds<T, N, Abi> &x) noexcept { return {+x.min(), +x.max()}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_bounds<T, N, Abi> operator-(const basic_bounds<T, N, Abi> &x) noexcept { return {-x.min(), -x.max()}; }

	template<typename T, typename AM, typename AB = math_abi::deduce_t<T, 3, AM>>
	[[nodiscard]] inline basic_bounds<T, 3, AB> operator*(const basic_mat<T, 4, 4, AM> &a, const basic_bounds<T, 3, AB> &b) noexcept
	{
		typename basic_bounds<T, 3, AB>::vector_type min, max;
		min = max = a[3].xyz();

		for (std::size_t i = 0; i < 3; ++i)
		{
			const auto va = a[i].xyz() * b.min();
			const auto vb = a[i].xyz() * b.max();
			min += sek::min(va, va);
			max += sek::max(vb, vb);
		}
		return {min, max};
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline bool operator==(const basic_bounds<T, N, Abi> &a, const basic_bounds<T, N, Abi> &b) noexcept { return a.min() == a.max() & b.min() == b.max(); }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline bool operator!=(const basic_bounds<T, N, Abi> &a, const basic_bounds<T, N, Abi> &b) noexcept { return a.min() != a.max() & b.min() != b.max(); }
#pragma endregion

#pragma region "comparison functions"
	/** Determines if elements of bounding volume \a a are within relative epsilon \a e_rel or absolute epsilon \a e_abs of bounding volume \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_bounds<T, N, A> &a, const basic_bounds<T, N, A> &b, T e_rel, T e_abs) noexcept
	{
		return fcmp_eq(a.min(), b.min(), e_abs, e_rel) & fcmp_eq(a.max(), b.max(), e_abs, e_rel);
	}
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, std::size_t N, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_bounds<T0, N, A> &a, const basic_bounds<T1, N, A> &b, T2 e_rel, T3 e_abs) noexcept
	{
		return fcmp_eq(a.min(), b.min(), e_abs, e_rel) & fcmp_eq(a.max(), b.max(), e_abs, e_rel);
	}

	/** Determines if elements of bounding volume \a a are within epsilon \a e of bounding volume \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_bounds<T, N, A> &a, const basic_bounds<T, N, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_eq(a, b, e, e); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_bounds<T0, N, A> &a, const basic_bounds<T1, N, A> &b, T2 e) noexcept { return fcmp_eq(a, b, e, e); }

	/** Determines if elements of bounding volume \a a are not within relative epsilon \a e_rel or absolute epsilon \a e_abs of bounding volume \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_bounds<T, N, A> &a, const basic_bounds<T, N, A> &b, T e_rel, T e_abs) noexcept
	{
		return fcmp_ne(a.min(), b.min(), e_abs, e_rel) & fcmp_ne(a.max(), b.max(), e_abs, e_rel);
	}
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, std::size_t N, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_bounds<T0, N, A> &a, const basic_bounds<T1, N, A> &b, T2 e_rel, T3 e_abs) noexcept
	{
		return fcmp_ne(a.min(), b.min(), e_abs, e_rel) & fcmp_ne(a.max(), b.max(), e_abs, e_rel);
	}

	/** Determines if elements of bounding volume \a a are not within epsilon \a e of bounding volume \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_bounds<T, N, A> &a, const basic_bounds<T, N, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_ne(a, b, e, e); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_bounds<T0, N, A> &a, const basic_bounds<T1, N, A> &b, T2 e) noexcept { return fcmp_ne(a, b, e, e); }
#pragma endregion
}
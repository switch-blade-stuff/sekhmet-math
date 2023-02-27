/*
 * Created by switchblade on 2023-02-25.
 */

#pragma once

#include "vector.hpp"
#include "matrix.hpp"

namespace sek
{
	template<std::size_t I, typename T, typename Abi>
	[[nodiscard]] constexpr typename basic_rect<T, Abi>::vector_type &get(basic_rect<T, Abi> &x) noexcept requires (I < 2);
	template<std::size_t I, typename T, typename Abi>
	[[nodiscard]] constexpr const typename basic_rect<T, Abi>::vector_type &get(const basic_rect<T, Abi> &x) noexcept requires (I < 2);

	/** @brief Structure used to define a screen-space rectangle consisting of 2 vectors.
	 * @tparam T Value type of the underlying vectors.
	 * @tparam Abi ABI tag used by the underlying vectors.
	 * @note The \a Abi tag size must be `2`. */
	template<typename T, typename Abi>
	class basic_rect
	{
		template<std::size_t I, typename U, typename A>
		friend constexpr typename basic_rect<U, A>::vector_type &get(basic_rect<U, A> &) noexcept requires (I < 2);
		template<std::size_t I, typename U, typename A>
		friend constexpr const typename basic_rect<U, A>::vector_type &get(const basic_rect<U, A> &) noexcept requires (I < 2);

	public:
		using vector_type = basic_vec<T, 2, Abi>;
		using value_type = typename vector_type::value_type;

	public:
		constexpr basic_rect() noexcept = default;

		/** Initializes a rectangle from minimum (top left) & maximum (bottom right) coordinates. */
		constexpr basic_rect(vector_type min, vector_type max) noexcept : m_data{min, max} {}
		/** Initializes a rectangle origin coordinates, width and height. */
		basic_rect(vector_type origin, value_type width, value_type height) noexcept
		{
			m_data[0] = origin;
			m_data[1] = origin + vector_type{width, height};
		}

		/** Returns the coordinates of the minimum (top left) corner of the rectangle. */
		[[nodiscard]] constexpr vector_type min() const noexcept { return m_data[0]; }
		/** Returns the coordinates of the maximum (bottom right) corner of the rectangle. */
		[[nodiscard]] constexpr vector_type max() const noexcept { return m_data[1]; }
		/** Sets the coordinates of the minimum corner of the rectangle to the point specified by the vector \a value.
		 * @note This function only modifies coordinates of the minimum corner of the rectangle,
		 * coordinates of the maximum corner are left intact. */
		constexpr void min(vector_type value) const noexcept { m_data[0] = value; }
		/** Sets the coordinates of the maximum corner of the rectangle to the point specified by the vector \a value.
		 * @note This function only modifies coordinates of the maximum corner of the rectangle,
		 * coordinates of the minimum corner are left intact. */
		constexpr void max(vector_type value) const noexcept { m_data[1] = value; }

		/** Returns the center coordinates of the rectangle. */
		[[nodiscard]] vector_type center() const noexcept { return (max() + min()) / static_cast<T>(2); }
		/** Returns the origin coordinates of the rectangle. */
		[[nodiscard]] vector_type origin() const noexcept { return min(); }
		/** Returns the size (width, height) of the rectangle. */
		[[nodiscard]] vector_type size() const noexcept { return max() - min(); }

		/** Sets the center coordinates of the rectangle to the point specified by the vector \a value. */
		void center(vector_type value) noexcept
		{
			const auto so2 = size() / 2;
			m_data[0] = value - so2;
			m_data[1] = value + so2;
		}
		/** Sets the origin coordinates of the rectangle to the point specified by the vector \a value. */
		void origin(vector_type value) noexcept
		{
			const auto diff = value - m_data[0];
			m_data[0] = value;
			m_data[1] += diff;
		}
		/** Sets the size (width, height) of the rectangle to width and height specified by the vector \a value. */
		void size(vector_type value) noexcept { m_data[1] = m_data[0] + value; }

	private:
		vector_type m_data[2] = {};
	};

	template<typename T, typename Abi>
	[[nodiscard]] inline bool operator==(const basic_rect<T, Abi> &a, const basic_rect<T, Abi> &b) noexcept
	{
		return a.min() == a.max() & b.min() == b.max();
	}
	template<typename T, typename Abi>
	[[nodiscard]] inline bool operator!=(const basic_rect<T, Abi> &a, const basic_rect<T, Abi> &b) noexcept
	{
		return a.min() != a.max() & b.min() != b.max();
	}

	/** Gets coordinates of the `I`th corner of the rectangle.
	 * @note `I` must either be `0` or `1`. */
	template<std::size_t I, typename T, typename Abi>
	[[nodiscard]] constexpr typename basic_rect<T, Abi>::vector_type &get(basic_rect<T, Abi> &x) noexcept requires (I < 2) { return x.m_data[I]; }
	/** @copydoc get */
	template<std::size_t I, typename T, typename Abi>
	[[nodiscard]] constexpr const typename basic_rect<T, Abi>::vector_type &get(const basic_rect<T, Abi> &x) noexcept requires (I < 2) { return x.m_data[I]; }

#pragma region "basic_rect aliases"
	/** Alias for rectangle that uses implementation-defined ABI deduced from it's type and optional ABI hint. */
	template<typename T, typename Abi = math_abi::fixed_size<2>>
	using rect = basic_rect<T, math_abi::deduce_t<T, 2, Abi>>;
	/** Alias for rectangle that uses implementation-defined compatible ABI. */
	template<typename T>
	using compat_rect = basic_rect<T, math_abi::deduce_t<T, 2, math_abi::compatible<T>>>;
	/** Alias for rectangle that uses packed (non-vectorized) ABI. */
	template<typename T>
	using packed_rect = basic_rect<T, math_abi::packed_buffer<2>>;
#pragma endregion

#pragma region "projection functions"
	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	template<typename A>
	basic_mat<T, NCols, NRows, Abi> basic_mat<T, NCols, NRows, Abi>::rect_projection(const basic_rect<T, A> &sr, const basic_rect<T, A> &vp) noexcept requires (NCols == NRows && NCols == 4)
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
	[[nodiscard]] inline basic_vec<T, 3, AP> project(const basic_vec<T, 3, AP> &pos, const basic_mat<T, 4, 4, AM> &m, const basic_mat<T, 4, 4, AM> &p, const basic_rect<T, AR> &vp) noexcept
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
	[[nodiscard]] inline basic_vec<T, 3, AP> unproject(const basic_vec<T, 3, AP> &pos, const basic_mat<T, 4, 4, AM> &m, const basic_mat<T, 4, 4, AM> &p, const basic_rect<T, AR> &vp) noexcept
	{
		const auto a = (pos.xy() - vp.min()) / vp.max();
		const auto b = basic_vec<T, 4, AM>{a, pos.z(), 1} * T{2} - T{1};
		const auto c = inverse(p * m) * b;
		return c.xyz() / c.w();
	}
#pragma endregion

#pragma region "comparison functions"
	/** Determines if elements of rectangle \a a are within relative epsilon \a e_rel or absolute epsilon \a e_abs of rectangle \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_rect<T, A> &a, const basic_rect<T, A> &b, T e_rel, T e_abs) noexcept
	{
		return fcmp_eq(a.min(), b.min(), e_abs, e_rel) & fcmp_eq(a.max(), b.max(), e_abs, e_rel);
	}
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_rect<T0, A> &a, const basic_rect<T1, A> &b, T2 e_rel, T3 e_abs) noexcept
	{
		return fcmp_eq(a.min(), b.min(), e_abs, e_rel) & fcmp_eq(a.max(), b.max(), e_abs, e_rel);
	}

	/** Determines if elements of rectangle \a a are within epsilon \a e of rectangle \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_rect<T, A> &a, const basic_rect<T, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_eq(a, b, e, e); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_rect<T0, A> &a, const basic_rect<T1, A> &b, T2 e) noexcept { return fcmp_eq(a, b, e, e); }

	/** Determines if elements of rectangle \a a are not within relative epsilon \a e_rel or absolute epsilon \a e_abs of rectangle \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_rect<T, A> &a, const basic_rect<T, A> &b, T e_rel, T e_abs) noexcept
	{
		return fcmp_ne(a.min(), b.min(), e_abs, e_rel) & fcmp_ne(a.max(), b.max(), e_abs, e_rel);
	}
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_rect<T0, A> &a, const basic_rect<T1, A> &b, T2 e_rel, T3 e_abs) noexcept
	{
		return fcmp_ne(a.min(), b.min(), e_abs, e_rel) & fcmp_ne(a.max(), b.max(), e_abs, e_rel);
	}

	/** Determines if elements of rectangle \a a are not within epsilon \a e of rectangle \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_rect<T, A> &a, const basic_rect<T, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_ne(a, b, e, e); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_rect<T0, A> &a, const basic_rect<T1, A> &b, T2 e) noexcept { return fcmp_ne(a, b, e, e); }
#pragma endregion
}
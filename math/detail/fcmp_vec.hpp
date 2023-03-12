/*
 * Created by switchblade on 2023-02-24.
 */

#pragma once

#include "fclass.hpp"
#include "mbase.hpp"

namespace sek
{
	/** Determines if \a a is within relative epsilon \a e_rel or absolute epsilon \a e_abs of \a b.
	 * @note If either \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T>
	[[nodiscard]] constexpr bool fcmp_eq(T a, T b, T e_rel, T e_abs) noexcept
	{
		/* Compare infinities. */
		if (std::isinf(a) && std::isinf(b) && std::signbit(a) == std::signbit(b)) [[unlikely]]
			return true;

		/* Compare using absolute epsilon. */
		const auto diff = std::abs(a - b);
		if (diff <= e_abs) return true;

		/* Compare using relative epsilon. */
		const auto abs_a = std::abs(a);
		const auto abs_b = std::abs(b);
		const auto max_ab = std::max(abs_a, abs_b);
		return diff <= max_ab * e_rel;
	}
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3>
	[[nodiscard]] constexpr bool fcmp_eq(T0 a, T1 b, T2 e_rel, T3 e_abs) noexcept
	{
		using promoted_t = detail::promote_t<T0, T1, T2, T3>;
		return fcmp_eq(promoted_t{a}, promoted_t{b}, promoted_t{e_rel}, promoted_t{e_abs});
	}

	/** Determines if \a a is within epsilon \a e of \a b.
	 * @note If either \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T>
	[[nodiscard]] constexpr bool fcmp_eq(T a, T b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_eq(a, b, e, e); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2>
	[[nodiscard]] constexpr bool fcmp_eq(T0 a, T1 b, T2 e) noexcept { return fcmp_eq(a, b, e, e); }

	/** Determines if \a a is not within relative epsilon \a e_rel or absolute epsilon \a e_abs of \a b.
	 * @note If either \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T>
	[[nodiscard]] constexpr bool fcmp_ne(T a, T b, T e_rel, T e_abs) noexcept
	{
		/* Compare infinities. */
		if (std::isinf(a) && std::isinf(b) && std::signbit(a) == std::signbit(b)) [[unlikely]]
			return false;

		/* Compare using absolute epsilon. */
		const auto diff = std::abs(a - b);
		if (diff > e_abs) return true;

		/* Compare using relative epsilon. */
		const auto abs_a = std::abs(a);
		const auto abs_b = std::abs(b);
		const auto max_ab = std::max(abs_a, abs_b);
		return diff > max_ab * e_rel;
	}
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3>
	[[nodiscard]] constexpr bool fcmp_ne(T0 a, T1 b, T2 e_rel, T3 e_abs) noexcept
	{
		using promoted_t = detail::promote_t<T0, T1, T2, T3>;
		return fcmp_ne(promoted_t{a}, promoted_t{b}, promoted_t{e_rel}, promoted_t{e_abs});
	}

	/** Determines if \a a is not within epsilon \a e of \a b.
	 * @note If either \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T>
	[[nodiscard]] constexpr bool fcmp_ne(T a, T b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_ne(a, b, e, e); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2>
	[[nodiscard]] constexpr bool fcmp_ne(T0 a, T1 b, T2 e) noexcept { return fcmp_ne(a, b, e, e); }

	/** Determines if elements of vector \a a are within relative epsilon vector \a e_rel or absolute epsilon vector \a e_abs of vector \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> fcmp_eq(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &e_rel, const basic_vec<T, N, A> &e_abs) noexcept
	{
		/* Compare signs. */
		const auto signs_eq = signbit(a) == signbit(b);
		/* Compare infinities. */
		auto result = isinf(a) && isinf(b);

		/* Compare using absolute epsilon. */
		const auto diff = abs(a - b);
		result |= diff <= e_abs;

		/* Compare using relative epsilon. */
		const auto abs_a = abs(a);
		const auto abs_b = abs(b);
		const auto max_ab = max(abs_a, abs_b);
		return signs_eq & (result | (diff <= max_ab * e_rel));
	}
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, std::size_t N, typename A>
	[[nodiscard]] inline auto fcmp_eq(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &e_rel, const basic_vec<T3, N, A> &e_abs) noexcept
	{
		using promoted_t = vec<detail::promote_t<T0, T1, T2, T3>, N, A>;
		return fcmp_eq(promoted_t{a}, promoted_t{b}, promoted_t{e_rel}, promoted_t{e_abs});
	}

	/** Determines if elements of vector \a a are within epsilon vector \a e of vector \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> fcmp_eq(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &e) noexcept { return fcmp_eq(a, b, e, e); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A>
	[[nodiscard]] inline auto fcmp_eq(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &e) noexcept { return fcmp_eq(a, b, e, e); }

	/** Determines if elements of vector \a a are not within relative epsilon vector \a e_rel or absolute epsilon vector \a e_abs of vector \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> fcmp_ne(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &e_rel, const basic_vec<T, N, A> &e_abs) noexcept
	{
		/* Compare signs. */
		const auto signs_eq = signbit(a) == signbit(b);
		/* Compare infinities. */
		auto result = !(isinf(a) && isinf(b));

		/* Compare using absolute epsilon. */
		const auto diff = abs(a - b);
		result |= diff > e_abs;

		/* Compare using relative epsilon. */
		const auto abs_a = abs(a);
		const auto abs_b = abs(b);
		const auto max_ab = max(abs_a, abs_b);
		return !signs_eq & (result | (diff > max_ab * e_rel));
	}
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, std::size_t N, typename A>
	[[nodiscard]] inline auto fcmp_ne(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &e_rel, const basic_vec<T3, N, A> &e_abs) noexcept
	{
		using promoted_t = vec<detail::promote_t<T0, T1, T2, T3>, N, A>;
		return fcmp_ne(promoted_t{a}, promoted_t{b}, promoted_t{e_rel}, promoted_t{e_abs});
	}

	/** Determines if elements of vector \a a are not within epsilon vector \a e of vector \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> fcmp_ne(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec<T, N, A> &e) noexcept { return fcmp_ne(a, b, e, e); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A>
	[[nodiscard]] inline auto fcmp_ne(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, const basic_vec<T2, N, A> &e) noexcept { return fcmp_ne(a, b, e, e); }

	/** Determines if elements of vector \a a are within relative epsilon \a e_rel or absolute epsilon \a e_abs of vector \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> fcmp_eq(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, T e_rel, T e_abs) noexcept { return fcmp_eq(a, b, basic_vec<T, N, A>{e_rel}, basic_vec<T, N, A>{e_abs}); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, std::size_t N, typename A>
	[[nodiscard]] inline auto fcmp_eq(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, T2 e_rel, T3 e_abs) noexcept
	{
		using promoted_value = detail::promote_t<T0, T1, T2, T3>;
		using promoted_vec = vec<promoted_value, N, A>;
		return fcmp_eq(promoted_vec{a}, promoted_vec{b}, static_cast<promoted_value>(e_rel), static_cast<promoted_value>(e_abs));
	}

	/** Determines if elements of vector \a a are within epsilon \a e of vector \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> fcmp_eq(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_eq(a, b, basic_vec<T, N, A>{e}); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A>
	[[nodiscard]] inline auto fcmp_eq(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, T2 e) noexcept { return fcmp_eq(a, b, e, e); }

	/** Determines if elements of vector \a a are not within relative epsilon \a e_rel or absolute epsilon \a e_abs of vector \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> fcmp_ne(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, T e_rel, T e_abs) noexcept { return fcmp_ne(a, b, basic_vec<T, N, A>{e_rel}, basic_vec<T, N, A>{e_abs}); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, std::size_t N, typename A>
	[[nodiscard]] inline auto fcmp_ne(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, T2 e_rel, T3 e_abs) noexcept
	{
		using promoted_value = detail::promote_t<T0, T1, T2, T3>;
		using promoted_vec = vec<promoted_value, N, A>;
		return fcmp_ne(promoted_vec{a}, promoted_vec{b}, static_cast<promoted_value>(e_rel), static_cast<promoted_value>(e_abs));
	}

	/** Determines if elements of vector \a a are not within epsilon \a e of vector \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline vec_mask<T, N, A> fcmp_ne(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_ne(a, b, basic_vec<T, N, A>{e}); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A>
	[[nodiscard]] inline auto fcmp_ne(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b, T2 e) noexcept { return fcmp_ne(a, b, e, e); }
}
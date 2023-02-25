/*
 * Created by switchblade on 2023-02-24.
 */

#pragma once

#include "type_mat.hpp"
#include "fcmp_vec.hpp"

namespace sek
{
	/** Determines if elements of matrix \a a are within relative epsilon \a e_rel or absolute epsilon \a e_abs of matrix \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t NCols, std::size_t NRows, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_mat<T, NCols, NRows, A> &a, const basic_mat<T, NCols, NRows, A> &b, T e_rel, T e_abs) noexcept
	{
		auto result = fcmp_eq(a[0], b[0], e_rel, e_abs);
		for (std::size_t i = 1; i < NCols; ++i)
			result &= fcmp_eq(a[i], b[i], e_rel, e_abs);
		return all_of(result);
	}
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, std::size_t NCols, std::size_t NRows, typename A, typename Promoted = detail::promote_t<T0, T1, T2, T3>>
	[[nodiscard]] inline bool fcmp_eq(const basic_mat<T0, NCols, NRows, A> &a, const basic_mat<T1, NCols, NRows, A> &b, T2 e_rel, T3 e_abs) noexcept
	{
		using promoted_mat = mat<Promoted, NCols, NRows, A>;
		return fcmp_eq(promoted_mat{a}, promoted_mat{b}, static_cast<Promoted>(e_rel), static_cast<Promoted>(e_abs));
	}

	/** Determines if elements of matrix \a a are within epsilon \a e of matrix \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t NCols, std::size_t NRows, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_mat<T, NCols, NRows, A> &a, const basic_mat<T, NCols, NRows, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_eq(a, b, e, e); }
	/** @copydoc fcmp_eq
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t NCols, std::size_t NRows, typename A>
	[[nodiscard]] inline bool fcmp_eq(const basic_mat<T0, NCols, NRows, A> &a, const basic_mat<T1, NCols, NRows, A> &b, T2 e) noexcept { return fcmp_eq(a, b, e, e); }

	/** Determines if elements of matrix \a a are not within relative epsilon \a e_rel or absolute epsilon \a e_abs of matrix \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t NCols, std::size_t NRows, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_mat<T, NCols, NRows, A> &a, const basic_mat<T, NCols, NRows, A> &b, T e_rel, T e_abs) noexcept
	{
		auto result = fcmp_ne(a[0], b[0], e_rel, e_abs);
		for (std::size_t i = 1; i < NCols; ++i)
			result &= fcmp_ne(a[i], b[i], e_rel, e_abs);
		return all_of(result);
	}
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, typename T3, std::size_t NCols, std::size_t NRows, typename A, typename Promoted = detail::promote_t<T0, T1, T2, T3>>
	[[nodiscard]] inline bool fcmp_ne(const basic_mat<T0, NCols, NRows, A> &a, const basic_mat<T1, NCols, NRows, A> &b, T2 e_rel, T3 e_abs) noexcept
	{
		using promoted_mat = mat<Promoted, NCols, NRows, A>;
		return fcmp_ne(promoted_mat{a}, promoted_mat{b}, static_cast<Promoted>(e_rel), static_cast<Promoted>(e_abs));
	}

	/** Determines if elements of matrix \a a are not within epsilon \a e of matrix \a b.
	 * @note If any of the elements of \a a or \a b are NaN, floating-point exceptions may be raised. */
	template<std::floating_point T, std::size_t NCols, std::size_t NRows, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_mat<T, NCols, NRows, A> &a, const basic_mat<T, NCols, NRows, A> &b, T e = std::numeric_limits<T>::epsilon()) noexcept { return fcmp_ne(a, b, e, e); }
	/** @copydoc fcmp_ne
	 * @note Arguments are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t NCols, std::size_t NRows, typename A>
	[[nodiscard]] inline bool fcmp_ne(const basic_mat<T0, NCols, NRows, A> &a, const basic_mat<T1, NCols, NRows, A> &b, T2 e) noexcept { return fcmp_ne(a, b, e, e); }
}
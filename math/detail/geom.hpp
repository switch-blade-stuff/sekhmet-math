/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "mbase.hpp"
#include "power.hpp"

namespace sek
{
	/** Calculates the dot product of elements in vectors \a a and \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T dot(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return hadd(a * b); }
	/** Calculates the cross product of elements in vectors \a a and \a b. */
	template<typename T, typename A>
	[[nodiscard]] inline basic_vec<T, 3, A> cross(const basic_vec<T, 3, A> &a, const basic_vec<T, 3, A> &b) noexcept
	{
		const auto a231 = shuffle<2, 3, 1>(a);
		const auto b231 = shuffle<2, 3, 1>(b);
		const auto a312 = shuffle<3, 1, 2>(a);
		const auto b312 = shuffle<3, 1, 2>(b);
		return fmsub(a231, b312, a312 * b231);
	}

	/** Calculates the magnitude of vector \a x. Equivalent to `std::sqrt(dot(x, x))`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline T magn(const basic_vec<T, N, A> &x) noexcept { return detail::sqrt(dot(x, x)); }
	/** Calculates the Euclidean distance between vectors \a a and \a b. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline T dist(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return magn(a - b); }
	/** @copydoc magn
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline typename Promoted::value_type magn(const basic_vec<T, N, A> &x) noexcept { return magn(Promoted{x}); }
	/** @copydoc dist
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline typename Promoted::value_type dist(const basic_vec<T0, N, A> &a, const basic_vec<T1, N, A> &b) noexcept { return magn(Promoted{a}, Promoted{b}); }

	/** Returns normalized copy (length 1) of vector \a x. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> normalize(const basic_vec<T, N, A> &x) noexcept { return x * detail::rsqrt(dot(x, x)); }
	/** @copydoc normalize
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T>, N, A>>
	[[nodiscard]] inline Promoted normalize(const basic_vec<T, N, A> &x) noexcept { return normalize(Promoted{x}); }

	/** Orients normal vector \a n to point in the direction specified by incident vector \a i and normal reference vector \a r.
	 * @return `dot(r, i) < 0 ? n : -n`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> faceforward(const basic_vec<T, N, A> &n, const basic_vec<T, N, A> &i, const basic_vec<T, N, A> &r) noexcept
	{
		return dot(r, i) < static_cast<T>(0) ? n : -n;
	}
	/** @copydoc normalize
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted faceforward(const basic_vec<T0, N, A> &n, const basic_vec<T1, N, A> &i, const basic_vec<T2, N, A> &r) noexcept
	{
		return faceforward(Promoted{n}, Promoted{i}, Promoted{r});
	}

	/** Calculates reflection direction for incident vector \a i and normal vector \a n.
	 * @return `i - 2 * dot(n, i) * n`. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> reflect(const basic_vec<T, N, A> &i, const basic_vec<T, N, A> &n) noexcept
	{
		return fmadd(n, {dot(n, i) * static_cast<T>(-2)}, i);
	}
	/** @copydoc reflect
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1>, N, A>>
	[[nodiscard]] inline Promoted reflect(const basic_vec<T0, N, A> &i, const basic_vec<T1, N, A> &n) noexcept
	{
		return reflect(Promoted{i}, Promoted{n});
	}

	/** Calculates refraction vector for incident vector \a i and normal vector \a n using refraction ratio of indices \a e. */
	template<std::floating_point T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> refract(const basic_vec<T, N, A> &i, const basic_vec<T, N, A> &n, T e) noexcept
	{
		const auto dp = dot(n, i);
		const auto k = detail::fmadd(e * e, detail::fmadd(dp, dp, static_cast<T>(-1)), static_cast<T>(-1));
		return k >= static_cast<T>(0) ? fmsub(i, {e}, n * detail::fmadd(e, dp, detail::sqrt(k))) : basic_vec<T, N, A>{0};
	}
	/** @copydoc refract
	 * @note Arguments and return type are promoted to `double`, or `long double` if one of the arguments is `long double`. */
	template<typename T0, typename T1, typename T2, std::size_t N, typename A, typename Promoted = vec<detail::promote_t<T0, T1, T2>, N, A>>
	[[nodiscard]] inline Promoted refract(const basic_vec<T0, N, A> &i, const basic_vec<T1, N, A> &n, T2 e) noexcept
	{
		return refract(Promoted{i}, Promoted{n}, static_cast<typename Promoted::value_type>(e));
	}
}
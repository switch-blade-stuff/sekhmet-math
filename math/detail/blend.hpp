/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "type_vec.hpp"

namespace sek
{
	/** Replaces element of vector \a b, if the corresponding element of vector mask \a m evaluates to `true`, otherwise selects element of \a a. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> blend(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b, const basic_vec_mask<T, N, A> &m)
	{
		return {dpm::blend(to_simd(a), to_simd(b), to_simd(m))};
	}
	/** Replaces element of vector mask \a b, if the corresponding element of vector mask \a m evaluates to `true`, otherwise selects element of \a a. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE basic_vec_mask<T, N, A> blend(const basic_vec_mask<T, N, A> &a, const basic_vec_mask<T, N, A> &b, const basic_vec_mask<T, N, A> &m)
	{
		return {dpm::blend(to_simd(a), to_simd(b), to_simd(m))};
	}

	template<typename M, typename T>
	class where_expression;

	template<typename T, std::size_t N, typename A, typename M>
	inline basic_vec<T, N, A> blend(const basic_vec<T, N, A> &a, const where_expression<M, basic_vec<T, N, A>> &b);
	template<typename T, std::size_t N, typename A, typename M>
	inline basic_vec<T, N, A> blend(const basic_vec<T, N, A> &a, const where_expression<M, const basic_vec<T, N, A>> &b);
	template<typename T, std::size_t N, typename A, typename M>
	inline basic_vec_mask<T, N, A> blend(const basic_vec_mask<T, N, A> &a, const where_expression<M, basic_vec_mask<T, N, A>> &b);
	template<typename T, std::size_t N, typename A, typename M>
	inline basic_vec_mask<T, N, A> blend(const basic_vec_mask<T, N, A> &a, const where_expression<M, const basic_vec_mask<T, N, A>> &b);

	template<typename M, typename V, typename Op>
	inline typename V::value_type reduce(const where_expression<M, V> &x, typename V::value_type identity, Op binary_op);
	template<typename M, typename V>
	inline typename V::value_type hmin(const where_expression<M, V> &x) noexcept;
	template<typename M, typename V>
	inline typename V::value_type hmax(const where_expression<M, V> &x) noexcept;
	template<typename M, typename V>
	inline typename V::value_type hadd(const where_expression<M, V> &x) noexcept;
	template<typename M, typename V>
	inline typename V::value_type hmul(const where_expression<M, V> &x) noexcept;
	template<typename M, typename V>
	inline typename V::value_type hand(const where_expression<M, V> &x) noexcept;
	template<typename M, typename V>
	inline typename V::value_type hxor(const where_expression<M, V> &x) noexcept;
	template<typename M, typename V>
	inline typename V::value_type hor(const where_expression<M, V> &x) noexcept;

	/** @brief Type used to select element(s) of `T` using mask `M`.
	 * @tparam T Instance of either `basic_vec` or `basic_vec_mask`, who's element(s) to select.
	 * @tparam M Instance of `basic_vec_mask` used to select elements of `T`. */
	template<typename M, typename T>
	class where_expression
	{
		template<typename K, typename U, typename O>
		friend inline typename U::value_type reduce(const where_expression<K, U> &, typename U::value_type, O);
		template<typename K, typename U>
		friend inline typename U::value_type hmin(const where_expression<K, U> &) noexcept;
		template<typename K, typename U>
		friend inline typename U::value_type hmax(const where_expression<K, U> &) noexcept;
		template<typename K, typename U>
		friend inline typename U::value_type hadd(const where_expression<K, U> &) noexcept;
		template<typename K, typename U>
		friend inline typename U::value_type hmul(const where_expression<K, U> &) noexcept;
		template<typename K, typename U>
		friend inline typename U::value_type hand(const where_expression<K, U> &) noexcept;
		template<typename K, typename U>
		friend inline typename U::value_type hxor(const where_expression<K, U> &) noexcept;
		template<typename K, typename U>
		friend inline typename U::value_type hor(const where_expression<K, U> &) noexcept;

	public:
		using mask_type = M;
		using value_type = std::remove_const_t<T>;

	private:
		using value_simd = typename value_type::simd_type;
		using mask_simd = typename mask_type::simd_type;
		using simd_expression = std::conditional_t<std::is_const_v<T>, dpm::const_where_expression<mask_simd, value_simd>, dpm::where_expression<mask_simd, value_simd>>;

	public:
		where_expression(const where_expression &) = delete;
		where_expression &operator=(const where_expression &) = delete;

		/** Initializes the where expression with a selection mask and a value vector or mask. */
		DPM_FORCEINLINE where_expression(const M &mask, T &data) noexcept : m_data(to_simd(mask), to_simd(data)) {}

		/** Returns a copy of the underlying vector or mask. */
		[[nodiscard]] DPM_FORCEINLINE value_type operator+() const && noexcept { return {+m_data}; }
		[[nodiscard]] DPM_FORCEINLINE value_type operator-() const && noexcept { return {-m_data}; }
		[[nodiscard]] DPM_FORCEINLINE value_type operator~() const && noexcept requires (requires(T x){ ~x; }) { return {~m_data}; }

		/** Returns either the selected elements of the underlying vector, or the elements specified by \a x. Equivalent to `blend(x, expr)`. */
		[[nodiscard]] DPM_FORCEINLINE value_type value_or(const value_type &x) const && noexcept { return {dpm::blend(to_simd(x), m_data)}; }

		DPM_FORCEINLINE void operator++(int) && noexcept requires (requires(simd_expression &&data){ data++; }) { m_data++; }
		DPM_FORCEINLINE void operator--(int) && noexcept requires (requires(simd_expression &&data){ data--; }) { m_data--; }
		DPM_FORCEINLINE void operator++() && noexcept requires (requires(simd_expression &&data){ ++data; }) { ++m_data; }
		DPM_FORCEINLINE void operator--() && noexcept requires (requires(simd_expression &&data){ --data; }) { --m_data; }

		template<typename U>
		DPM_FORCEINLINE void operator+=(U &&value) && noexcept requires (requires(simd_expression &&data){ data += value; }) { m_data += std::forward<U>(value); }
		template<typename U>
		DPM_FORCEINLINE void operator-=(U &&value) && noexcept requires (requires(simd_expression &&data){ data -= value; }) { m_data -= std::forward<U>(value); }

		template<typename U>
		DPM_FORCEINLINE void operator*=(U &&value) && noexcept requires (requires(simd_expression &&data){ data *= value; }) { m_data *= std::forward<U>(value); }
		template<typename U>
		DPM_FORCEINLINE void operator/=(U &&value) && noexcept requires (requires(simd_expression &&data){ data /= value; }) { m_data /= std::forward<U>(value); }
		template<typename U>
		DPM_FORCEINLINE void operator%=(U &&value) && noexcept requires (requires(simd_expression &&data){ data %= value; }) { m_data %= std::forward<U>(value); }

		template<typename U>
		DPM_FORCEINLINE void operator&=(U &&value) && noexcept requires (requires(simd_expression &&data){ data &= value; }) { m_data &= std::forward<U>(value); }
		template<typename U>
		DPM_FORCEINLINE void operator|=(U &&value) && noexcept requires (requires(simd_expression &&data){ data |= value; }) { m_data |= std::forward<U>(value); }
		template<typename U>
		DPM_FORCEINLINE void operator^=(U &&value) && noexcept requires (requires(simd_expression &&data){ data ^= value; }) { m_data ^= std::forward<U>(value); }

		template<typename U>
		DPM_FORCEINLINE void operator<<=(U &&value) && noexcept requires (requires(simd_expression &&data){ data <<= value; }) { m_data <<= std::forward<U>(value); }
		template<typename U>
		DPM_FORCEINLINE void operator>>=(U &&value) && noexcept requires (requires(simd_expression &&data){ data >>= value; }) { m_data >>= std::forward<U>(value); }

	private:
		simd_expression m_data;
	};

	/** Creates a where expression used to select elements of vector \a v using vector mask \a m. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE where_expression<basic_vec_mask<T, N, A>, basic_vec<T, N, A>> where(const basic_vec_mask<T, N, A> &m, basic_vec<T, N, A> &v) noexcept
	{
		return {m, v};
	}
	/** @copydoc where */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE where_expression<basic_vec_mask<T, N, A>, const basic_vec<T, N, A>> where(const basic_vec_mask<T, N, A> &m, const basic_vec<T, N, A> &v) noexcept
	{
		return {m, v};
	}
	/** Creates a where expression used to select elements of vector mask \a v using vector mask \a m. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE where_expression<basic_vec_mask<T, N, A>, basic_vec_mask<T, N, A>> where(const basic_vec_mask<T, N, A> &m, basic_vec_mask<T, N, A> &v) noexcept
	{
		return {m, v};
	}
	/** @copydoc where */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] DPM_FORCEINLINE where_expression<basic_vec_mask<T, N, A>, const basic_vec_mask<T, N, A>> where(const basic_vec_mask<T, N, A> &m, const basic_vec_mask<T, N, A> &v) noexcept
	{
		return {m, v};
	}

	/** Replaces elements of vector \a a with selected elements of where expression \a b. */
	template<typename T, std::size_t N, typename A, typename M>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> blend(const basic_vec<T, N, A> &a, const where_expression<M, basic_vec<T, N, A>> &b) { return std::move(b).value_or(a); }
	/** @copydoc blend */
	template<typename T, std::size_t N, typename A, typename M>
	[[nodiscard]] DPM_FORCEINLINE basic_vec<T, N, A> blend(const basic_vec<T, N, A> &a, const where_expression<M, const basic_vec<T, N, A>> &b) { return std::move(b).value_or(a); }
	/** Replaces elements of vector mask \a a with selected elements of where expression \a b. */
	template<typename T, std::size_t N, typename A, typename M>
	[[nodiscard]] DPM_FORCEINLINE basic_vec_mask<T, N, A> blend(const basic_vec_mask<T, N, A> &a, const where_expression<M, basic_vec_mask<T, N, A>> &b) { return std::move(b).value_or(a); }
	/** @copydoc blend */
	template<typename T, std::size_t N, typename A, typename M>
	[[nodiscard]] DPM_FORCEINLINE basic_vec_mask<T, N, A> blend(const basic_vec_mask<T, N, A> &a, const where_expression<M, const basic_vec_mask<T, N, A>> &b) { return std::move(b).value_or(a); }

	/** Calculates a reduction of selected elements from \a x using \a binary_op and identity element \a identity. */
	template<typename M, typename V, typename Op>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type reduce(const where_expression<M, V> &x, typename V::value_type identity, Op binary_op)
	{
		return dpm::reduce(x.m_data, identity, std::move(binary_op));
	}
	/** Calculates a sum of selected elements from \a x. Equivalent to `reduce(x, typename V::value_type{0}, binary_op)` */
	template<typename M, typename V, dpm::detail::template_instance<std::plus> Op>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type reduce(const where_expression<M, V> &x, Op binary_op) noexcept
	{
		return reduce(x, typename V::value_type{0}, std::move(binary_op));
	}
	/** Calculates a product of selected elements from \a x. Equivalent to `reduce(x, typename V::value_type{1}, binary_op)` */
	template<typename M, typename V, dpm::detail::template_instance<std::multiplies> Op>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type reduce(const where_expression<M, V> &x, Op binary_op) noexcept
	{
		return reduce(x, typename V::value_type{1}, std::move(binary_op));
	}
	/** Calculates a bitwise AND of selected elements from \a x. Equivalent to `reduce(x, typename V::value_type{ones-mask}, binary_op)` */
	template<typename M, typename V, dpm::detail::template_instance<std::bit_and> Op>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type reduce(const where_expression<M, V> &x, Op binary_op) noexcept
	{
		using mask_int = std::make_unsigned_t<typename V::value_type>;
		return reduce(x, std::bit_cast<typename V::value_type>(~mask_int{0}), std::move(binary_op));
	}
	/** Calculates a bitwise XOR of selected elements from \a x. Equivalent to `reduce(x, typename V::value_type{zeros-mask}, binary_op)` */
	template<typename M, typename V, dpm::detail::template_instance<std::bit_xor> Op>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type reduce(const where_expression<M, V> &x, Op binary_op) noexcept
	{
		using mask_int = std::make_unsigned_t<typename V::value_type>;
		return reduce(x, std::bit_cast<typename V::value_type>(mask_int{0}), binary_op);
	}
	/** Calculates a bitwise OR of selected elements from \a x. Equivalent to `reduce(x, typename V::value_type{zeros-mask}, binary_op)` */
	template<typename M, typename V, dpm::detail::template_instance<std::bit_or> Op>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type reduce(const where_expression<M, V> &x, Op binary_op) noexcept
	{
		using mask_int = std::make_unsigned_t<typename V::value_type>;
		return reduce(x, std::bit_cast<typename V::value_type>(mask_int{0}), binary_op);
	}

	/** Finds the horizontal minimum of all selected elements in \a x. */
	template<typename M, typename V>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type hmin(const where_expression<M, V> &x) noexcept { return dpm::hmin(x.m_data); }
	/** Finds the horizontal maximum of all selected elements in \a x. */
	template<typename M, typename V>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type hmax(const where_expression<M, V> &x) noexcept { return dpm::hmax(x.m_data); }

	/** Finds the horizontal sum of all selected elements in \a x. */
	template<typename M, typename V>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type hadd(const where_expression<M, V> &x) noexcept { return dpm::hadd(x.m_data); }
	/** Finds the horizontal product of all selected elements in \a x. */
	template<typename M, typename V>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type hmul(const where_expression<M, V> &x) noexcept { return dpm::hmul(x.m_data); }

	/** Finds the horizontal bitwise AND of all selected elements in \a x. */
	template<typename M, typename V>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type hand(const where_expression<M, V> &x) noexcept { return dpm::hand(x.m_data); }
	/** Finds the horizontal bitwise XOR of all selected elements in \a x. */
	template<typename M, typename V>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type hxor(const where_expression<M, V> &x) noexcept { return dpm::hxor(x.m_data); }
	/** Finds the horizontal bitwise OR of all selected elements in \a x. */
	template<typename M, typename V>
	[[nodiscard]] DPM_FORCEINLINE typename V::value_type hor(const where_expression<M, V> &x) noexcept { return dpm::hor(x.m_data); }
}
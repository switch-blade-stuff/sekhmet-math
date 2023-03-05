/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#include <stdexcept>
#include <ranges>

#include "shuffle.hpp"

namespace sek
{
	namespace detail
	{
		template<typename T>
		concept has_tuple_size = requires { typename std::void_t<decltype(sizeof(std::tuple_size<std::remove_cvref_t<T>>))>; };

		template<typename>
		struct arg_extent : std::integral_constant<std::size_t, 1> {};
		template<has_tuple_size T>
		struct arg_extent<T> : std::tuple_size<std::remove_cvref_t<T>> {};
		template<typename T>
		inline constexpr auto arg_extent_v = arg_extent<T>::value;

		template<typename, typename, typename>
		struct is_compatible_tuple : std::false_type {};
		template<typename T, typename U>
		struct is_compatible_tuple<T, U, std::index_sequence<>> : std::true_type {};
		template<typename T, typename U, std::size_t I, std::size_t... Is> requires std::is_convertible_v<std::tuple_element_t<I, U>, T>
		struct is_compatible_tuple<T, U, std::index_sequence<I, Is...>> : is_compatible_tuple<T, U, std::index_sequence<Is...>> {};

		template<typename T, typename U>
		struct is_compatible_arg : std::is_convertible<U, T> {};
		template<typename T, has_tuple_size U>
		struct is_compatible_arg<T, U> : is_compatible_tuple<T, U, std::make_index_sequence<std::tuple_size_v<U>>> {};

		template<typename T, std::size_t N, typename... Ts>
		concept compatible_args = std::conjunction_v<is_compatible_arg<T, std::remove_cvref_t<Ts>>...> && (arg_extent_v<Ts> + ...) == N;
	}

	/** @brief Structure used to define a boolean mask for a mathematical vector.
	 * @tparam T Value type of the mask's vector type.
	 * @tparam N Dimension of the vector mask.
	 * @tparam Abi ABI tag used for the underlying storage of the vector mask.
	 * @note \a N must be greater than `1` and the \a Abi tag size must match \a N. */
	template<typename T, std::size_t N, typename Abi>
	class basic_vec_mask
	{
		static_assert(N > 0, "Cannot create vector mask of 0 elements");
		static_assert(dpm::simd_size<T, Abi>::value == N, "Abi size must match vector mask size");

		template<typename U, std::size_t M, typename A>
		friend constexpr typename basic_vec_mask<U, M, A>::simd_type &to_simd(basic_vec_mask<U, M, A> &) noexcept;
		template<typename U, std::size_t M, typename A>
		friend constexpr const typename basic_vec_mask<U, M, A>::simd_type &to_simd(const basic_vec_mask<U, M, A> &) noexcept;

	public:
		using abi_type = Abi;
		using simd_type = dpm::simd_mask<T, Abi>;
		using value_type = typename simd_type::value_type;
		using vector_type = basic_vec<T, N, Abi>;

	private:
		static inline void assert_idx(std::size_t i) { if (i >= N) [[unlikely]] throw std::range_error("Element index out of range"); }

	public:
		constexpr basic_vec_mask() noexcept = default;

		/** Initializes elements of the vector mask to `static_cast<value_type>(x)`. */
		template<typename U>
		basic_vec_mask(U &&x) noexcept requires std::is_convertible_v<U, value_type> : m_data(std::forward<U>(x)) {}
		/** @brief Initializes vector mask from \a vals.
		 *
		 * Given argument `arg` from \a args of type `U`, if `U` is a tuple-like type, initializes `std::tuple_size_v<std::remove_cvref_t<U>>`
		 * elements of the vector mask as `static_cast<value_type>(get<I>(arg))`, where `I` is the index of the corresponding element in `U`.
		 * Otherwise, if `U` is not tuple-like, initializes the `N`th element of the vector mask as `static_cast<value_type>(arg)`. */
		template<typename... Args>
		basic_vec_mask(Args &&...args) noexcept requires detail::compatible_args<value_type, N, Args...> { fill_vals(std::forward<Args>(args)...); }

		/** Initializes vector mask from a range of elements pointed to by iterators \a first and \a last.
		 * @throw std::range_error If size of the range is less than `size()`. */
		template<std::forward_iterator I>
		basic_vec_mask(const I &first, const I &last) requires std::is_convertible_v<std::iter_value_t<I>, value_type>
		{
			for (std::size_t i = 0; i < size(); ++i, ++first)
			{
				if (first == last) [[unlikely]] throw std::range_error("Iterator distance is less than vector mask size");
				operator[](i) = static_cast<value_type>(*first);
			}
		}
		/** @copydoc basic_vec_mask */
		template<std::contiguous_iterator I>
		basic_vec_mask(const I &first, const I &last) requires std::is_convertible_v<std::iter_value_t<I>, value_type>
		{
			if (std::distance(first, last) < static_cast<std::ptrdiff_t>(N))
				[[unlikely]] throw std::range_error("Iterator distance is less than vector mask size");

			if constexpr (N > 1)
				m_data.copy_from(std::to_address(first), dpm::element_aligned);
			else
				m_data[0] = static_cast<value_type>(*first);
		}
		/** Initializes vector mask from a range of elements.
		 * @throw std::range_error If size of the range is less than `size()`. */
		template<std::ranges::forward_range R>
		basic_vec_mask(const R &data) requires std::is_convertible_v<std::ranges::range_value_t<R>, value_type> : basic_vec_mask(std::ranges::begin(data), std::ranges::end(data)) {}

		template<typename U, typename OtherAbi>
		basic_vec_mask(const dpm::simd_mask<U, OtherAbi> &data) noexcept : m_data(data) {}
		template<typename U, std::size_t M, typename OtherAbi>
		basic_vec_mask(const basic_vec_mask<U, M, OtherAbi> &other) noexcept { fill_other(other); }

		/** @brief Fills vector mask from \a args.
		 *
		 * Given argument `arg` from \a args of type `U`, if `U` is a tuple-like type, assigns `std::tuple_size_v<std::remove_cvref_t<U>>`
		 * elements of the vector mask from `static_cast<value_type>(get<I>(arg))`, where `I` is the index of the corresponding element in `U`.
		 * Otherwise, if `U` is not tuple-like, assigns the `N`th element of the vector mask from `static_cast<value_type>(arg)`. */
		template<typename... Args>
		basic_vec_mask &fill(Args &&...args) noexcept requires detail::compatible_args<value_type, N, Args...>
		{
			fill_vals(std::forward<Args>(args)...);
			return *this;
		}

		/** Returns the number of elements in the vector mask. */
		[[nodiscard]] constexpr std::size_t size() const noexcept { return N; }

		/** Returns `true` if all elements of the vector mask evaluate to `true`. */
		[[nodiscard]] inline operator bool() const noexcept;

		/** Returns reference to the `i`th element of the vector mask.
		 * @param i Index of the requested element.
		 * @throw std::range_error In case \a i exceeds `size()`. */
		[[nodiscard]] auto &at(std::size_t i)
		{
			assert_idx(i);
			return m_data[i];
		}
		/** Returns copy of the `i`th element of the vector mask.
		 * @param i Index of the requested element.
		 * @throw std::range_error In case \a i exceeds `size()`. */
		[[nodiscard]] value_type at(std::size_t i) const
		{
			assert_idx(i);
			return m_data[i];
		}

		/** Returns reference to the `i`th element of the vector mask.
		 * @param i Index of the requested element. */
		[[nodiscard]] auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		/** Returns copy of the `i`th element of the vector mask.
		 * @param i Index of the requested element. */
		[[nodiscard]] value_type operator[](std::size_t i) const noexcept { return m_data[i]; }

		SEK_MAKE_VEC_GETTERS(basic_vec_mask, value_type, x, y, z, w)
		SEK_MAKE_VEC_GETTERS(basic_vec_mask, value_type, r, g, b, a)

	private:
		template<std::size_t... Is, typename U, std::size_t M, typename OtherAbi>
		SEK_FORCEINLINE void fill_other(std::index_sequence<Is...>, const basic_vec_mask<U, M, OtherAbi> &other) noexcept
		{
			m_data = dpm::shuffle<Is...>(to_simd(other));
		}
		template<typename U, std::size_t M, typename OtherAbi>
		SEK_FORCEINLINE void fill_other(const basic_vec_mask<U, M, OtherAbi> &other) noexcept
		{
			if constexpr (M != N)
				fill_other(std::make_index_sequence<N>{}, other);
			else
				m_data = to_simd(other);
		}
		template<std::size_t J, std::size_t I, std::size_t... Is, typename U>
		SEK_FORCEINLINE void fill_tuple(std::index_sequence<I, Is...>, U &&x) noexcept
		{
			using std::get;
			operator[](J) = static_cast<value_type>(get<I>(x));
			if constexpr (sizeof...(Is) != 0) fill_tuple < J + 1 > (std::index_sequence<Is...>{}, std::forward<U>(x));
		}
		template<std::size_t I = 0, typename U, typename... Us>
		SEK_FORCEINLINE void fill_vals(U &&x, Us &&...args) noexcept
		{
			if constexpr (I < N)
			{
				if constexpr (detail::has_tuple_size<U>)
					fill_tuple<I>(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<U>>>{}, std::forward<U>(x));
				else
					operator[](I) = static_cast<value_type>(x);
				if constexpr (sizeof...(Us) != 0) fill_vals < I + detail::arg_extent_v<U>>(std::forward<Us>(args)...);
			}
		}

		simd_type m_data = {};
	};

	/** Returns reference to the underlying `dpm::simd_mask` object of the vector mask. */
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_vec_mask<T, N, Abi>::simd_type &to_simd(basic_vec_mask<T, N, Abi> &x) noexcept { return x.m_data; }
	/** @copydoc to_simd */
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_vec_mask<T, N, Abi>::simd_type &to_simd(const basic_vec_mask<T, N, Abi> &x) noexcept { return x.m_data; }

	/** Shuffles elements of the vector mask according to the indices specified by `Is`. */
	template<std::size_t... Is, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, sizeof...(Is), math_abi::deduce_t<T, sizeof...(Is), Abi>> shuffle(const basic_vec_mask<T, N, Abi> &x) noexcept
	{
		return {dpm::shuffle<Is...>(to_simd(x))};
	}

	/** Gets the `I`th element of the vector mask. */
	template<std::size_t I, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline auto &get(basic_vec_mask<T, N, Abi> &x) noexcept requires (I < N) { return x[I]; }
	/** @copydoc get */
	template<std::size_t I, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline auto get(const basic_vec_mask<T, N, Abi> &x) noexcept requires (I < N) { return x[I]; }

#pragma region "basic_vec_mask aliases"
	/** Alias for vector mask that uses implementation-defined ABI deduced from it's size, type and optional ABI hint. */
	template<typename T, std::size_t N, typename Abi = math_abi::fixed_size<N>>
	using vec_mask = basic_vec_mask<T, N, math_abi::deduce_t<T, N, Abi>>;
	/** Alias for deduced-ABI vector mask of 1 element. */
	template<typename T, typename Abi = math_abi::fixed_size<1>>
	using vec1_mask = vec_mask<T, 1, Abi>;
	/** Alias for deduced-ABI vector mask of 2 elements. */
	template<typename T, typename Abi = math_abi::fixed_size<2>>
	using vec2_mask = vec_mask<T, 2, Abi>;
	/** Alias for deduced-ABI vector mask of 3 elements. */
	template<typename T, typename Abi = math_abi::fixed_size<3>>
	using vec3_mask = vec_mask<T, 3, Abi>;
	/** Alias for deduced-ABI vector mask of 4 elements. */
	template<typename T, typename Abi = math_abi::fixed_size<4>>
	using vec4_mask = vec_mask<T, 4, Abi>;

	/** Alias for vector mask that uses implementation-defined compatible ABI. */
	template<typename T, std::size_t N>
	using compat_vec_mask = basic_vec_mask<T, N, math_abi::deduce_t<T, N, math_abi::compatible<T>>>;
	/** Alias for compatible-ABI vector mask of 1 element. */
	template<typename T>
	using compat_vec1_mask = compat_vec_mask<T, 1>;
	/** Alias for compatible-ABI vector mask of 2 elements. */
	template<typename T>
	using compat_vec2_mask = compat_vec_mask<T, 2>;
	/** Alias for compatible-ABI vector mask of 3 elements. */
	template<typename T>
	using compat_vec3_mask = compat_vec_mask<T, 3>;
	/** Alias for compatible-ABI vector mask of 4 elements. */
	template<typename T>
	using compat_vec4_mask = compat_vec_mask<T, 4>;

	/** Alias for vector mask that uses packed (non-vectorized) ABI. */
	template<typename T, std::size_t N>
	using packed_vec_mask = basic_vec_mask<T, N, math_abi::packed_buffer<N>>;
	/** Alias for packed-ABI vector mask of 1 element. */
	template<typename T>
	using packed_vec1_mask = packed_vec_mask<T, 1>;
	/** Alias for packed-ABI vector mask of 2 elements. */
	template<typename T>
	using packed_vec2_mask = packed_vec_mask<T, 2>;
	/** Alias for packed-ABI vector mask of 3 elements. */
	template<typename T>
	using packed_vec3_mask = packed_vec_mask<T, 3>;
	/** Alias for packed-ABI vector mask of 4 elements. */
	template<typename T>
	using packed_vec4_mask = packed_vec_mask<T, 4>;
#pragma endregion

#pragma region "basic_vec_mask operators"
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator!(const basic_vec_mask<T, N, Abi> &x) noexcept { return {!to_simd(x)}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator&(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) & to_simd(b)}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator|(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) | to_simd(b)}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator^(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) ^ to_simd(b)}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec_mask<T, N, Abi> &operator&=(basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept
	{
		to_simd(a) &= to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec_mask<T, N, Abi> &operator|=(basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept
	{
		to_simd(a) |= to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec_mask<T, N, Abi> &operator^=(basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept
	{
		to_simd(a) ^= to_simd(b);
		return a;
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator&&(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) && to_simd(b)}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator||(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) || to_simd(b)}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator==(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) == to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator!=(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) != to_simd(b)};
	}
#pragma endregion

#pragma region "basic_vec_mask reductions"
	/** Returns `true` if all of the elements of the vector mask are `true`. Otherwise returns `false`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline bool all_of(const basic_vec_mask<T, N, A> &x) noexcept { return all_of(to_simd(x)); }
	/** Returns `true` if at least one of the elements of the vector mask are `true`. Otherwise returns `false`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline bool any_of(const basic_vec_mask<T, N, A> &x) noexcept { return any_of(to_simd(x)); }
	/** Returns `true` if at none of the elements of the vector mask is `true`. Otherwise returns `false`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline bool none_of(const basic_vec_mask<T, N, A> &x) noexcept { return none_of(to_simd(x)); }
	/** Returns `true` if at least one of the elements of the vector mask is `true` and at least one is `false`. Otherwise returns `false`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline bool some_of(const basic_vec_mask<T, N, A> &x) noexcept { return some_of(to_simd(x)); }

	/** Returns the number of `true` elements of the vector mask. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline std::size_t popcount(const basic_vec_mask<T, N, A> &x) noexcept { return popcount(to_simd(x)); }
	/** Returns the index of the first `true` element of the vector mask. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline std::size_t find_first_set(const basic_vec_mask<T, N, A> &x) noexcept { return find_first_set(to_simd(x)); }
	/** Returns the index of the last `true` element of the vector mask. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline std::size_t find_last_set(const basic_vec_mask<T, N, A> &x) noexcept { return find_last_set(to_simd(x)); }
#pragma endregion

	template<typename T, std::size_t N, typename A>
	basic_vec_mask<T, N, A>::operator bool() const noexcept { return all_of(*this); }

	/** @brief Structure used to define a mathematical vector.
	 * @tparam T Value type stored by the vector.
	 * @tparam N Dimension of the vector.
	 * @tparam Abi ABI tag used for the underlying storage of the vector.
	 * @note \a N must be greater than `1` and the \a Abi tag size must match \a N. */
	template<typename T, std::size_t N, typename Abi>
	class basic_vec
	{
		static_assert(N > 0, "Cannot create vector of 0 elements");
		static_assert(dpm::simd_size<T, Abi>::value == N, "Abi size must match vector size");

		template<typename U, std::size_t M, typename A>
		friend constexpr typename basic_vec<U, M, A>::simd_type &to_simd(basic_vec<U, M, A> &) noexcept;
		template<typename U, std::size_t M, typename A>
		friend constexpr const typename basic_vec<U, M, A>::simd_type &to_simd(const basic_vec<U, M, A> &) noexcept;

	public:
		using abi_type = Abi;
		using simd_type = dpm::simd<T, Abi>;
		using value_type = typename simd_type::value_type;
		using mask_type = basic_vec_mask<T, N, Abi>;

		/** Returns an `up` unit vector. Equivalent to `basic_vec{0, 1}`.
		 * @note This overload is defined only for 2D vectors. */
		[[nodiscard]] static inline basic_vec up() noexcept requires (std::is_signed_v<T> && N == 2);
		/** Returns a `down` unit vector. Equivalent to `basic_vec{0, -1}`.
		 * @note This overload is defined only for 2D vectors. */
		[[nodiscard]] static inline basic_vec down() noexcept requires (std::is_signed_v<T> && N == 2);
		/** Returns a `left` unit vector. Equivalent to `basic_vec{-1, 0}`.
		 * @note This overload is defined only for 2D vectors. */
		[[nodiscard]] static inline basic_vec left() noexcept requires (std::is_signed_v<T> && N == 2);
		/** Returns a `right` unit vector. Equivalent to `basic_vec{1, 0}`.
		 * @note This overload is defined only for 2D vectors. */
		[[nodiscard]] static inline basic_vec right() noexcept requires (std::is_signed_v<T> && N == 2);

		/** Returns an `up` unit vector. Equivalent to `basic_vec{0, 1, 0}`.
		 * @note This overload is defined only for 3D vectors. */
		[[nodiscard]] static inline basic_vec up() noexcept requires (std::is_signed_v<T> && N == 3);
		/** Returns a `down` unit vector. Equivalent to `basic_vec{0, -1, 0}`.
		 * @note This overload is defined only for 3D vectors. */
		[[nodiscard]] static inline basic_vec down() noexcept requires (std::is_signed_v<T> && N == 3);
		/** Returns a `left` unit vector. Equivalent to `basic_vec{-1, 0, 0}`.
		 * @note This overload is defined only for 3D vectors. */
		[[nodiscard]] static inline basic_vec left() noexcept requires (std::is_signed_v<T> && N == 3);
		/** Returns a `right` unit vector. Equivalent to `basic_vec{1, 0, 0}`.
		 * @note This overload is defined only for 3D vectors. */
		[[nodiscard]] static inline basic_vec right() noexcept requires (std::is_signed_v<T> && N == 3);
		/** Returns a `forward` unit vector. Equivalent to `basic_vec{0, 0, -1}` (or `basic_vec{0, 0, 1}` if using left-handed coordinates).
		 * @note This overload is defined only for 3D vectors. */
		[[nodiscard]] static inline basic_vec forward() noexcept requires (std::is_signed_v<T> && N == 3);
		/** Returns a `backward` unit vector. Equivalent to `basic_vec{0, 0, 1}` (or `basic_vec{0, 0, -1}` if using left-handed coordinates).
		 * @note This overload is defined only for 3D vectors. */
		[[nodiscard]] static inline basic_vec backward() noexcept requires (std::is_signed_v<T> && N == 3);

	private:
		static inline void assert_idx(std::size_t i) { if (i >= N) [[unlikely]] throw std::range_error("Element index out of range"); }

	public:
		constexpr basic_vec() noexcept = default;

		/** Initializes elements of the vector to `static_cast<value_type>(x)`. */
		template<typename U>
		basic_vec(U &&x) noexcept requires std::is_convertible_v<U, value_type> : m_data(std::forward<U>(x)) {}
		/** @brief Initializes vector from \a vals.
		 *
		 * Given argument `arg` from \a args of type `U`, if `U` is a tuple-like type, initializes `std::tuple_size_v<std::remove_cvref_t<U>>`
		 * elements of the vector as `static_cast<value_type>(get<I>(arg))`, where `I` is the index of the corresponding element in `U`.
		 * Otherwise, if `U` is not tuple-like, initializes the `N`th element of the vector as `static_cast<value_type>(arg)`. */
		template<typename... Args>
		basic_vec(Args &&...args) noexcept requires detail::compatible_args<value_type, N, Args...> { fill_vals(std::forward<Args>(args)...); }

		/** Initializes vector from a range of elements pointed to by iterators \a first and \a last.
		 * @throw std::range_error If size of the range is less than `size()`. */
		template<std::forward_iterator I>
		basic_vec(const I &first, const I &last) requires std::is_convertible_v<std::iter_value_t<I>, value_type>
		{
			for (std::size_t i = 0; i < size(); ++i, ++first)
			{
				if (first == last) [[unlikely]] throw std::range_error("Iterator distance is less than vector mask size");
				operator[](i) = static_cast<value_type>(*first);
			}
		}
		/** @copydoc basic_vec */
		template<std::contiguous_iterator I>
		basic_vec(const I &first, const I &last) requires std::is_convertible_v<std::iter_value_t<I>, value_type>
		{
			if (std::distance(first, last) < static_cast<std::ptrdiff_t>(N))
				[[unlikely]] throw std::range_error("Iterator distance is less than vector mask size");

			if constexpr (N > 1)
				m_data.copy_from(std::to_address(first), dpm::element_aligned);
			else
				m_data[0] = static_cast<value_type>(*first);
		}
		/** Initializes vector from a range of elements.
		 * @throw std::range_error If size of the range is less than `size()`. */
		template<std::ranges::forward_range R>
		basic_vec(const R &data) requires std::is_convertible_v<std::ranges::range_value_t<R>, value_type> : basic_vec(std::ranges::begin(data), std::ranges::end(data)) {}

		template<typename U, typename OtherAbi>
		basic_vec(const dpm::simd<U, OtherAbi> &data) noexcept : m_data(data) {}
		template<typename U, std::size_t M, typename OtherAbi>
		basic_vec(const basic_vec<U, M, OtherAbi> &other) noexcept { fill_other(other); }

		/** @brief Fills vector from \a args.
		 *
		 * Given argument `arg` from \a args of type `U`, if `U` is a tuple-like type, assigns `std::tuple_size_v<std::remove_cvref_t<U>>`
		 * elements of the vector from `static_cast<value_type>(get<I>(arg))`, where `I` is the index of the corresponding element in `U`.
		 * Otherwise, if `U` is not tuple-like, assigns the `N`th element of the vector from `static_cast<value_type>(arg)`. */
		template<typename... Args>
		basic_vec &fill(Args &&...args) noexcept requires detail::compatible_args<value_type, N, Args...>
		{
			fill_vals(std::forward<Args>(args)...);
			return *this;
		}

		/** Returns the number of elements in the vector. */
		[[nodiscard]] constexpr std::size_t size() const noexcept { return N; }

		/** Returns reference to the `i`th element of the vector.
		 * @param i Index of the requested element.
		 * @throw std::range_error In case \a i exceeds `size()`. */
		[[nodiscard]] auto &at(std::size_t i)
		{
			assert_idx(i);
			return m_data[i];
		}
		/** Returns copy of the `i`th element of the vector.
		 * @param i Index of the requested element.
		 * @throw std::range_error In case \a i exceeds `size()`. */
		[[nodiscard]] value_type at(std::size_t i) const
		{
			assert_idx(i);
			return m_data[i];
		}

		/** Returns reference to the `i`th element of the vector.
		 * @param i Index of the requested element. */
		[[nodiscard]] auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		/** Returns copy of the `i`th element of the vector.
		 * @param i Index of the requested element. */
		[[nodiscard]] value_type operator[](std::size_t i) const noexcept { return m_data[i]; }

		SEK_MAKE_VEC_GETTERS(basic_vec, value_type, x, y, z, w)
		SEK_MAKE_VEC_GETTERS(basic_vec, value_type, r, g, b, a)

	private:
		template<std::size_t... Is, typename U, std::size_t M, typename OtherAbi>
		SEK_FORCEINLINE void fill_other(std::index_sequence<Is...>, const basic_vec<U, M, OtherAbi> &other) noexcept
		{
			m_data = dpm::shuffle<Is...>(to_simd(other));
		}
		template<typename U, std::size_t M, typename OtherAbi>
		SEK_FORCEINLINE void fill_other(const basic_vec<U, M, OtherAbi> &other) noexcept
		{
			if constexpr (M != N)
				fill_other(std::make_index_sequence<N>{}, other);
			else
				m_data = to_simd(other);
		}
		template<std::size_t J, std::size_t I, std::size_t... Is, typename U>
		SEK_FORCEINLINE void fill_tuple(std::index_sequence<I, Is...>, U &&x) noexcept
		{
			using std::get;
			operator[](J) = static_cast<value_type>(get<I>(x));
			if constexpr (sizeof...(Is) != 0) fill_tuple < J + 1 > (std::index_sequence<Is...>{}, std::forward<U>(x));
		}
		template<std::size_t I = 0, typename U, typename... Us>
		SEK_FORCEINLINE void fill_vals(U &&x, Us &&...args) noexcept
		{
			if constexpr (I < N)
			{
				if constexpr (detail::has_tuple_size<U>)
					fill_tuple<I>(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<U>>>{}, std::forward<U>(x));
				else
					operator[](I) = static_cast<value_type>(x);
				if constexpr (sizeof...(Us) != 0) fill_vals < I + detail::arg_extent_v<U>>(std::forward<Us>(args)...);
			}
		}

		simd_type m_data = {};
	};

	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::up() noexcept requires (std::is_signed_v<T> && N == 2) { return basic_vec<T, N, A>{0, 1}; }
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::down() noexcept requires (std::is_signed_v<T> && N == 2) { return basic_vec<T, N, A>{0, -1}; }
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::left() noexcept requires (std::is_signed_v<T> && N == 2) { return basic_vec<T, N, A>{-1, 0}; }
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::right() noexcept requires (std::is_signed_v<T> && N == 2) { return basic_vec<T, N, A>{1, 0}; }

	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::up() noexcept requires (std::is_signed_v<T> && N == 3) { return basic_vec<T, N, A>{0, 1, 0}; }
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::down() noexcept requires (std::is_signed_v<T> && N == 3) { return basic_vec<T, N, A>{0, -1, 0}; }
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::left() noexcept requires (std::is_signed_v<T> && N == 3) { return basic_vec<T, N, A>{-1, 0, 0}; }
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::right() noexcept requires (std::is_signed_v<T> && N == 3) { return basic_vec<T, N, A>{1, 0, 0}; }

#ifndef SEK_FORCE_LEFT_HANDED
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::forward() noexcept requires (std::is_signed_v<T> && N == 3) { return basic_vec<T, N, A>{0, 0, -1}; }
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::backward() noexcept requires (std::is_signed_v<T> && N == 3) { return basic_vec<T, N, A>{0, 0, 1}; }
#else
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::forward() noexcept requires (std::is_signed_v<T> && N == 3) { return basic_vec<T, N, A>{0, 0, 1}; }
	template<typename T, std::size_t N, typename A>
	basic_vec<T, N, A> basic_vec<T, N, A>::backward() noexcept requires (std::is_signed_v<T> && N == 3) { return basic_vec<T, N, A>{0, 0, -1}; }
#endif

	/** Returns reference to the underlying `dpm::simd` object of the vector. */
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_vec<T, N, Abi>::simd_type &to_simd(basic_vec<T, N, Abi> &x) noexcept { return x.m_data; }
	/** @copydoc to_simd */
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_vec<T, N, Abi>::simd_type &to_simd(const basic_vec<T, N, Abi> &x) noexcept { return x.m_data; }

	/** Shuffles elements of the vector according to the indices specified by `Is`. */
	template<std::size_t... Is, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, sizeof...(Is), math_abi::deduce_t<T, sizeof...(Is), Abi>> shuffle(const basic_vec<T, N, Abi> &x) noexcept
	{
		return {dpm::shuffle<Is...>(to_simd(x))};
	}

	/** Gets the `I`th element of the vector. */
	template<std::size_t I, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline auto &get(basic_vec<T, N, Abi> &x) noexcept requires (I < N) { return x[I]; }
	/** @copydoc get */
	template<std::size_t I, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline auto get(const basic_vec<T, N, Abi> &x) noexcept requires (I < N) { return x[I]; }

#pragma region "basic_vec aliases"
	/** Alias for vector that uses implementation-defined ABI deduced from it's size, type and optional ABI hint. */
	template<typename T, std::size_t N, typename Abi = math_abi::fixed_size<N>>
	using vec = basic_vec<T, N, math_abi::deduce_t<T, N, Abi>>;
	/** Alias for deduced-ABI vector of 1 element. */
	template<typename T, typename Abi = math_abi::fixed_size<1>>
	using vec1 = vec<T, 1, Abi>;
	/** Alias for deduced-ABI vector of 2 elements. */
	template<typename T, typename Abi = math_abi::fixed_size<2>>
	using vec2 = vec<T, 2, Abi>;
	/** Alias for deduced-ABI vector of 3 elements. */
	template<typename T, typename Abi = math_abi::fixed_size<3>>
	using vec3 = vec<T, 3, Abi>;
	/** Alias for deduced-ABI vector of 4 elements. */
	template<typename T, typename Abi = math_abi::fixed_size<4>>
	using vec4 = vec<T, 4, Abi>;

	/** Alias for vector that uses implementation-defined compatible ABI. */
	template<typename T, std::size_t N>
	using compat_vec = basic_vec<T, N, math_abi::deduce_t<T, N, math_abi::compatible<T>>>;
	/** Alias for compatible-ABI vector of 1 element. */
	template<typename T>
	using compat_vec1 = compat_vec<T, 1>;
	/** Alias for compatible-ABI vector of 2 elements. */
	template<typename T>
	using compat_vec2 = compat_vec<T, 2>;
	/** Alias for compatible-ABI vector of 3 elements. */
	template<typename T>
	using compat_vec3 = compat_vec<T, 3>;
	/** Alias for compatible-ABI vector of 4 elements. */
	template<typename T>
	using compat_vec4 = compat_vec<T, 4>;

	/** Alias for vector that uses packed (non-vectorized) ABI. */
	template<typename T, std::size_t N>
	using packed_vec = basic_vec<T, N, math_abi::packed_buffer<N>>;
	/** Alias for packed-ABI vector of 1 element. */
	template<typename T>
	using packed_vec1 = packed_vec<T, 1>;
	/** Alias for packed-ABI vector of 2 elements. */
	template<typename T>
	using packed_vec2 = packed_vec<T, 2>;
	/** Alias for packed-ABI vector of 3 elements. */
	template<typename T>
	using packed_vec3 = packed_vec<T, 3>;
	/** Alias for packed-ABI vector of 4 elements. */
	template<typename T>
	using packed_vec4 = packed_vec<T, 4>;
#pragma endregion

#pragma region "basic_vec operators"
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> operator+(const basic_vec<T, N, Abi> &x) noexcept requires (requires { +to_simd(x); }) { return {+to_simd(x)}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> operator-(const basic_vec<T, N, Abi> &x) noexcept requires (requires { -to_simd(x); }) { return {-to_simd(x)}; }

	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> operator++(basic_vec<T, N, Abi> &x, int) noexcept requires (requires { to_simd(x)++; }) { return {to_simd(x)++}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> operator--(basic_vec<T, N, Abi> &x, int) noexcept requires (requires { to_simd(x)--; }) { return {to_simd(x)--}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator++(basic_vec<T, N, Abi> &x) noexcept requires (requires { ++to_simd(x); }) { return ++to_simd(x); }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator--(basic_vec<T, N, Abi> &x) noexcept requires (requires { --to_simd(x); }) { return --to_simd(x); }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator+(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) + to_simd(b); })
	{
		return {to_simd(a) + to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator-(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) - to_simd(b); })
	{
		return {to_simd(a) - to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator+=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) += to_simd(b); })
	{
		to_simd(a) += to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator-=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) -= to_simd(b); })
	{
		to_simd(a) -= to_simd(b);
		return a;
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator+(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a + basic_vec<T, N, Abi>{b}; }) { return a + basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator-(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a - basic_vec<T, N, Abi>{b}; }) { return a - basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator+=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a += basic_vec<T, N, Abi>{b}; }) { return a += basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator-=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a -= basic_vec<T, N, Abi>{b}; }) { return a -= basic_vec<T, N, Abi>{b}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator*(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) * to_simd(b); })
	{
		return {to_simd(a) * to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator/(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) / to_simd(b); })
	{
		return {to_simd(a) / to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator%(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) % to_simd(b); })
	{
		return {to_simd(a) % to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator*=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) *= to_simd(b); })
	{
		to_simd(a) *= to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator/=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) /= to_simd(b); })
	{
		to_simd(a) /= to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator%=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) %= to_simd(b); })
	{
		to_simd(a) %= to_simd(b);
		return a;
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator*(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a * basic_vec<T, N, Abi>{b}; }) { return a * basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator/(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a / basic_vec<T, N, Abi>{b}; }) { return a / basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator%(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a % basic_vec<T, N, Abi>{b}; }) { return a % basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator*=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a *= basic_vec<T, N, Abi>{b}; }) { return a *= basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator/=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a /= basic_vec<T, N, Abi>{b}; }) { return a /= basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator%=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a %= basic_vec<T, N, Abi>{b}; }) { return a %= basic_vec<T, N, Abi>{b}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator>>(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) >> to_simd(b); })
	{
		return {to_simd(a) >> to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator<<(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) << to_simd(b); })
	{
		return {to_simd(a) << to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator>>=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) >>= to_simd(b); })
	{
		to_simd(a) >>= to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator<<=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) <<= to_simd(b); })
	{
		to_simd(a) <<= to_simd(b);
		return a;
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator>>(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { to_simd(a) >> b; })
	{
		return {to_simd(a) >> b};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, N, Abi> operator<<(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { to_simd(a) << b; })
	{
		return {to_simd(a) << b};
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator>>=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { to_simd(a) >>= b; })
	{
		to_simd(a) >>= b;
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	inline basic_vec<T, N, Abi> &operator<<=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { to_simd(a) <<= b; })
	{
		to_simd(a) <<= b;
		return a;
	}

	/** Logically shifts elements of vector \a x left by a constant number of bits `N`. */
	template<std::size_t N, std::integral T, std::size_t M, typename A>
	[[nodiscard]] inline basic_vec<T, M, A> lsl(const basic_vec<T, M, A> &x) noexcept requires (N < std::numeric_limits<T>::digits) { return {dpm::lsl<N>(to_simd(x))}; }
	/** Logically shifts elements of vector \a x right by a constant number of bits `N`. */
	template<std::size_t N, std::integral T, std::size_t M, typename A>
	[[nodiscard]] inline basic_vec<T, M, A> lsr(const basic_vec<T, M, A> &x) noexcept requires (N < std::numeric_limits<T>::digits) { return {dpm::lsr<N>(to_simd(x))}; }
	/** Arithmetically shifts elements of vector \a x left by a constant number of bits `N`. */
	template<std::size_t N, std::integral T, std::size_t M, typename A>
	[[nodiscard]] inline basic_vec<T, M, A> asl(const basic_vec<T, M, A> &x) noexcept requires (N < std::numeric_limits<T>::digits) { return {dpm::asl<N>(to_simd(x))}; }
	/** Arithmetically shifts elements of vector \a x right by a constant number of bits `N`. */
	template<std::size_t N, std::integral T, std::size_t M, typename A>
	[[nodiscard]] inline basic_vec<T, M, A> asr(const basic_vec<T, M, A> &x) noexcept requires (N < std::numeric_limits<T>::digits) { return {dpm::asr<N>(to_simd(x))}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator!(const basic_vec<T, N, Abi> &x) noexcept { return {!to_simd(x)}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator==(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) == to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator!=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) != to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator<=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) <= to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator>=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) >= to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator<(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) < to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, N, Abi> operator>(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) > to_simd(b)};
	}
#pragma endregion

#pragma region "basic_vec reductions"
	/** Calculates a reduction of all elements from \a x using \a binary_op. */
	template<typename T, std::size_t N, typename A, typename Op = std::plus<>>
	[[nodiscard]] inline T reduce(const basic_vec<T, N, A> &x, Op binary_op = {}) { return reduce(to_simd(x), std::move(binary_op)); }

	/** Finds the horizontal sum of all elements in \a x. Equivalent to `reduce(x, std::plus<>{})`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T hadd(const basic_vec<T, N, A> &x) { return hadd(to_simd(x)); }
	/** Finds the horizontal product of all elements in \a x. Equivalent to `reduce(x, std::multiplies<>{})`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T hmul(const basic_vec<T, N, A> &x) { return hmul(to_simd(x)); }

	/** Finds the horizontal bitwise AND of all elements in \a x. Equivalent to `reduce(x, std::bit_and<>{})`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T hand(const basic_vec<T, N, A> &x) { return hand(to_simd(x)); }
	/** Finds the horizontal bitwise XOR of all elements in \a x. Equivalent to `reduce(x, std::bit_xor<>{})`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T hxor(const basic_vec<T, N, A> &x) { return hxor(to_simd(x)); }
	/** Finds the horizontal bitwise OR of all elements in \a x. Equivalent to `reduce(x, std::bit_or<>{})`. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T hor(const basic_vec<T, N, A> &x) { return hor(to_simd(x)); }

	/** Finds the horizontal minimum of elements in \a x. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T hmin(const basic_vec<T, N, A> &x) { return hmin(to_simd(x)); }
	/** Finds the horizontal maximum of elements in \a x. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline T hmax(const basic_vec<T, N, A> &x) { return hmax(to_simd(x)); }
#pragma endregion

#pragma region "basic_vec algorithms"
	/** Returns a vector of minimum elements of \a a and \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> min(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::min(to_simd(a), to_simd(b))}; }
	/** Returns a vector of maximum elements of \a a and \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> max(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept { return {dpm::max(to_simd(a), to_simd(b))}; }

	/** Returns a vector of minimum elements of \a a and scalar \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> min(const basic_vec<T, N, A> &a, T b) noexcept { return {dpm::min(to_simd(a), b)}; }
	/** Returns a vector of maximum elements of \a a and scalar \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> max(const basic_vec<T, N, A> &a, T b) noexcept { return {dpm::max(to_simd(a), b)}; }

	/** Returns a pair of vectors of minimum and maximum elements of \a a and \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline std::pair<basic_vec<T, N, A>, basic_vec<T, N, A>> minmax(const basic_vec<T, N, A> &a, const basic_vec<T, N, A> &b) noexcept
	{
		const auto [simd_min, simd_max] = dpm::minmax(to_simd(a), to_simd(b));
		return {simd_min, simd_max};
	}
	/** Returns a pair of vectors of minimum and maximum elements of \a a and scalar \a b. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline std::pair<basic_vec<T, N, A>, basic_vec<T, N, A>> minmax(const basic_vec<T, N, A> &a, T b) noexcept
	{
		const auto [simd_min, simd_max] = dpm::minmax(to_simd(a), b);
		return {simd_min, simd_max};
	}

	/** Clamps elements of \a x between corresponding elements of \a min and \a max. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> clamp(const basic_vec<T, N, A> &x, const basic_vec<T, N, A> &min, const basic_vec<T, N, A> &max) noexcept
	{
		return {dpm::clamp(to_simd(x), to_simd(min), to_simd(max))};
	}
	/** Clamps elements of \a x between \a min and \a max. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> clamp(const basic_vec<T, N, A> &x, T min, T max) noexcept
	{
		return {dpm::clamp(to_simd(x), to_simd(min), to_simd(max))};
	}

	/** Converts elements of vector \a x from degrees to radians. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> rad(const basic_vec<T, N, A> &x) noexcept { return x * T{180} / std::numbers::pi_v<T>; }
	/** Converts elements of vector \a x from radians to degrees. */
	template<typename T, std::size_t N, typename A>
	[[nodiscard]] inline basic_vec<T, N, A> deg(const basic_vec<T, N, A> &x) noexcept { return x * std::numbers::pi_v<T> / T{180}; }
#pragma endregion
}

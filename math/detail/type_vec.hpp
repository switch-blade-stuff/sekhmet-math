/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#include <stdexcept>

#include "shuffle.hpp"

namespace sek
{
	namespace detail
	{
		template<typename T>
		concept has_tuple_size = requires { typename std::void_t<decltype(sizeof(std::tuple_size<T>))>; };

		template<typename T>
		struct is_vec : std::false_type {};
		template<typename T, std::size_t N, typename A>
		struct is_vec<basic_vec<T, N, A>> : std::true_type {};
		template<typename T>
		concept vec_instance = is_vec<std::remove_cvref_t<T>>::value;

		template<typename T>
		struct is_vec_mask : std::false_type {};
		template<typename T, std::size_t N, typename A>
		struct is_vec_mask<basic_vec_mask<T, N, A>> : std::true_type {};
		template<typename T>
		concept vec_mask_instance = is_vec_mask<std::remove_cvref_t<T>>::value;

		template<typename... Ts>
		struct all_same : std::false_type {};
		template<typename T, std::same_as<T>... Ts>
		struct all_same<T, Ts...> : std::true_type {};

		template<typename, typename>
		struct common_tuple_type;
		template<typename T, std::size_t... Is>
		struct common_tuple_type<T, std::index_sequence<Is...>> : std::common_type<std::tuple_element_t<Is, T>...> {};

		template<typename>
		struct arg_extent : std::integral_constant<std::size_t, 1> {};
		template<has_tuple_size T>
		struct arg_extent<T> : std::tuple_size<T> {};
		template<typename T>
		inline constexpr auto arg_extent_v = arg_extent<std::remove_cvref_t<T>>::value;

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
		concept compatible_args = std::conjunction_v<is_compatible_arg<T, Ts>...> && (arg_extent_v<Ts> + ...) == N;
	}

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

	private:
		static inline void assert_idx(std::size_t i) { if (i >= N) [[unlikely]] throw std::range_error("Element index out of range"); }

	public:
		SEK_FORCEINLINE basic_vec_mask() noexcept : basic_vec_mask(false) {}
		template<typename U>
		SEK_FORCEINLINE basic_vec_mask(U x) noexcept requires std::is_convertible_v<U, value_type> : m_data(x) {}

		template<typename... Us>
		basic_vec_mask(Us ...vals) noexcept requires detail::compatible_args<value_type, N, Us...> { fill_impl(std::move(vals)...); }
		template<typename... Us>
		basic_vec_mask &fill(Us ...vals) noexcept requires detail::compatible_args<value_type, N, Us...>
		{
			fill_impl(std::move(vals)...);
			return *this;
		}

		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE basic_vec_mask(const dpm::simd_mask<U, OtherAbi> &data) noexcept : m_data(data) {}
		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE basic_vec_mask(const basic_vec_mask<U, N, OtherAbi> &other) noexcept : basic_vec_mask(to_simd(other)) {}

		/** Returns the number of elements in the vector mask. */
		[[nodiscard]] constexpr std::size_t size() const noexcept { return N; }

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
		[[nodiscard]] SEK_FORCEINLINE auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		/** Returns copy of the `i`th element of the vector mask.
		 * @param i Index of the requested element. */
		[[nodiscard]] SEK_FORCEINLINE value_type operator[](std::size_t i) const noexcept { return m_data[i]; }

		SEK_MAKE_VEC_GETTERS(basic_vec_mask, value_type, x, y, z, w)
		SEK_MAKE_VEC_GETTERS(basic_vec_mask, value_type, r, g, b, a)

	private:
		template<std::size_t J, std::size_t I, std::size_t... Is, typename U, typename... Us>
		SEK_FORCEINLINE void fill_tuple(std::index_sequence<I, Is...>, U &&x) noexcept
		{
			using std::get;
			operator[](J) = static_cast<value_type>(get<I>(x));
			if constexpr (sizeof...(Is) != 0) fill_tuple<J + 1>(std::index_sequence<Is...>{}, std::forward<U>(x));
		}
		template<std::size_t I = 0, typename U, typename... Us>
		SEK_FORCEINLINE void fill_impl(U &&x, Us &&...vals) noexcept
		{
			if constexpr(I < N)
			{
				if constexpr (requires{ typename std::tuple_size<U>; })
					fill_tuple<I>(std::make_index_sequence<std::tuple_size_v<U>>{}, std::forward<U>(x));
				else
					operator[](I) = static_cast<value_type>(x);
				if constexpr(sizeof...(Us) != 0) fill_impl<I + detail::arg_extent_v<U>>(std::forward<Us>(vals)...);
			}
		}

		simd_type m_data;
	};

	/** Returns reference to the underlying `dpm::simd_mask` object of the vector mask. */
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_vec_mask<T, N, Abi>::simd_type &to_simd(basic_vec_mask<T, N, Abi> &x) noexcept { return x.m_data; }
	/** @copydoc to_simd */
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_vec_mask<T, N, Abi>::simd_type &to_simd(const basic_vec_mask<T, N, Abi> &x) noexcept { return x.m_data; }

	/** Shuffles elements of the vector mask according to the indices specified by `Is`. */
	template<std::size_t... Is, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, sizeof...(Is), abi::deduce_t<T, sizeof...(Is), Abi>> shuffle(basic_vec_mask<T, N, Abi> x) noexcept
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
	template<typename T, std::size_t N, typename Abi = abi::fixed_size<N>>
	using vec_mask = basic_vec_mask<T, N, abi::deduce_t<T, N, Abi>>;
	/** Alias for deduced-ABI vector mask of 2 elements. */
	template<typename T, typename Abi = abi::fixed_size<2>>
	using vec2_mask = vec_mask<T, 2, Abi>;
	/** Alias for deduced-ABI vector mask of 3 elements. */
	template<typename T, typename Abi = abi::fixed_size<3>>
	using vec3_mask = vec_mask<T, 3, Abi>;
	/** Alias for deduced-ABI vector mask of 4 elements. */
	template<typename T, typename Abi = abi::fixed_size<4>>
	using vec4_mask = vec_mask<T, 4, Abi>;

	/** Alias for vector mask that uses compatible (implementation-defined) ABI. */
	template<typename T, std::size_t N>
	using compat_vec_mask = basic_vec_mask<T, N, abi::deduce_t<T, N, abi::compatible<T>>>;
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
	using packed_vec_mask = basic_vec_mask<T, N, abi::packed_buffer<N>>;
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
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator!(const basic_vec_mask<T, N, Abi> &x) noexcept { return {!to_simd(x)}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator&(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) & to_simd(b)}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator|(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) | to_simd(b)}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator^(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) ^ to_simd(b)}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator&&(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) && to_simd(b)}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator||(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept { return {to_simd(a) || to_simd(b)}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator==(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) == to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator!=(const basic_vec_mask<T, N, Abi> &a, const basic_vec_mask<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) != to_simd(b)};
	}
#pragma endregion

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

	private:
		static inline void assert_idx(std::size_t i) { if (i >= N) [[unlikely]] throw std::range_error("Element index out of range"); }

	public:
		SEK_FORCEINLINE basic_vec() noexcept : basic_vec(T{}) {}
		template<typename U>
		SEK_FORCEINLINE basic_vec(U x) noexcept requires std::is_convertible_v<U, value_type> : m_data(x) {}

		template<typename... Us>
		basic_vec(Us ...vals) noexcept requires detail::compatible_args<value_type, N, Us...> { fill_impl(std::move(vals)...); }
		template<typename... Us>
		basic_vec &fill(Us ...vals) noexcept requires detail::compatible_args<value_type, N, Us...>
		{
			fill_impl(std::move(vals)...);
			return *this;
		}

		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE basic_vec(const dpm::simd<U, OtherAbi> &data) noexcept : m_data(data) {}
		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE basic_vec(const basic_vec<U, N, OtherAbi> &other) noexcept : basic_vec(to_simd(other)) {}

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
		[[nodiscard]] SEK_FORCEINLINE auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		/** Returns copy of the `i`th element of the vector.
		 * @param i Index of the requested element. */
		[[nodiscard]] SEK_FORCEINLINE value_type operator[](std::size_t i) const noexcept { return m_data[i]; }

		SEK_MAKE_VEC_GETTERS(basic_vec, value_type, x, y, z, w)
		SEK_MAKE_VEC_GETTERS(basic_vec, value_type, r, g, b, a)

	private:
		template<std::size_t J, std::size_t I, std::size_t... Is, typename U, typename... Us>
		SEK_FORCEINLINE void fill_tuple(std::index_sequence<I, Is...>, U &&x) noexcept
		{
			using std::get;
			operator[](J) = static_cast<value_type>(get<I>(x));
			if constexpr (sizeof...(Is) != 0) fill_tuple<J + 1>(std::index_sequence<Is...>{}, std::forward<U>(x));
		}
		template<std::size_t I = 0, typename U, typename... Us>
		SEK_FORCEINLINE void fill_impl(U &&x, Us &&...vals) noexcept
		{
			if constexpr(I < N)
			{
				if constexpr (requires{ typename std::tuple_size<U>; })
					fill_tuple<I>(std::make_index_sequence<std::tuple_size_v<U>>{}, std::forward<U>(x));
				else
					operator[](I) = static_cast<value_type>(x);
				if constexpr(sizeof...(Us) != 0) fill_impl<I + detail::arg_extent_v<U>>(std::forward<Us>(vals)...);
			}
		}

		simd_type m_data;
	};

	/** Returns reference to the underlying `dpm::simd` object of the vector. */
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename basic_vec<T, N, Abi>::simd_type &to_simd(basic_vec<T, N, Abi> &x) noexcept { return x.m_data; }
	/** @copydoc to_simd */
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename basic_vec<T, N, Abi>::simd_type &to_simd(const basic_vec<T, N, Abi> &x) noexcept { return x.m_data; }

	/** Shuffles elements of the vector according to the indices specified by `Is`. */
	template<std::size_t... Is, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, sizeof...(Is), abi::deduce_t<T, sizeof...(Is), Abi>> shuffle(basic_vec<T, N, Abi> x) noexcept
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
	template<typename T, std::size_t N, typename Abi = abi::fixed_size<N>>
	using vec = basic_vec<T, N, abi::deduce_t<T, N, Abi>>;
	/** Alias for deduced-ABI vector of 2 elements. */
	template<typename T, typename Abi = abi::fixed_size<2>>
	using vec2 = vec<T, 2, Abi>;
	/** Alias for deduced-ABI vector of 3 elements. */
	template<typename T, typename Abi = abi::fixed_size<3>>
	using vec3 = vec<T, 3, Abi>;
	/** Alias for deduced-ABI vector of 4 elements. */
	template<typename T, typename Abi = abi::fixed_size<4>>
	using vec4 = vec<T, 4, Abi>;

	/** Alias for vector that uses compatible (implementation-defined) ABI. */
	template<typename T, std::size_t N>
	using compat_vec = basic_vec<T, N, abi::deduce_t<T, N, abi::compatible<T>>>;
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
	using packed_vec = basic_vec<T, N, abi::packed_buffer<N>>;
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
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator++(basic_vec<T, N, Abi> &x, int) noexcept requires (requires { to_simd(x)++; }) { return to_simd(x)++; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator--(basic_vec<T, N, Abi> &x, int) noexcept requires (requires { to_simd(x)--; }) { return to_simd(x)--; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator++(basic_vec<T, N, Abi> &x) noexcept requires (requires { ++to_simd(x); }) { return ++to_simd(x); }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator--(basic_vec<T, N, Abi> &x) noexcept requires (requires { --to_simd(x); }) { return --to_simd(x); }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator+(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) + to_simd(b); })
	{
		return {to_simd(a) + to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator-(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) - to_simd(b); })
	{
		return {to_simd(a) - to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator+=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) += to_simd(b); })
	{
		to_simd(a) += to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator-=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) -= to_simd(b); })
	{
		to_simd(a) -= to_simd(b);
		return a;
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator+(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a + basic_vec<T, N, Abi>{b}; }) { return a + basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator-(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a - basic_vec<T, N, Abi>{b}; }) { return a - basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator+=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a += basic_vec<T, N, Abi>{b}; }) { return a += basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator-=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a -= basic_vec<T, N, Abi>{b}; }) { return a -= basic_vec<T, N, Abi>{b}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator*(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) * to_simd(b); })
	{
		return {to_simd(a) * to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator/(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) / to_simd(b); })
	{
		return {to_simd(a) / to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator%(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) % to_simd(b); })
	{
		return {to_simd(a) % to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator*=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) *= to_simd(b); })
	{
		to_simd(a) *= to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator/=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) /= to_simd(b); })
	{
		to_simd(a) /= to_simd(b);
		return a;
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator%=(basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept requires (requires { to_simd(a) %= to_simd(b); })
	{
		to_simd(a) %= to_simd(b);
		return a;
	}

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator*(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a * basic_vec<T, N, Abi>{b}; }) { return a * basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator/(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a / basic_vec<T, N, Abi>{b}; }) { return a / basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> operator%(const basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a % basic_vec<T, N, Abi>{b}; }) { return a % basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator*=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a *= basic_vec<T, N, Abi>{b}; }) { return a *= basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator/=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a /= basic_vec<T, N, Abi>{b}; }) { return a /= basic_vec<T, N, Abi>{b}; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec<T, N, Abi> &operator%=(basic_vec<T, N, Abi> &a, T b) noexcept requires (requires { a %= basic_vec<T, N, Abi>{b}; }) { return a %= basic_vec<T, N, Abi>{b}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator!(const basic_vec<T, N, Abi> &x) noexcept { return {!to_simd(x)}; }

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator==(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) == to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator!=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) != to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator<=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) <= to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator>=(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) >= to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator<(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) < to_simd(b)};
	}
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE basic_vec_mask<T, N, Abi> operator>(const basic_vec<T, N, Abi> &a, const basic_vec<T, N, Abi> &b) noexcept
	{
		return {to_simd(a) > to_simd(b)};
	}
#pragma endregion
}

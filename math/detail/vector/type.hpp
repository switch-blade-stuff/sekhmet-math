/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#include "shuffle.hpp"

namespace sek::math
{
	namespace detail
	{
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

		template<typename T, typename... Ts>
		struct arg_value { using type = T; };
		template<typename T, std::size_t N, typename A, typename... Ts>
		struct arg_value<basic_vec<T, N, A>, Ts...> { using type = T; };
		template<typename T, std::size_t N, typename A, typename... Ts>
		struct arg_value<basic_vec_mask<T, N, A>, Ts...> { using type = bool; };
		template<typename... Ts>
		using arg_value_t = typename arg_value<std::remove_cvref_t<Ts>...>::type;

		template<typename T, typename... Ts>
		struct arg_abi { using type = abi::compatible<T>; };
		template<typename T, std::size_t N, typename A, typename... Ts>
		struct arg_abi<basic_vec<T, N, A>, Ts...> { using type = A; };
		template<typename T, std::size_t N, typename A, typename... Ts>
		struct arg_abi<basic_vec_mask<T, N, A>, Ts...> { using type = A; };
		template<typename... Ts>
		using arg_abi_t = typename arg_abi<std::remove_cvref_t<Ts>...>::type;

		template<typename>
		struct arg_extent : std::integral_constant<std::size_t, 1> {};
		template<typename T, std::size_t N, typename A>
		struct arg_extent<basic_vec<T, N, A>> : std::integral_constant<std::size_t, N> {};
		template<typename T>
		inline constexpr auto arg_extent_v = arg_extent<std::remove_cvref_t<T>>::value;

		template<typename... Ts>
		using combine_value = std::common_type_t<arg_value_t<Ts>...>;
		template<typename... Ts>
		using combine_abi = std::conditional_t<std::conjunction_v<all_same<Ts>...>, arg_abi_t<Ts...>, abi::compatible<arg_value_t<Ts...>>>;

		template<typename T, std::size_t N, typename... Ts>
		concept compatible_args = (std::is_convertible_v<arg_value_t<Ts>, T> && ...) && (arg_extent_v<Ts> + ...) == N;
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
		using data_t = dpm::simd_mask<T, Abi>;
		using abi_type = Abi;

	public:
		SEK_FORCEINLINE basic_vec_mask() noexcept : basic_vec_mask(false) {}
		template<typename U>
		SEK_FORCEINLINE basic_vec_mask(U x) noexcept requires std::is_convertible_v<U, bool> : m_data(x) {}

		template<typename... Us>
		basic_vec_mask(Us ...vals) noexcept requires detail::compatible_args<bool, N, Us...> { fill_impl(std::move(vals)...); }
		template<typename... Us>
		basic_vec_mask &fill(Us ...vals) noexcept requires detail::compatible_args<T, N, Us...>
		{
			fill_impl(std::move(vals)...);
			return *this;
		}

		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE basic_vec_mask(const dpm::simd_mask<U, OtherAbi> &data) noexcept : m_data(data) {}
		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE basic_vec_mask(const basic_vec_mask<U, N, OtherAbi> &other) noexcept : basic_vec_mask(to_simd(other)) {}

		[[nodiscard]] SEK_FORCEINLINE auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		[[nodiscard]] SEK_FORCEINLINE bool operator[](std::size_t i) const noexcept { return m_data[i]; }

		SEK_MAKE_VEC_GETTERS(basic_vec_mask, bool, x, y, z, w)
		SEK_MAKE_VEC_GETTERS(basic_vec_mask, bool, r, g, b, a)

	private:
		template<std::size_t I = 0, typename U, typename... Us>
		SEK_FORCEINLINE void fill_impl(U &&x, Us &&...vals) noexcept requires (!detail::vec_mask_instance<U>)
		{
			if constexpr (I < N)
			{
				operator[](I) = static_cast<T>(x);
				if constexpr (sizeof...(Us) != 0)
					fill_impl<I + 1>(std::forward<Us>(vals)...);
				else
					fill_impl<I + 1>(x);
			}
		}
		template<std::size_t I = 0, std::size_t J = 0, std::size_t M, typename OtherAbi, typename U, typename... Us>
		SEK_FORCEINLINE void fill_impl(const basic_vec<U, M, OtherAbi> &other, Us &&...vals) noexcept
		{
			if constexpr (J < M)
			{
				operator[](I) = static_cast<T>(other[J]);
				fill_impl<I + 1, J + 1>(other, std::forward<Us>(vals)...);
			}
			else if constexpr (sizeof...(Us) != 0)
				fill_impl<I>(std::forward<Us>(vals)...);
		}

		data_t m_data;
	};

	template<typename... Us>
	basic_vec_mask(Us &&...) -> basic_vec_mask<detail::combine_value<Us...>, (detail::arg_extent_v<Us> + ...), detail::combine_abi<Us...>>;

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

	/** Alias for vector mask that uses implementation-defined ABI deduced from it's size, type and optional Abi hint. */
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
		using simd_type = dpm::simd<T, Abi>;
		using abi_type = Abi;

	public:
		SEK_FORCEINLINE basic_vec() noexcept : basic_vec(T{}) {}
		template<typename U>
		SEK_FORCEINLINE basic_vec(U x) noexcept requires std::is_convertible_v<U, T> : m_data(x) {}

		template<typename... Us>
		basic_vec(Us ...vals) noexcept requires detail::compatible_args<T, N, Us...> { fill_impl(std::move(vals)...); }
		template<typename... Us>
		basic_vec &fill(Us ...vals) noexcept requires detail::compatible_args<T, N, Us...>
		{
			fill_impl(std::move(vals)...);
			return *this;
		}

		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE basic_vec(const dpm::simd<U, OtherAbi> &data) noexcept : m_data(data) {}
		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE basic_vec(const basic_vec<U, N, OtherAbi> &other) noexcept : basic_vec(to_simd(other)) {}

		[[nodiscard]] SEK_FORCEINLINE auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		[[nodiscard]] SEK_FORCEINLINE T operator[](std::size_t i) const noexcept { return m_data[i]; }

		SEK_MAKE_VEC_GETTERS(basic_vec, T, x, y, z, w)
		SEK_MAKE_VEC_GETTERS(basic_vec, T, r, g, b, a)

	private:
		template<std::size_t I = 0, typename U, typename... Us>
		SEK_FORCEINLINE void fill_impl(U &&x, Us &&...vals) noexcept requires (!detail::vec_instance<U>)
		{
			if constexpr (I < N)
			{
				operator[](I) = static_cast<T>(x);
				if constexpr (sizeof...(Us) != 0)
					fill_impl<I + 1>(std::forward<Us>(vals)...);
				else
					fill_impl<I + 1>(x);
			}
		}
		template<std::size_t I = 0, std::size_t J = 0, std::size_t M, typename OtherAbi, typename U, typename... Us>
		SEK_FORCEINLINE void fill_impl(const basic_vec<U, M, OtherAbi> &other, Us &&...vals) noexcept
		{
			if constexpr (J < M)
			{
				operator[](I) = static_cast<T>(other[J]);
				fill_impl<I + 1, J + 1>(other, std::forward<Us>(vals)...);
			}
			else if constexpr (sizeof...(Us) != 0)
				fill_impl<I>(std::forward<Us>(vals)...);
		}

		simd_type m_data;
	};

	template<typename... Us>
	basic_vec(Us &&...) -> basic_vec<detail::combine_value<Us...>, (detail::arg_extent_v<Us> + ...), detail::combine_abi<Us...>>;

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

	/** Alias for vector that uses implementation-defined ABI deduced from it's size, type and optional Abi hint. */
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
}
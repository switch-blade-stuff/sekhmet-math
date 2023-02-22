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
		struct is_vec<vec<T, N, A>> : std::true_type {};
		template<typename T>
		concept vec_instance = is_vec<std::remove_cvref_t<T>>::value;

		template<typename T>
		struct is_vec_mask : std::false_type {};
		template<typename T, std::size_t N, typename A>
		struct is_vec_mask<vec_mask<T, N, A>> : std::true_type {};
		template<typename T>
		concept vec_mask_instance = is_vec_mask<std::remove_cvref_t<T>>::value;

		template<typename... Ts>
		struct all_same : std::false_type {};
		template<typename T, std::same_as<T>... Ts>
		struct all_same<T, Ts...> : std::true_type {};

		template<typename T, typename... Ts>
		struct arg_value { using type = T; };
		template<typename T, std::size_t N, typename A, typename... Ts>
		struct arg_value<vec<T, N, A>, Ts...> { using type = T; };
		template<typename T, std::size_t N, typename A, typename... Ts>
		struct arg_value<vec_mask<T, N, A>, Ts...> { using type = bool; };
		template<typename... Ts>
		using arg_value_t = typename arg_value<std::remove_cvref_t<Ts>...>::type;

		template<typename T, typename... Ts>
		struct arg_abi { using type = abi::compatible<T>; };
		template<typename T, std::size_t N, typename A, typename... Ts>
		struct arg_abi<vec<T, N, A>, Ts...> { using type = A; };
		template<typename T, std::size_t N, typename A, typename... Ts>
		struct arg_abi<vec_mask<T, N, A>, Ts...> { using type = A; };
		template<typename... Ts>
		using arg_abi_t = typename arg_abi<std::remove_cvref_t<Ts>...>::type;

		template<typename>
		struct arg_extent : std::integral_constant<std::size_t, 1> {};
		template<typename T, std::size_t N, typename A>
		struct arg_extent<vec<T, N, A>> : std::integral_constant<std::size_t, N> {};
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
	class vec_mask
	{
		static_assert(N > 1, "Cannot create vector mask of 1 or 0 elements");

		template<typename U, std::size_t M, typename A>
		friend constexpr typename vec_mask<U, M, A>::simd_type &to_simd(vec_mask<U, M, A> &) noexcept;
		template<typename U, std::size_t M, typename A>
		friend constexpr const typename vec_mask<U, M, A>::simd_type &to_simd(const vec_mask<U, M, A> &) noexcept;

	public:
		using abi_type = dpm::simd_abi::deduce_t<T, N, Abi>;
		using data_t = dpm::simd_mask<T, abi_type>;

	public:
		SEK_FORCEINLINE vec_mask() noexcept : vec_mask(false) {}
		template<typename U>
		SEK_FORCEINLINE vec_mask(U x) noexcept requires std::is_convertible_v<U, bool> : m_data(x) {}

		template<typename... Us>
		vec_mask(Us ...vals) noexcept requires detail::compatible_args<bool, N, Us...> { fill_impl(std::move(vals)...); }
		template<typename... Us>
		vec_mask &fill(Us ...vals) noexcept requires detail::compatible_args<T, N, Us...>
		{
			fill_impl(std::move(vals)...);
			return *this;
		}

		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE vec_mask(const dpm::simd_mask<U, OtherAbi> &data) noexcept : m_data(data) {}
		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE vec_mask(const vec_mask<U, N, OtherAbi> &other) noexcept : vec_mask(to_simd(other)) {}

		[[nodiscard]] SEK_FORCEINLINE auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		[[nodiscard]] SEK_FORCEINLINE bool operator[](std::size_t i) const noexcept { return m_data[i]; }

		SEK_MAKE_VEC_GETTERS(vec_mask, bool, x, y, z, w)
		SEK_MAKE_VEC_GETTERS(vec_mask, bool, r, g, b, a)

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
		SEK_FORCEINLINE void fill_impl(const vec<U, M, OtherAbi> &other, Us &&...vals) noexcept
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
	vec_mask(Us &&...) -> vec_mask<detail::combine_value<Us...>, (detail::arg_extent_v<Us> + ...), detail::combine_abi<Us...>>;

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename vec_mask<T, N, Abi>::simd_type &to_simd(vec_mask<T, N, Abi> &x) noexcept { return x.m_data; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename vec_mask<T, N, Abi>::simd_type &to_simd(const vec_mask<T, N, Abi> &x) noexcept { return x.m_data; }
	template<std::size_t... Is, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline vec_mask<T, sizeof...(Is), abi::deduce_t<T, sizeof...(Is), Abi>> shuffle(vec_mask<T, N, Abi> x) noexcept
	{
		return {dpm::shuffle<Is...>(to_simd(x))};
	}

	template<typename T, std::size_t N, typename Abi>
	class vec
	{
		static_assert(N > 1, "Cannot create vector of 1 or 0 elements");

		template<typename U, std::size_t M, typename A>
		friend constexpr typename vec<U, M, A>::simd_type &to_simd(vec<U, M, A> &) noexcept;
		template<typename U, std::size_t M, typename A>
		friend constexpr const typename vec<U, M, A>::simd_type &to_simd(const vec<U, M, A> &) noexcept;

	public:
		using abi_type = dpm::simd_abi::deduce_t<T, N, Abi>;
		using simd_type = dpm::simd<T, abi_type>;

	public:
		SEK_FORCEINLINE vec() noexcept : vec(T{}) {}
		template<typename U>
		SEK_FORCEINLINE vec(U x) noexcept requires std::is_convertible_v<U, T> : m_data(x) {}

		template<typename... Us>
		vec(Us ...vals) noexcept requires detail::compatible_args<T, N, Us...> { fill_impl(std::move(vals)...); }
		template<typename... Us>
		vec &fill(Us ...vals) noexcept requires detail::compatible_args<T, N, Us...>
		{
			fill_impl(std::move(vals)...);
			return *this;
		}

		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE vec(const dpm::simd<U, OtherAbi> &data) noexcept : m_data(data) {}
		template<typename U, typename OtherAbi>
		SEK_FORCEINLINE vec(const vec<U, N, OtherAbi> &other) noexcept : vec(to_simd(other)) {}

		[[nodiscard]] SEK_FORCEINLINE auto &operator[](std::size_t i) noexcept { return m_data[i]; }
		[[nodiscard]] SEK_FORCEINLINE T operator[](std::size_t i) const noexcept { return m_data[i]; }

		SEK_MAKE_VEC_GETTERS(vec, T, x, y, z, w)
		SEK_MAKE_VEC_GETTERS(vec, T, r, g, b, a)

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
		SEK_FORCEINLINE void fill_impl(const vec<U, M, OtherAbi> &other, Us &&...vals) noexcept
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
	vec(Us &&...) -> vec<detail::combine_value<Us...>, (detail::arg_extent_v<Us> + ...), detail::combine_abi<Us...>>;

	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr typename vec<T, N, Abi>::simd_type &to_simd(vec<T, N, Abi> &x) noexcept { return x.m_data; }
	template<typename T, std::size_t N, typename Abi>
	[[nodiscard]] constexpr const typename vec<T, N, Abi>::simd_type &to_simd(const vec<T, N, Abi> &x) noexcept { return x.m_data; }
	template<std::size_t... Is, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline vec<T, sizeof...(Is), abi::deduce_t<T, sizeof...(Is), Abi>> shuffle(vec<T, N, Abi> x) noexcept
	{
		return {dpm::shuffle<Is...>(to_simd(x))};
	}

	template<typename T, typename Abi = abi::fixed_size<2>>
	using vec2 = vec<T, 2, Abi>;
	template<typename T, typename Abi = abi::fixed_size<3>>
	using vec3 = vec<T, 3, Abi>;
	template<typename T, typename Abi = abi::fixed_size<4>>
	using vec4 = vec<T, 4, Abi>;

	template<typename T, std::size_t N>
	using compat_vec = vec<T, N, abi::deduce_t<T, N, abi::compatible<T>>>;
	template<typename T>
	using compat_vec2 = compat_vec<T, 2>;
	template<typename T>
	using compat_vec3 = compat_vec<T, 3>;
	template<typename T>
	using compat_vec4 = compat_vec<T, 4>;

	template<typename T, std::size_t N>
	using packed_vec = vec<T, N, abi::packed_buffer<N>>;
	template<typename T>
	using packed_vec2 = packed_vec<T, 2>;
	template<typename T>
	using packed_vec3 = packed_vec<T, 3>;
	template<typename T>
	using packed_vec4 = packed_vec<T, 4>;
}
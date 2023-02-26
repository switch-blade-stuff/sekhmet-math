/*
 * Created by switchblade on 2023-02-22.
 */

#pragma once

#include "type_vec.hpp"
#include "mbase.hpp"

namespace sek
{
	namespace detail
	{
		template<typename T, typename MA, typename VA>
		[[nodiscard]] inline basic_mat<T, 4, 4, MA> impl_look_at_rh(const basic_vec<T, 3, VA> &org, const basic_vec<T, 3, VA> &dir, const basic_vec<T, 3, VA> &up) noexcept;
		template<typename T, typename MA, typename VA>
		[[nodiscard]] inline basic_mat<T, 4, 4, MA> impl_look_at_lh(const basic_vec<T, 3, VA> &org, const basic_vec<T, 3, VA> &dir, const basic_vec<T, 3, VA> &up) noexcept;

		template<typename>
		struct is_mat : std::false_type {};
		template<typename T, std::size_t C, std::size_t R, typename A>
		struct is_mat<basic_mat<T, C, R, A>> : std::true_type {};

		template<typename T, std::size_t Rows>
		concept compatible_col = detail::has_tuple_size<T> && !is_mat<T>::value && std::tuple_size_v<T> <= Rows;
		template<std::size_t N>
		concept is_square = []()
		{
			for (std::size_t i = 0; i < N; ++i)
				if (i * i == N) return true;
			return false;
		}();
	}

	/** @brief Structure used to define a mathematical matrix.
	 * @tparam T Value type stored by the matrix.
	 * @tparam NCols Number of columns of the matrix.
	 * @tparam NRows Number of rows of the matrix.
	 * @tparam Abi ABI tag used for the underlying storage of the matrix.
	 * @note \a NCols and \a NRows must be at least 2 and the \a Abi tag size must match \a NRows. */
	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	class basic_mat
	{
		static_assert(dpm::simd_size<T, Abi>::value == NRows, "Abi size must match matrix row count");
		static_assert(NCols > 1, "Cannot create matrix of less than 2 columns");
		static_assert(NRows > 1, "Cannot create matrix of less than 2 rows");

	public:
		using col_type = basic_vec<T, NRows, Abi>;
		using value_type = typename col_type::value_type;
		using row_type = basic_vec<T, NCols, math_abi::deduce_t<T, NCols, Abi>>;

		/** Returns identity matrix. Equivalent to `basic_mat{1}`.
		 * @note This function is defined only for matrices where `cols() == rows()`. */
		[[nodiscard]] inline static basic_mat identity() noexcept requires (NCols == NRows);

		/** Creates a look-at transform matrix used to rotate an origin vector \a org in the look direction \a dir using the up direction \a up with default handedness.
	 	 * @param org Origin vector to be rotated.
	 	 * @param dir Direction vector to rotate the origin towards.
	 	 * @param up Normalized up vector.
		 * @return Resulting transform matrix.
		 * @note This function is defined only for 4x4 matrices. */
		template<typename A>
		[[nodiscard]] static SEK_FORCEINLINE basic_mat look_at(const basic_vec<T, 3, A> &org, const basic_vec<T, 3, A> &dir, const basic_vec<T, 3, A> &up = basic_vec<T, 3, A>::up()) noexcept requires (NCols == NRows && NCols == 4)
		{
#ifndef SEK_FORCE_LEFT_HANDED
			return look_at_rh(org, dir, up);
#else
			return look_at_lh(org, dir, up);
#endif
		}
		/** Creates a right-handed look-at transform matrix used to rotate an origin vector \a org in the look direction \a dir using the up direction \a up.
		 * @param org Origin vector to be rotated.
		 * @param dir Direction vector to rotate the origin towards.
		 * @param up Normalized up vector.
		 * @return Resulting transform matrix.
		 * @note This function is defined only for 4x4 matrices. */
		template<typename A>
		[[nodiscard]] static basic_mat look_at_rh(const basic_vec<T, 3, A> &org, const basic_vec<T, 3, A> &dir, const basic_vec<T, 3, A> &up = basic_vec<T, 3, A>::up()) noexcept requires (NCols == NRows && NCols == 4)
		{
			return detail::impl_look_at_rh<T, Abi, A>(org, dir, up);
		}
		/** Creates left-handed look-at transform matrix used to rotate an origin vector \a org in the look direction \a dir using the up direction \a up.
		 * @param org Origin vector to be rotated.
		 * @param dir Direction vector to rotate the origin towards.
		 * @param up Normalized up vector.
		 * @return Resulting transform matrix.
		 * @note This function is defined only for 4x4 matrices. */
		template<typename A>
		[[nodiscard]] static basic_mat look_at_lh(const basic_vec<T, 3, A> &org, const basic_vec<T, 3, A> &dir, const basic_vec<T, 3, A> &up = basic_vec<T, 3, A>::up()) noexcept requires (NCols == NRows && NCols == 4)
		{
			return detail::impl_look_at_lh<T, Abi, A>(org, dir, up);
		}

	private:
		static inline void assert_cols(std::size_t i) { if (i >= NCols) [[unlikely]] throw std::range_error("Column index out of range"); }
		static inline void assert_rows(std::size_t i) { if (i >= NRows) [[unlikely]] throw std::range_error("Row index out of range"); }

	public:
		/** Initializes a null (zero) matrix. */
		constexpr basic_mat() noexcept = default;

		/** Initializes the matrix where elements along the main diagonal are set to `static_cast<value_type>(x)`, and the rest are zero. */
		template<typename U>
		basic_mat(U &&x) noexcept requires std::is_convertible_v<U, value_type> { fill_diag(std::forward<U>(x)); }
		/** Initializes columns of the matrix from \a args tuples. Remaining elements are initialized to have ones (`1`) along the main diagonal and zeros elsewhere. */
		template<typename... Args>
		basic_mat(Args &&...args) noexcept requires ((detail::compatible_col<std::remove_cvref_t<Args>, NRows> && ...)) { fill_cols(std::forward<Args>(args)...); }
		/** Initializes the matrix from columns of another matrix. Remaining elements are initialized to have ones (`1`) along the main diagonal and zeros elsewhere. */
		template<typename U, std::size_t OtherCols, std::size_t OtherRows, typename A>
		basic_mat(const basic_mat<U, OtherCols, OtherRows, A> &other) noexcept requires std::is_convertible_v<U, value_type>
		{
			for (std::size_t i = 0; i < NCols; ++i)
			{
				if (i < OtherCols) m_data[i] = col_type{other[i]};
				for (std::size_t j = OtherRows; j < NRows; ++j)
					m_data[i][j] = i == j ? T{1} : T{0};
			}
		}

		/** Returns the number of columns in the matrix. */
		[[nodiscard]] constexpr std::size_t cols() const noexcept { return NCols; }
		/** Returns the number of rows in the matrix. */
		[[nodiscard]] constexpr std::size_t rows() const noexcept { return NRows; }

		/** Returns the `i`th row of the matrix.
		 * @param i Index of the requested row.
		 * @throw std::range_error In case \a i exceeds `rows()`.
		 * @note Since matrices are column-major, reading a row will require element-wise access of all columns. */
		[[nodiscard]] row_type row(std::size_t i) const
		{
			assert_rows(i);
			row_type result = {};
			for (std::size_t j = 0; j < NCols; ++j)
				result[j] = m_data[j][i];
			return result;
		}

		/** Returns reference to the `i`th column of the matrix.
		 * @param i Index of the requested column.
		 * @throw std::range_error In case \a i exceeds `cols()`. */
		[[nodiscard]] col_type &col(std::size_t i)
		{
			assert_cols(i);
			return m_data[i];
		}
		/** @copydoc col */
		[[nodiscard]] const col_type &col(std::size_t i) const
		{
			assert_cols(i);
			return m_data[i];
		}

		/** Returns reference to the `j`th element of the `i`th column of the matrix.
		 * @param i Column index of the requested element.
		 * @param j Row index of the requested element.
		 * @throw std::range_error In case \a i exceeds `cols()` or \a j exceeds `rows()`. */
		[[nodiscard]] auto &at(std::size_t i, std::size_t j)
		{
			assert_cols(i);
			assert_rows(j);
			return m_data[i][j];
		}
		/** Returns copy of the element of the `j`th element of the `i`th column of the matrix.
		 * @param i Column index of the requested element.
		 * @param j Row index of the requested element.
		 * @throw std::range_error In case \a i exceeds `cols()` or \a j exceeds `rows()`. */
		[[nodiscard]] value_type at(std::size_t i, std::size_t j) const
		{
			assert_cols(i);
			assert_rows(j);
			return m_data[i][j];
		}

		/** Returns reference to the `i`th column of the matrix.
		 * @param i Index of the requested column. */
		[[nodiscard]] constexpr col_type &operator[](std::size_t i) noexcept
		{
			SEK_ASSUME(i < NCols);
			return m_data[i];
		}
		/** @copydoc operator[] */
		[[nodiscard]] constexpr const col_type &operator[](std::size_t i) const noexcept
		{
			SEK_ASSUME(i < NCols);
			return m_data[i];
		}

		/* Support multi-dimensional subscript operator for C++23 and later */
#if SEK_CXX_VERSION > 202002L
		/** Returns reference to the element of the `j`th element of the `i`th column of the matrix.
		 * @param i Column index of the requested element.
		 * @param j Row index of the requested element. */
		[[nodiscard]] constexpr auto &operator[](std::size_t i, std::size_t j) noexcept
		{
			SEK_ASSUME(i < NCols);
			SEK_ASSUME(j < NRows);
			return m_data[i][j];
		}
		/** Returns copy of the element of the `j`th element of the `i`th column of the matrix.
		 * @param i Column index of the requested element.
		 * @param j Row index of the requested element. */
		[[nodiscard]] constexpr value_type operator[](std::size_t i, std::size_t j) const noexcept
		{
			SEK_ASSUME(i < NCols);
			SEK_ASSUME(j < NRows);
			return m_data[i][j];
		}
#endif

	private:
		template<std::size_t I = 0, typename U>
		inline void fill_diag(U &&value) noexcept
		{
			if constexpr (I < NCols && I < NRows)
			{
				m_data[I][I] = static_cast<value_type>(value);
				fill_diag<I + 1>(std::forward<U>(value));
			}
		}
		template<std::size_t I = 0, std::size_t J = 0, typename U, typename... Args>
		inline void fill_cols(U &&value, Args &&...args) noexcept
		{
			if constexpr (J < NRows)
			{
				using std::get;

				/* If the row index is within the tuple argument, use tuple value. Otherwise, initialize the main diagonal. */
				if constexpr (J < std::tuple_size_v<std::remove_cvref_t<U>>)
					m_data[I][J] = static_cast<value_type>(get<J>(value));
				else if constexpr (J == I)
					m_data[I][J] = static_cast<value_type>(1);
				fill_cols<I, J + 1>(std::forward<U>(value), std::forward<Args>(args)...);
			}
			else if constexpr (sizeof...(Args) != 0) /* Move onto the next column. */
				fill_cols<I + 1>(std::forward<Args>(args)...);
			else
				fill_diag<I + 1>(1);
		}

		col_type m_data[NCols] = {};
	};

	template<typename T, std::size_t C, std::size_t R, typename A>
	basic_mat<T, C, R, A> basic_mat<T, C, R, A>::identity() noexcept requires (C == R) { return basic_mat{1}; }

	/** Gets the `I`th column of the matrix. */
	template<std::size_t I, typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	[[nodiscard]] constexpr typename basic_mat<T, NCols, NRows, Abi>::col_type &get(basic_mat<T, NCols, NRows, Abi> &x) noexcept requires (I < NCols) { return x[I]; }
	/** @copydoc get */
	template<std::size_t I, typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	[[nodiscard]] constexpr const typename basic_mat<T, NCols, NRows, Abi>::col_type &get(const basic_mat<T, NCols, NRows, Abi> &x) noexcept requires (I < NCols) { return x[I]; }
	/** Gets the `J`th element of the `I`th column of the matrix. */
	template<std::size_t I, std::size_t J, typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	[[nodiscard]] inline auto &get(basic_mat<T, NCols, NRows, Abi> &x) noexcept requires (I < NCols && J < NRows) { return x[I][J]; }
	/** @copydoc get */
	template<std::size_t I, std::size_t J, typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	[[nodiscard]] inline auto get(const basic_mat<T, NCols, NRows, Abi> &x) noexcept requires (I < NCols && J < NRows) { return x[I][J]; }

#pragma region "basic_mat aliases"
	/** Alias for matrix that uses implementation-defined ABI deduced from it's size, type and optional ABI hint. */
	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi = math_abi::fixed_size<NRows>>
	using mat = basic_mat<T, NCols, NRows, math_abi::deduce_t<T, NRows, Abi>>;
	/** Alias for deduced-ABI matrix of 2 columns by 2 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<2>>
	using mat2x2 = mat<T, 2, 2, Abi>;
	/** Alias for deduced-ABI matrix of 2 columns by 3 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<3>>
	using mat2x3 = mat<T, 2, 3, Abi>;
	/** Alias for deduced-ABI matrix of 2 columns by 4 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<4>>
	using mat2x4 = mat<T, 2, 4, Abi>;
	/** Alias for deduced-ABI matrix of 3 columns by 2 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<2>>
	using mat3x2 = mat<T, 3, 2, Abi>;
	/** Alias for deduced-ABI matrix of 3 columns by 3 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<3>>
	using mat3x3 = mat<T, 3, 3, Abi>;
	/** Alias for deduced-ABI matrix of 3 columns by 4 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<4>>
	using mat3x4 = mat<T, 3, 4, Abi>;
	/** Alias for deduced-ABI matrix of 4 columns by 2 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<2>>
	using mat4x2 = mat<T, 4, 2, Abi>;
	/** Alias for deduced-ABI matrix of 4 columns by 3 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<3>>
	using mat4x3 = mat<T, 4, 3, Abi>;
	/** Alias for deduced-ABI matrix of 4 columns by 4 rows. */
	template<typename T, typename Abi = math_abi::fixed_size<4>>
	using mat4x4 = mat<T, 4, 4, Abi>;

	/** Alias for matrix that uses compatible (implementation-defined) ABI. */
	template<typename T, std::size_t NCols, std::size_t NRows>
	using compat_mat = basic_mat<T, NCols, NRows, math_abi::deduce_t<T, NRows, math_abi::compatible < T>>>;
	/** Alias for compatible-ABI matrix of 2 columns by 2 rows. */
	template<typename T>
	using compat_mat2x2 = compat_mat<T, 2, 2>;
	/** Alias for compatible-ABI matrix of 2 columns by 3 rows. */
	template<typename T>
	using compat_mat2x3 = compat_mat<T, 2, 3>;
	/** Alias for compatible-ABI matrix of 2 columns by 4 rows. */
	template<typename T>
	using compat_mat2x4 = compat_mat<T, 2, 4>;
	/** Alias for compatible-ABI matrix of 3 columns by 2 rows. */
	template<typename T>
	using compat_mat3x2 = compat_mat<T, 3, 2>;
	/** Alias for compatible-ABI matrix of 3 columns by 3 rows. */
	template<typename T>
	using compat_mat3x3 = compat_mat<T, 3, 3>;
	/** Alias for compatible-ABI matrix of 3 columns by 4 rows. */
	template<typename T>
	using compat_mat3x4 = compat_mat<T, 3, 4>;
	/** Alias for compatible-ABI matrix of 4 columns by 2 rows. */
	template<typename T>
	using compat_mat4x2 = compat_mat<T, 4, 2>;
	/** Alias for compatible-ABI matrix of 4 columns by 3 rows. */
	template<typename T>
	using compat_mat4x3 = compat_mat<T, 4, 3>;
	/** Alias for compatible-ABI matrix of 4 columns by 4 rows. */
	template<typename T>
	using compat_mat4x4 = compat_mat<T, 4, 4>;

	/** Alias for matrix that uses packed (non-vectorized) ABI. */
	template<typename T, std::size_t NCols, std::size_t NRows>
	using packed_mat = basic_mat<T, NCols, NRows, math_abi::packed_buffer<NRows>>;
	/** Alias for packed-ABI matrix of 2 columns by 2 rows. */
	template<typename T>
	using packed_mat2x2 = packed_mat<T, 2, 2>;
	/** Alias for packed-ABI matrix of 2 columns by 3 rows. */
	template<typename T>
	using packed_mat2x3 = packed_mat<T, 2, 3>;
	/** Alias for packed-ABI matrix of 2 columns by 4 rows. */
	template<typename T>
	using packed_mat2x4 = packed_mat<T, 2, 4>;
	/** Alias for packed-ABI matrix of 3 columns by 2 rows. */
	template<typename T>
	using packed_mat3x2 = packed_mat<T, 3, 2>;
	/** Alias for packed-ABI matrix of 3 columns by 3 rows. */
	template<typename T>
	using packed_mat3x3 = packed_mat<T, 3, 3>;
	/** Alias for packed-ABI matrix of 3 columns by 4 rows. */
	template<typename T>
	using packed_mat3x4 = packed_mat<T, 3, 4>;
	/** Alias for packed-ABI matrix of 4 columns by 2 rows. */
	template<typename T>
	using packed_mat4x2 = packed_mat<T, 4, 2>;
	/** Alias for packed-ABI matrix of 4 columns by 3 rows. */
	template<typename T>
	using packed_mat4x3 = packed_mat<T, 4, 3>;
	/** Alias for packed-ABI matrix of 4 columns by 4 rows. */
	template<typename T>
	using packed_mat4x4 = packed_mat<T, 4, 4>;
#pragma endregion

#pragma region "basic_mat operators"
	template<typename T, std::size_t CR, std::size_t NR, typename AM, typename AV>
	[[nodiscard]] inline basic_vec<T, NR, math_abi::deduce_t<T, NR, AM>> operator*(const basic_mat<T, CR, NR, AM> &a, const basic_vec<T, CR, AV> &b) noexcept
	{
		basic_vec<T, NR, math_abi::deduce_t<T, NR, AM>> result = a[0] * b[0];
		for (std::size_t i = 1; i < CR; ++i) result = fmadd(a[i], {b[i]}, result);
		return result;
	}
	template<typename T, std::size_t CR, std::size_t R0, std::size_t C1, typename A0, typename A1>
	[[nodiscard]] inline basic_mat<T, C1, R0, math_abi::deduce_t<T, R0, A0, A1>> operator*(const basic_mat<T, CR, R0, A0> &a, const basic_mat<T, C1, CR, A1> &b) noexcept
	{
		basic_mat<T, C1, R0, math_abi::deduce_t<T, R0, A0, A1>> result;
		for (std::size_t i = 0; i < C1; ++i)
		{
			auto col = a[0] * b[i][0];
			for (std::size_t j = 1; j < CR; ++j)
				col = fmadd(a[j], {b[i][j]}, col);
			result[i] = col;
		}
		return result;
	}

	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	[[nodiscard]] inline bool operator==(const basic_mat<T, NCols, NRows, Abi> &a, const basic_mat<T, NCols, NRows, Abi> &b) noexcept
	{
		auto cmp = a[0] == b[0];
		for (std::size_t i = 1; i < NCols; ++i)
			cmp &= a[i] == b[i];
		return all_of(cmp);
	}
	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	[[nodiscard]] inline bool operator!=(const basic_mat<T, NCols, NRows, Abi> &a, const basic_mat<T, NCols, NRows, Abi> &b) noexcept
	{
		auto cmp = a[0] != b[0];
		for (std::size_t i = 1; i < NCols; ++i)
			cmp &= a[i] != b[i];
		return all_of(cmp);
	}
#pragma endregion

#pragma region "basic_mat algorithms"
	/** Calculates the outer product matrix of vectors \a c and \a r. */
	template<typename T, std::size_t NCols, std::size_t NRows, typename AC, typename AR>
	[[nodiscard]] inline mat<T, NCols, NRows, AC> outer_prod(const basic_vec<T, NRows, AC> &c, const basic_vec<T, NCols, AR> &r) noexcept
	{
		mat<T, NCols, NRows, AC> result;
		for (std::size_t i = 0; i < NCols; ++i)
			result[i] = c * r[i];
		return result;
	}

	/** Calculates the transpose matrix of \a x. */
	template<typename T, std::size_t NRows, std::size_t NCols, typename A>
	[[nodiscard]] inline mat<T, NRows, NCols, A> transpose(const basic_mat<T, NCols, NRows, A> &x) noexcept
	{
		basic_mat<T, NRows, NCols, math_abi::deduce_t<T, NCols, A>> result;
		for (std::size_t i = 0; i < NRows; ++i) result[i] = x.row(i);
		return result;
	}

	/** Calculates the determinant of a 2x2 matrix \a x. */
	template<typename T, typename A>
	[[nodiscard]] inline T determinant(const basic_mat<T, 2, 2, A> &x) noexcept
	{
		return detail::fmsub(x[0][0], x[1][1], x[1][0] * x[0][1]);
	}
	/** Calculates the determinant of a 3x3 matrix \a x. */
	template<typename T, typename A>
	[[nodiscard]] inline T determinant(const basic_mat<T, 3, 3, A> &x) noexcept
	{
		const auto a = detail::fmsub(x[1][1], x[2][2], x[2][1] * x[1][2]);
		const auto b = detail::fmsub(x[0][1], x[2][2], x[2][1] * x[0][2]);
		const auto c = detail::fmsub(x[0][1], x[1][2], x[1][1] * x[0][2]);
		return detail::fmsub(a, x[0][0], detail::fmadd(b, x[1][0], c * x[2][0]));
	}
	/** Calculates the determinant of a 4x4 matrix \a x. */
	template<typename T, typename A>
	[[nodiscard]] inline T determinant(const basic_mat<T, 4, 4, A> &x) noexcept
	{
		const auto f0 = detail::fmsub(x[2][2], x[3][3], x[3][2] * x[2][3]);
		const auto f1 = detail::fmsub(x[2][1], x[3][3], x[3][1] * x[2][3]);
		const auto f2 = detail::fmsub(x[2][1], x[3][2], x[3][1] * x[2][2]);
		const auto f3 = detail::fmsub(x[2][0], x[3][3], x[3][0] * x[2][3]);
		const auto f4 = detail::fmsub(x[2][0], x[3][2], x[3][0] * x[2][2]);
		const auto f5 = detail::fmsub(x[2][0], x[3][1], x[3][0] * x[2][1]);
		const auto c0 = detail::fmsub(x[1][1], f0, detail::fmadd(x[1][2], f1, x[1][3] * f2));
		const auto c1 = detail::fmsub(x[1][0], f0, detail::fmadd(x[1][2], f3, x[1][3] * f4));
		const auto c2 = detail::fmsub(x[1][0], f1, detail::fmadd(x[1][1], f3, x[1][3] * f5));
		const auto c3 = detail::fmsub(x[1][0], f2, detail::fmadd(x[1][1], f4, x[1][2] * f5));
		return detail::fmsub(x[0][0], c0, detail::fmadd(x[0][1], c1, detail::fmsub(x[0][2], c2, x[0][3] * c3)));
	}
#pragma endregion
}
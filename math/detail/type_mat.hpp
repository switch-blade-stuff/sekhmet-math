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
		template<std::size_t N>
		concept is_square = []()
		{
			for (std::size_t i = 0; i < N; ++i)
				if (i * i == N) return true;
			return false;
		}();
	}

	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	class basic_mat
	{
		static_assert(dpm::simd_size<T, Abi>::value == NRows, "Abi size must match matrix row count");
		static_assert(NCols > 1, "Cannot create matrix of less than 2 columns");
		static_assert(NRows > 1, "Cannot create matrix of less than 2 rows");

	public:
		using col_type = basic_vec<T, NRows, Abi>;
		using value_type = typename col_type::value_type;
		using row_type = basic_vec<T, NCols, abi::deduce_t<T, NCols, Abi>>;

	private:
		static inline void assert_cols(std::size_t i) { if (i >= NCols) [[unlikely]] throw std::range_error("Column index out of range"); }
		static inline void assert_rows(std::size_t i) { if (i >= NRows) [[unlikely]] throw std::range_error("Row index out of range"); }

	public:
		/** Initializes a null (zero) matrix. */
		constexpr basic_mat() noexcept = default;

		/** Initializes the matrix where elements along the main diagonal are set to `static_cast<value_type>(x)`, and the rest are zero. */
		template<typename U>
		SEK_FORCEINLINE basic_mat(U &&x) noexcept requires std::is_convertible_v<U, value_type> { fill_diag(std::forward<U>(x)); }
		/** Initializes columns of the matrix from \a args tuples. Remaining columns are initialized to have ones (`1`) along the main diagonal and zeros elsewhere. */
		template<detail::has_tuple_size... Args>
		SEK_FORCEINLINE basic_mat(Args &&...args) noexcept requires (sizeof...(Args) <= NCols && ((std::tuple_size_v<std::remove_cvref_t<Args>> <= NRows) && ...)) { fill_cols(std::forward<Args>(args)...); }

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
		DPM_FORCEINLINE void fill_diag(U &&value) noexcept
		{
			if constexpr (I < NCols && I < NRows)
			{
				m_data[I][I] = static_cast<value_type>(value);
				fill_diag<I + 1>(std::forward<U>(value));
			}
		}
		template<std::size_t I = 0, std::size_t J = 0, typename U, typename... Args>
		DPM_FORCEINLINE void fill_cols(U &&value, Args &&...args) noexcept
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
	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi = abi::fixed_size<NRows>>
	using mat = basic_mat<T, NCols, NRows, abi::deduce_t<T, NRows, Abi>>;
	/** Alias for deduced-ABI matrix of 2 columns by 2 rows. */
	template<typename T, typename Abi = abi::fixed_size<2>>
	using mat2x2 = mat<T, 2, 2, Abi>;
	/** Alias for deduced-ABI matrix of 2 columns by 3 rows. */
	template<typename T, typename Abi = abi::fixed_size<3>>
	using mat2x3 = mat<T, 2, 3, Abi>;
	/** Alias for deduced-ABI matrix of 2 columns by 4 rows. */
	template<typename T, typename Abi = abi::fixed_size<4>>
	using mat2x4 = mat<T, 2, 4, Abi>;
	/** Alias for deduced-ABI matrix of 3 columns by 2 rows. */
	template<typename T, typename Abi = abi::fixed_size<2>>
	using mat3x2 = mat<T, 3, 2, Abi>;
	/** Alias for deduced-ABI matrix of 3 columns by 3 rows. */
	template<typename T, typename Abi = abi::fixed_size<3>>
	using mat3x3 = mat<T, 3, 3, Abi>;
	/** Alias for deduced-ABI matrix of 3 columns by 4 rows. */
	template<typename T, typename Abi = abi::fixed_size<4>>
	using mat3x4 = mat<T, 3, 4, Abi>;
	/** Alias for deduced-ABI matrix of 4 columns by 2 rows. */
	template<typename T, typename Abi = abi::fixed_size<2>>
	using mat4x2 = mat<T, 4, 2, Abi>;
	/** Alias for deduced-ABI matrix of 4 columns by 3 rows. */
	template<typename T, typename Abi = abi::fixed_size<3>>
	using mat4x3 = mat<T, 4, 3, Abi>;
	/** Alias for deduced-ABI matrix of 4 columns by 4 rows. */
	template<typename T, typename Abi = abi::fixed_size<4>>
	using mat4x4 = mat<T, 4, 4, Abi>;

	/** Alias for matrix that uses compatible (implementation-defined) ABI. */
	template<typename T, std::size_t NCols, std::size_t NRows>
	using compat_mat = basic_mat<T, NCols, NRows, abi::deduce_t<T, NRows, abi::compatible<T>>>;
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
	using packed_mat = basic_mat<T, NCols, NRows, abi::packed_buffer<NRows>>;
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
	[[nodiscard]] inline basic_vec<T, NR, abi::deduce_t<T, NR, AM, AV>> operator*(const basic_mat<T, CR, NR, AM> &a, const basic_vec<T, CR, AV> &b) noexcept
	{
		basic_vec<T, NR, abi::deduce_t<T, NR, AM, AV>> result = a[0] * b[0];
		for (std::size_t i = 1; i < CR; ++i) result = fmadd(a[i], {b[i]}, result);
		return result;
	}
	template<typename T, std::size_t CR, std::size_t R0, std::size_t C1, typename A0, typename A1>
	[[nodiscard]] inline basic_mat<T, C1, R0, abi::deduce_t<T, R0, A0, A1>> operator*(const basic_mat<T, CR, R0, A0> &a, const basic_mat<T, C1, CR, A1> &b) noexcept
	{
		basic_mat<T, C1, R0, abi::deduce_t<T, R0, A0, A1>> result;
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
	[[nodiscard]] SEK_FORCEINLINE bool operator==(const basic_mat<T, NCols, NRows, Abi> &a, const basic_mat<T, NCols, NRows, Abi> &b) noexcept
	{
		auto cmp = a[0] == b[0];
		for (std::size_t i = 1; i < NCols; ++i)
			cmp &= a[i] == b[i];
		return all_of(cmp);
	}
	template<typename T, std::size_t NCols, std::size_t NRows, typename Abi>
	[[nodiscard]] SEK_FORCEINLINE bool operator!=(const basic_mat<T, NCols, NRows, Abi> &a, const basic_mat<T, NCols, NRows, Abi> &b) noexcept
	{
		auto cmp = a[0] == b[0];
		for (std::size_t i = 1; i < NCols; ++i)
			cmp |= a[i] == b[i];
		return none_of(cmp);
	}
#pragma endregion
}
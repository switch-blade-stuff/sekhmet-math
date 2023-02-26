/*
 * Created by switchblade on 2023-02-21.
 */

#pragma once

#include "fwd.hpp"

namespace sek
{
	template<std::size_t... Is, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec<T, sizeof...(Is), math_abi::deduce_t<T, sizeof...(Is), Abi>> shuffle(const basic_vec<T, N, Abi> &x) noexcept;
	template<std::size_t... Is, typename T, std::size_t N, typename Abi>
	[[nodiscard]] inline basic_vec_mask<T, sizeof...(Is), math_abi::deduce_t<T, sizeof...(Is), Abi>> shuffle(const basic_vec_mask<T, N, Abi> &x) noexcept;
}

/* Vector API exposes convenience element shuffles (xxx, xxy, xxz, etc.). These macros are used to generate all possible
 * permutations of element shuffle functions for 2-, 3-, end 4-element vectors. Ugly but beats writing them manually. */

#define SEK_MAKE_VEC_GETTERS2(type, M, x, y, ix, iy)                                                            \
    template<typename NewAbi = Abi>                                                                             \
    [[nodiscard]] type<T, 2, math_abi::deduce_t<T, 2, NewAbi>> x##y() const noexcept requires (N >= M)          \
    {                                                                                                           \
        return {shuffle<ix, iy>(*this)};                                                                        \
    }
#define SEK_MAKE_VEC_GETTERS3(type, M, x, y, z, ix, iy, iz)                                                     \
    template<typename NewAbi = Abi>                                                                             \
    [[nodiscard]] type<T, 3, math_abi::deduce_t<T, 3, NewAbi>> x##y##z() const noexcept requires (N >= M)       \
    {                                                                                                           \
        return {shuffle<ix, iy, iz>(*this)};                                                                    \
    }
#define SEK_MAKE_VEC_GETTERS4(type, M, x, y, z, w, ix, iy, iz, iw)                                              \
    template<typename NewAbi = Abi>                                                                             \
    [[nodiscard]] type<T, 4, math_abi::deduce_t<T, 4, NewAbi>> x##y##z##w() const noexcept requires (N >= M)    \
    {                                                                                                           \
        return {shuffle<ix, iy, iz, iw>(*this)};                                                                \
    }

#define SEK_MAKE_VEC_GETTERS(type, U, x, y, z, w)                                                   \
    [[nodiscard]] auto & x() noexcept { return operator[](0); }                                     \
    [[nodiscard]] auto & y() noexcept { return operator[](1); }                                     \
    [[nodiscard]] auto & z() noexcept requires (N > 2) { return operator[](2); }                    \
    [[nodiscard]] auto & w() noexcept requires (N > 3) { return operator[](3); }                    \
    [[nodiscard]] U x() const noexcept { return operator[](0); }                                    \
    [[nodiscard]] U y() const noexcept { return operator[](1); }                                    \
    [[nodiscard]] U z() const noexcept requires (N > 2) { return operator[](2); }                   \
    [[nodiscard]] U w() const noexcept requires (N > 3) { return operator[](3); }                   \
                                                                                                    \
    SEK_MAKE_VEC_GETTERS2(type, 2, x, x, 0, 0)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 2, x, y, 0, 1)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 3, x, z, 0, 2)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 4, x, w, 0, 3)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 2, y, x, 1, 0)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 2, y, y, 1, 1)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 3, y, z, 1, 2)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 4, y, w, 1, 3)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 3, z, x, 2, 0)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 3, z, y, 2, 1)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 3, z, z, 2, 2)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 4, z, w, 2, 3)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 4, w, x, 3, 0)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 4, w, y, 3, 1)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 4, w, z, 3, 2)                                                      \
    SEK_MAKE_VEC_GETTERS2(type, 4, w, w, 3, 3)                                                      \
    SEK_MAKE_VEC_GETTERS3(type, 2, x, x, x, 0, 0, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 2, x, x, y, 0, 0, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, x, x, z, 0, 0, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, x, x, w, 0, 0, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 2, x, y, x, 0, 1, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 2, x, y, y, 0, 1, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, x, y, z, 0, 1, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, x, y, w, 0, 1, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, x, z, x, 0, 2, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, x, z, y, 0, 2, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, x, z, z, 0, 2, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, x, z, w, 0, 2, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, x, w, x, 0, 3, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, x, w, y, 0, 3, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, x, w, z, 0, 3, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, x, w, w, 0, 3, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 2, y, x, x, 1, 0, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 2, y, x, y, 1, 0, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, y, x, z, 1, 0, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, y, x, w, 1, 0, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 2, y, y, x, 1, 1, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 2, y, y, y, 1, 1, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, y, y, z, 1, 1, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, y, y, w, 1, 1, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, y, z, x, 1, 2, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, y, z, y, 1, 2, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, y, z, z, 1, 2, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, y, z, w, 1, 2, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, y, w, x, 1, 3, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, y, w, y, 1, 3, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, y, w, z, 1, 3, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, y, w, w, 1, 3, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, x, x, 2, 0, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, x, y, 2, 0, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, x, z, 2, 0, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, z, x, w, 2, 0, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, y, x, 2, 1, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, y, y, 2, 1, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, y, z, 2, 1, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, z, y, w, 2, 1, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, z, x, 2, 2, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, z, y, 2, 2, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 3, z, z, z, 2, 2, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, z, z, w, 2, 2, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, z, w, x, 2, 3, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, z, w, y, 2, 3, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, z, w, z, 2, 3, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, z, w, w, 2, 3, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, x, x, 3, 0, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, x, y, 3, 0, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, x, z, 3, 0, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, x, w, 3, 0, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, y, x, 3, 1, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, y, y, 3, 1, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, y, z, 3, 1, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, y, w, 3, 1, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, z, x, 3, 2, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, z, y, 3, 2, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, z, z, 3, 2, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, z, w, 3, 2, 3)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, w, x, 3, 3, 0)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, w, y, 3, 3, 1)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, w, z, 3, 3, 2)                                                \
    SEK_MAKE_VEC_GETTERS3(type, 4, w, w, w, 3, 3, 3)                                                \
    SEK_MAKE_VEC_GETTERS4(type, 2, x, x, x, x, 0, 0, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, x, x, x, y, 0, 0, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, x, x, z, 0, 0, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, x, x, w, 0, 0, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, x, x, y, x, 0, 0, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, x, x, y, y, 0, 0, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, x, y, z, 0, 0, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, x, y, w, 0, 0, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, x, z, x, 0, 0, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, x, z, y, 0, 0, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, x, z, z, 0, 0, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, x, z, w, 0, 0, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, x, w, x, 0, 0, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, x, w, y, 0, 0, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, x, w, z, 0, 0, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, x, w, w, 0, 0, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, x, y, x, x, 0, 1, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, x, y, x, y, 0, 1, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, y, x, z, 0, 1, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, y, x, w, 0, 1, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, x, y, y, x, 0, 1, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, x, y, y, y, 0, 1, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, y, y, z, 0, 1, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, y, y, w, 0, 1, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, y, z, x, 0, 1, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, y, z, y, 0, 1, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, y, z, z, 0, 1, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, y, z, w, 0, 1, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, y, w, x, 0, 1, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, y, w, y, 0, 1, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, y, w, z, 0, 1, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, y, w, w, 0, 1, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, x, x, 0, 2, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, x, y, 0, 2, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, x, z, 0, 2, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, z, x, w, 0, 2, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, y, x, 0, 2, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, y, y, 0, 2, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, y, z, 0, 2, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, z, y, w, 0, 2, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, z, x, 0, 2, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, z, y, 0, 2, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, x, z, z, z, 0, 2, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, z, z, w, 0, 2, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, z, w, x, 0, 2, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, z, w, y, 0, 2, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, z, w, z, 0, 2, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, z, w, w, 0, 2, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, x, x, 0, 3, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, x, y, 0, 3, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, x, z, 0, 3, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, x, w, 0, 3, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, y, x, 0, 3, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, y, y, 0, 3, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, y, z, 0, 3, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, y, w, 0, 3, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, z, x, 0, 3, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, z, y, 0, 3, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, z, z, 0, 3, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, z, w, 0, 3, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, w, x, 0, 3, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, w, y, 0, 3, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, w, z, 0, 3, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, x, w, w, w, 0, 3, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, y, x, x, x, 1, 0, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, y, x, x, y, 1, 0, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, x, x, z, 1, 0, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, x, x, w, 1, 0, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, y, x, y, x, 1, 0, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, y, x, y, y, 1, 0, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, x, y, z, 1, 0, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, x, y, w, 1, 0, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, x, z, x, 1, 0, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, x, z, y, 1, 0, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, x, z, z, 1, 0, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, x, z, w, 1, 0, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, x, w, x, 1, 0, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, x, w, y, 1, 0, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, x, w, z, 1, 0, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, x, w, w, 1, 0, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, y, y, x, x, 1, 1, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, y, y, x, y, 1, 1, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, y, x, z, 1, 1, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, y, x, w, 1, 1, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, y, y, y, x, 1, 1, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 2, y, y, y, y, 1, 1, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, y, y, z, 1, 1, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, y, y, w, 1, 1, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, y, z, x, 1, 1, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, y, z, y, 1, 1, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, y, z, z, 1, 1, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, y, z, w, 1, 1, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, y, w, x, 1, 1, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, y, w, y, 1, 1, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, y, w, z, 1, 1, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, y, w, w, 1, 1, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, x, x, 1, 2, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, x, y, 1, 2, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, x, z, 1, 2, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, z, x, w, 1, 2, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, y, x, 1, 2, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, y, y, 1, 2, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, y, z, 1, 2, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, z, y, w, 1, 2, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, z, x, 1, 2, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, z, y, 1, 2, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, y, z, z, z, 1, 2, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, z, z, w, 1, 2, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, z, w, x, 1, 2, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, z, w, y, 1, 2, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, z, w, z, 1, 2, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, z, w, w, 1, 2, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, x, x, 1, 3, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, x, y, 1, 3, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, x, z, 1, 3, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, x, w, 1, 3, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, y, x, 1, 3, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, y, y, 1, 3, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, y, z, 1, 3, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, y, w, 1, 3, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, z, x, 1, 3, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, z, y, 1, 3, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, z, z, 1, 3, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, z, w, 1, 3, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, w, x, 1, 3, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, w, y, 1, 3, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, w, z, 1, 3, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, y, w, w, w, 1, 3, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, x, x, 2, 0, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, x, y, 2, 0, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, x, z, 2, 0, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, x, x, w, 2, 0, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, y, x, 2, 0, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, y, y, 2, 0, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, y, z, 2, 0, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, x, y, w, 2, 0, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, z, x, 2, 0, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, z, y, 2, 0, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, x, z, z, 2, 0, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, x, z, w, 2, 0, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, x, w, x, 2, 0, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, x, w, y, 2, 0, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, x, w, z, 2, 0, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, x, w, w, 2, 0, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, x, x, 2, 1, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, x, y, 2, 1, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, x, z, 2, 1, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, y, x, w, 2, 1, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, y, x, 2, 1, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, y, y, 2, 1, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, y, z, 2, 1, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, y, y, w, 2, 1, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, z, x, 2, 1, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, z, y, 2, 1, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, y, z, z, 2, 1, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, y, z, w, 2, 1, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, y, w, x, 2, 1, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, y, w, y, 2, 1, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, y, w, z, 2, 1, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, y, w, w, 2, 1, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, x, x, 2, 2, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, x, y, 2, 2, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, x, z, 2, 2, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, z, x, w, 2, 2, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, y, x, 2, 2, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, y, y, 2, 2, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, y, z, 2, 2, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, z, y, w, 2, 2, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, z, x, 2, 2, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, z, y, 2, 2, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 3, z, z, z, z, 2, 2, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, z, z, w, 2, 2, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, z, w, x, 2, 2, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, z, w, y, 2, 2, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, z, w, z, 2, 2, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, z, w, w, 2, 2, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, x, x, 2, 3, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, x, y, 2, 3, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, x, z, 2, 3, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, x, w, 2, 3, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, y, x, 2, 3, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, y, y, 2, 3, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, y, z, 2, 3, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, y, w, 2, 3, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, z, x, 2, 3, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, z, y, 2, 3, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, z, z, 2, 3, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, z, w, 2, 3, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, w, x, 2, 3, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, w, y, 2, 3, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, w, z, 2, 3, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, z, w, w, w, 2, 3, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, x, x, 3, 0, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, x, y, 3, 0, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, x, z, 3, 0, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, x, w, 3, 0, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, y, x, 3, 0, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, y, y, 3, 0, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, y, z, 3, 0, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, y, w, 3, 0, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, z, x, 3, 0, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, z, y, 3, 0, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, z, z, 3, 0, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, z, w, 3, 0, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, w, x, 3, 0, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, w, y, 3, 0, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, w, z, 3, 0, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, x, w, w, 3, 0, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, x, x, 3, 1, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, x, y, 3, 1, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, x, z, 3, 1, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, x, w, 3, 1, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, y, x, 3, 1, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, y, y, 3, 1, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, y, z, 3, 1, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, y, w, 3, 1, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, z, x, 3, 1, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, z, y, 3, 1, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, z, z, 3, 1, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, z, w, 3, 1, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, w, x, 3, 1, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, w, y, 3, 1, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, w, z, 3, 1, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, y, w, w, 3, 1, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, x, x, 3, 2, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, x, y, 3, 2, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, x, z, 3, 2, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, x, w, 3, 2, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, y, x, 3, 2, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, y, y, 3, 2, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, y, z, 3, 2, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, y, w, 3, 2, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, z, x, 3, 2, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, z, y, 3, 2, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, z, z, 3, 2, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, z, w, 3, 2, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, w, x, 3, 2, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, w, y, 3, 2, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, w, z, 3, 2, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, z, w, w, 3, 2, 3, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, x, x, 3, 3, 0, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, x, y, 3, 3, 0, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, x, z, 3, 3, 0, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, x, w, 3, 3, 0, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, y, x, 3, 3, 1, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, y, y, 3, 3, 1, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, y, z, 3, 3, 1, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, y, w, 3, 3, 1, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, z, x, 3, 3, 2, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, z, y, 3, 3, 2, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, z, z, 3, 3, 2, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, z, w, 3, 3, 2, 3)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, w, x, 3, 3, 3, 0)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, w, y, 3, 3, 3, 1)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, w, z, 3, 3, 3, 2)                                          \
    SEK_MAKE_VEC_GETTERS4(type, 4, w, w, w, w, 3, 3, 3, 3)

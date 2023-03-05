/*
 * Created by switchblade on 2023-03-05.
 */

#pragma once

#include "define.hpp"

#include <span>

namespace sek::sys
{
	/** Fills a buffer \a dst with \a n random bytes generated using OS-specific cryptographic source.
	 * This function will always fail in case no cryptographic source is available for this system.
	 * @param dst Destination byte buffer.
	 * @param n Amount of random bytes to generate.
	 * @return Amount of bytes filled, or -1 if a failure occurred. */
	inline ssize_t random(std::span<std::byte> dst, std::size_t n) noexcept;
	/** @copydoc random
	 * @note This overload assumes that \a dst is at least large enough to fit \a n bytes. */
	SEK_MATH_PUBLIC ssize_t random(void *dst, std::size_t n) noexcept;

	SEK_FORCEINLINE ssize_t random(std::span<std::byte> dst, std::size_t n) noexcept { return random(dst.data(), std::min(dst.size(), n)); }
}
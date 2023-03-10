/*
 * Created by switchblade on 2023-03-05.
 */

#if defined(__linux__) || defined(__OpenBSD__)
#define _DEFAULT_SOURCE
#include <linux/version.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

#include "sysrandom.hpp"

#if defined(_WIN32) || defined(__CYGWIN__)

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

sek::ssize_t sek::sys::random(void *dst, std::size_t n) noexcept
{
	BCRYPT_ALG_HANDLE rng_alg;
	if (BCryptOpenAlgorithmProvider(&rng_alg, BCRYPT_RNG_ALGORITHM, nullptr, 0) != STATUS_SUCCESS)
		[[unlikely]] return -1;

	auto result = static_cast<ssize_t>(n);
	if (BCryptGenRandom(rng_alg, static_cast<PUCHAR>(dst), n, 0) != STATUS_SUCCESS)
		[[unlikely]] result = -1;
	if (BCryptCloseAlgorithmProvider(rng_alg, 0) != STATUS_SUCCESS)
		[[unlikely]] result = -1;
	return result;
}

#elif defined(__linux__) && LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)

sek::ssize_t sek::sys::random(void *dst, std::size_t n) noexcept { return syscall(SYS_getrandom, dst, n, 0); }

#elif defined(__linux__) || defined(__OpenBSD__)

sek::ssize_t sek::sys::random(void *dst, std::size_t n) noexcept { return getentropy(dst, n); }

#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))

sek::ssize_t sek::sys::random(void *dst, std::size_t n) noexcept { return getentropy(dst, n); }

#include <cstdio>

sek::ssize_t sek::sys::random(void *dst, std::size_t n) noexcept
{
	if (auto urandom = fopen("/dev/urandom", "rb"); urandom) [[likely]]
		return static_cast<ssize_t>(fread(dst, n, 1, urandom));
	return -1;
}

#else

sek::ssize_t sek::sys::random(void *, std::size_t) noexcept { return -1; }

#endif

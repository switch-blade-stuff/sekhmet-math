/*
 * Created by switchblade on 2023-02-23.
 */

#pragma once

#include "type_vec.hpp"

#include <dpm/math.hpp>

namespace sek::detail
{
	template<typename... Ts>
	using promote_t = std::conditional_t<std::disjunction_v<std::is_same<Ts, long double>...>, long double, double>;
}
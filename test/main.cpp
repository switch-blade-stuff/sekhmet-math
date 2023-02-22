/*
 * Created by switchblade on 2023-01-01.
 */

#include <math/vector.hpp>

auto mul(sek::math::vec2<float> a, sek::math::vec2<float> b)
{
	return a * b;
}
auto sin(sek::math::vec2<float> x)
{
	return sek::math::vec2<float>{dpm::sin(to_simd(x))};
}

int main()
{

}
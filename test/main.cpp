/*
 * Created by switchblade on 2023-01-01.
 */

#include <math/vector.hpp>

auto test(sek::vec4<float> a, sek::vec4<float> b, sek::vec4<float> c)
{
	return sek::fmadd(a, b, c);
}

int main()
{

}
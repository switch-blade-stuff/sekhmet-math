/*
 * Created by switchblade on 2023-01-01.
 */

#include <math/vector.hpp>
#include <math/matrix.hpp>

auto test(sek::mat4x4<float> m, sek::vec3<float> v, sek::vec2<float> sx, sek::vec2<float> sy, sek::vec2<float> sz)
{
	return sek::shear(m, v, sx, sy, sz);
}

int main()
{
	/* | 1 | 2 | 4 |
	 * | 0 | 1 | 2 |
	 * | 0 | 0 | 1 |
	 * | 0 | 0 | 0 | */
	const auto c0 = sek::vec1<float>{1};
	const auto c1 = sek::vec1<float>{2};
	const auto c2 = sek::vec2<float>{4, 2};

	const auto m0 = sek::mat3x4<float>{c0, c1, sek::vec3<float>{c2, 1}};
	const auto m1 = sek::mat3x4<float>{c0, c1, c2};

	const auto [m0c0, m0c1, m0c2] = m0;
	const auto [m1c0, m1c1, m1c2] = m1;

	return m0 == m1 && m0c0 == m1c0 && m0c1 == m1c1 && m0c2 == m1c2 &&
	       m0c0 == sek::vec4<float>{1, 0, 0, 0} &&
	       m0c1 == sek::vec4<float>{2, 1, 0, 0} &&
	       m0c2 == sek::vec4<float>{4, 2, 1, 0} &&
	       m1c0 == sek::vec4<float>{1, 0, 0, 0} &&
	       m1c1 == sek::vec4<float>{2, 1, 0, 0} &&
	       m1c2 == sek::vec4<float>{4, 2, 1, 0};
}
/*
 * Created by switchblade on 2023-01-01.
 */

#include <math/vector.hpp>
#include <math/matrix.hpp>

int main()
{
	/* | 1 | 2 | 3 |
	 * | 0 | 1 | 4 |
	 * | 0 | 0 | 1 |
	 * | 0 | 0 | 0 | */
	const auto c0 = sek::vec1<float>{1};
	const auto c1 = sek::vec1<float>{2};
	const auto c2 = sek::vec2<float>{3, 4};

	const auto m0 = sek::mat3x4<float>{c0, c1, sek::vec3<float>{c2, 1}};
	const auto m1 = sek::mat3x4<float>{c0, c1, c2};
	return m0 == m1;
}
/*
 * Created by switchblade on 2023-01-01.
 */

#include <math/math.hpp>

inline void test_rotate() noexcept
{
	const auto invoke_test = [](sek::vec3<float> v, float angle, sek::vec3<float> axis, sek::vec3<float> expected)
	{
		const auto m4 = rotate(sek::mat4x4<float>::identity(), angle, axis);
		const auto m3 = rotate(sek::mat3x3<float>::identity(), angle, axis);
		const auto q = sek::quat<float>::angle_axis(angle, axis);

		const auto v0 = (m4 * sek::vec4<float>{v, 1}).xyz();
		const auto v1 = m3 * v;
		const auto v2 = q * v;

		DPM_ASSERT_ALWAYS(sek::fcmp_eq(v0, expected));
		DPM_ASSERT_ALWAYS(sek::fcmp_eq(v0, v1));
		DPM_ASSERT_ALWAYS(sek::fcmp_eq(v0, v2));
	};

	invoke_test(sek::vec3<float>::left(), sek::rad(90.0f), sek::vec3<float>::up(), sek::vec3<float>::forward());
	invoke_test(sek::vec3<float>::left(), sek::rad(-90.0f), sek::vec3<float>::up(), sek::vec3<float>::backward());
	invoke_test(sek::vec3<float>::right(), sek::rad(90.0f), sek::vec3<float>::up(), sek::vec3<float>::backward());
	invoke_test(sek::vec3<float>::right(), sek::rad(-90.0f), sek::vec3<float>::up(), sek::vec3<float>::forward());

	invoke_test(sek::vec3<float>::up(), sek::rad(90.0f), sek::vec3<float>::left(), sek::vec3<float>::forward());
	invoke_test(sek::vec3<float>::up(), sek::rad(-90.0f), sek::vec3<float>::left(), sek::vec3<float>::backward());
	invoke_test(sek::vec3<float>::down(), sek::rad(90.0f), sek::vec3<float>::left(), sek::vec3<float>::backward());
	invoke_test(sek::vec3<float>::down(), sek::rad(-90.0f), sek::vec3<float>::left(), sek::vec3<float>::forward());

	invoke_test(sek::vec3<float>::up(), sek::rad(90.0f), sek::vec3<float>::forward(), sek::vec3<float>::right());
	invoke_test(sek::vec3<float>::up(), sek::rad(-90.0f), sek::vec3<float>::forward(), sek::vec3<float>::left());
	invoke_test(sek::vec3<float>::down(), sek::rad(90.0f), sek::vec3<float>::forward(), sek::vec3<float>::left());
	invoke_test(sek::vec3<float>::down(), sek::rad(-90.0f), sek::vec3<float>::forward(), sek::vec3<float>::right());
}

int main()
{
	test_rotate();
}
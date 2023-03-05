/*
 * Created by switchblade on 2023-01-01.
 */

#include <math/math.hpp>

#define TEST_ASSERT(cnd) DPM_ASSERT_ALWAYS(cnd)

inline void test_rotate() noexcept
{
	const auto invoke_test = [](sek::vec3<float> v, float angle, sek::vec3<float> axis, sek::vec3<float> expected)
	{
		const auto m40 = rotate(sek::mat4x4<float>::identity(), angle, axis);
		const auto m30 = rotate(sek::mat3x3<float>::identity(), angle, axis);
		const auto q0 = sek::quat<float>::angle_axis(angle, axis);

		const auto m31 = rotate(sek::mat3x3<float>::identity(), -angle, axis);
		const auto m41 = rotate(sek::mat4x4<float>::identity(), -angle, axis);
		const auto q1 = sek::quat<float>::angle_axis(angle, -axis);

		auto v0 = (m40 * sek::vec4<float>{v, 1}).xyz();
		auto v1 = m30 * v;
		auto v2 = q0 * v;

		TEST_ASSERT(sek::fcmp_eq(v0, expected));
		TEST_ASSERT(sek::fcmp_eq(v0, v1));
		TEST_ASSERT(sek::fcmp_eq(v0, v2));

		v0 = (m41 * sek::vec4<float>{v, 1}).xyz();
		v1 = m31 * v;
		v2 = q1 * v;

		TEST_ASSERT(sek::fcmp_eq(v0, -expected));
		TEST_ASSERT(sek::fcmp_eq(v0, v1));
		TEST_ASSERT(sek::fcmp_eq(v0, v2));
	};

	invoke_test(sek::vec3<float>::left(), sek::rad(90.0f), sek::vec3<float>::up(), sek::vec3<float>::backward());
	invoke_test(sek::vec3<float>::left(), sek::rad(-90.0f), sek::vec3<float>::up(), sek::vec3<float>::forward());
	invoke_test(sek::vec3<float>::right(), sek::rad(90.0f), sek::vec3<float>::up(), sek::vec3<float>::forward());
	invoke_test(sek::vec3<float>::right(), sek::rad(-90.0f), sek::vec3<float>::up(), sek::vec3<float>::backward());

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
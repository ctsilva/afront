#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/real/real.hpp>
#include <gtb/math/math.hpp>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#endif // WIN32

using namespace gtb;


int main(int, char *[])
{
	printf("sizeof(float) == %d\n", sizeof(float));
	printf("sizeof(double) == %d\n", sizeof(double));
	printf("sizeof(real_t) == %d\n", sizeof(real_t));

	printf("\n");
	printf("epsf() = %e\n", real::epsf());
	printf("epsd() = %e\n", real::epsd());
	printf("FLT_EPSILON = %e\n", FLT_EPSILON);
	printf("DBL_EPSILON = %e\n", DBL_EPSILON);
	printf("EPS = %e\n", real::EPS);

	printf("\n");
	printf("FLT_MIN = %e\n", FLT_MIN);
	printf("DBL_MIN = %e\n", DBL_MIN);
	printf("MIN = %e\n", real::MIN);

	printf("\n");
	printf("FLT_MAX = %e\n", FLT_MAX);
	printf("DBL_MAX = %e\n", DBL_MAX);
	printf("MAX = %e\n", real::MAX);

	real_t a = M_PI / 4.0;
	real_t b = atan(1.0);
	printf("\n");
	printf("a = %e\n", a);
	printf("b = %e\n", b);
	printf("a - b = %.20e\n", a - b);
	printf("((a - b) == 0) = %d\n", real::is_zero(a - b));

	assert(real::is_zero(0.0));
	assert(!real::is_positive(0.0));
	assert(!real::is_negative(0.0));
	assert(real::is_positive_or_zero(0.0));
	assert(real::is_negative_or_zero(0.0));

	assert(real::is_zero(real::EPS / 2.0));
	assert(!real::is_positive(real::EPS / 2.0));
	assert(!real::is_negative(real::EPS / 2.0));
	assert(real::is_positive_or_zero(real::EPS / 2.0));
	assert(real::is_negative_or_zero(real::EPS / 2.0));

	assert(real::is_zero(-real::EPS / 2.0));
	assert(!real::is_positive(-real::EPS / 2.0));
	assert(!real::is_negative(-real::EPS / 2.0));
	assert(real::is_positive_or_zero(-real::EPS / 2.0));
	assert(real::is_negative_or_zero(-real::EPS / 2.0));

	assert(!real::is_zero(real::EPS));
	assert(real::is_positive(real::EPS));
	assert(real::is_positive_or_zero(real::EPS));
	assert(!real::is_negative(real::EPS));
	assert(!real::is_negative_or_zero(real::EPS));

	assert(!real::is_zero(-real::EPS));
	assert(!real::is_positive(-real::EPS));
	assert(!real::is_positive_or_zero(-real::EPS));
	assert(real::is_negative(-real::EPS));
	assert(real::is_negative_or_zero(-real::EPS));

	assert(real::is_equal(1.0, 1.0));
	assert(real::is_equal(1.0, 1.0 + real::EPS / 2));
	assert(real::is_equal(1.0, 1.0 - real::EPS / 2));
	assert(!real::is_equal(1.0, 1.0 + real::EPS));
	assert(!real::is_equal(1.0, 1.0 - real::EPS));

	assert(!real::is_greater(1.0, 1.0));
	assert(!real::is_greater(1.0, 1.0 + real::EPS / 2));
	assert(!real::is_greater(1.0, 1.0 - real::EPS / 2));
	assert(!real::is_greater(1.0, 1.0 + real::EPS));
	assert(real::is_greater(1.0, 1.0 - real::EPS));

	assert(real::is_greater_or_equal(1.0, 1.0));
	assert(real::is_greater_or_equal(1.0, 1.0 + real::EPS / 2));
	assert(real::is_greater_or_equal(1.0, 1.0 - real::EPS / 2));
	assert(!real::is_greater_or_equal(1.0, 1.0 + real::EPS));
	assert(real::is_greater_or_equal(1.0, 1.0 - real::EPS));

	assert(!real::is_less(1.0, 1.0));
	assert(!real::is_less(1.0, 1.0 + real::EPS / 2));
	assert(!real::is_less(1.0, 1.0 - real::EPS / 2));
	assert(real::is_less(1.0, 1.0 + real::EPS));
	assert(!real::is_less(1.0, 1.0 - real::EPS));

	assert(real::is_less_or_equal(1.0, 1.0));
	assert(real::is_less_or_equal(1.0, 1.0 + real::EPS / 2));
	assert(real::is_less_or_equal(1.0, 1.0 - real::EPS / 2));
	assert(real::is_less_or_equal(1.0, 1.0 + real::EPS));
	assert(!real::is_less_or_equal(1.0, 1.0 - real::EPS));

	return EXIT_SUCCESS;
}

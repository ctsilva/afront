#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/math/math.hpp>
#endif // WIN32

using namespace std;
using namespace gtb;


int main(int, char *[])
{
	printf("%f\n", M_PI / 4.0);
	printf("%f\n", deg_to_rad(45.0));
	printf("%f\n", rad_to_deg(M_PI / 4.0));
	printf("%d\n", abs(1));
	printf("%f\n", abs(-1.2));
	printf("%f\n", abs(0.0));
	printf("%d\n", max(1, 3));
	printf("%f\n", min(3.0, 1.0));
	return EXIT_SUCCESS;
}

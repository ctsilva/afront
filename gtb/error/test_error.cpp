#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/error/error.hpp>
#include <stdio.h>
#include <stdlib.h>
#endif // WIN32
using namespace gtb;


int main(int argc, char *argv[])
{
	set_program_name(argv[0]);

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <[1-4]>\n", program_name());
		exit(EXIT_FAILURE);
	}
	int testno = atoi(argv[1]);

	switch (testno) {
	case 1:
	{
		const char *fname = "/non/existent/file";
		FILE *fp = fopen(fname, "rb");
		if (NULL == fp) {
			GTB_ERROR(fname);
		}
		printf("should not see this\n");
		break;
	}
	case 2:
		GTB_WARNING("division by zero\n");
		printf("should see this\n");
		break;
	case 3:
		REQUIRE(2 + 2 == 5);
		break;
	case 4:
		ENSURE(2 + 2 == 3);
		break;
	default:
		GTB_ERROR("invalid test number");
		break;
	}

	return EXIT_SUCCESS;
}

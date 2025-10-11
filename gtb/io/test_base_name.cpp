#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/io/io.hpp>
#include <stdlib.h>
#endif // WIN32
using namespace gtb;


int main(int argc, char *argv[])
{
	if ((2 != argc) && (3 != argc)) {
		fprintf(stderr, "Usage: %s <FILE> [SUFFIX]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char fname[PATH_MAX];
	if (2 == argc) {
		get_file_base_name(argv[1], fname, PATH_MAX);
	} else if (3 == argc) {
		get_file_base_name(argv[1], argv[2], fname, PATH_MAX);
	} else {
		fprintf(stderr, "internal error\n");
		exit(EXIT_FAILURE);
	}
  	printf("%s\n", fname);
	return 0;
}

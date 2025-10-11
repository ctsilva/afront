#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/io/io.hpp>
#include <stdlib.h>
#endif // WIN32
using namespace gtb;


int main(int argc, char *argv[])
{
	if (2 != argc) {
		fprintf(stderr, "Usage: %s <FILE>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char extension[10];
  	get_file_extension(argv[1], extension, sizeof(extension));
  	printf("%s\n", extension);
	return 0;
}

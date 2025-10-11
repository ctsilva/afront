#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/error/error.hpp>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#endif // WIN32

using namespace std;

GTB_BEGIN_NAMESPACE


static string _program_name;


const char *program_name()
{
	return _program_name.c_str();
}


void set_program_name(const char *argv0)
{
	assert(argv0 != NULL);
	_program_name = argv0;
}


void error(const char *file, int line, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	if (program_name() != NULL) {
		fprintf(stderr, "%s: ", program_name());
	}
	fprintf(stderr, "ERROR: %s:%d: ", file, line);
	vfprintf(stderr, fmt, args);
	va_end(args);
	if (errno != 0) {
		fprintf(stderr, ": ");
		perror(NULL);
	} else {
		fprintf(stderr, "\n");
	}
	exit(EXIT_FAILURE);
}


void warning(const char *file, int line, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	if (program_name() != NULL) {
		fprintf(stderr, "%s: ", program_name());
	}
	fprintf(stderr, "WARNING: %s:%d: ", file, line);
	vfprintf(stderr, fmt, args);
	va_end(args);
	if (errno != 0) {
		fprintf(stderr, ": ");
		perror(NULL);
	} else {
		fprintf(stderr, "\n");
	}
}


GTB_END_NAMESPACE

#ifndef GTB_ERROR_INCLUDED
#define GTB_ERROR_INCLUDED

#include <gtb/common.hpp>

GTB_BEGIN_NAMESPACE

const char *program_name();
void set_program_name(const char *argv0);

void error(const char *file, int line, const char *fmt, ...);
void warning(const char *file, int line, const char *fmt, ...);

#define GTB_ERROR(args) error(__FILE__, __LINE__, args)
#define GTB_WARNING(args) warning(__FILE__, __LINE__, args)

#define REQUIRE(condition) \
do { \
	if (!(condition)) { \
		error(__FILE__, __LINE__, "precondition violation: %s\n", \
		      # condition); \
	} \
} while (0)

#define ENSURE(condition) \
do { \
	if (!(condition)) { \
		error(__FILE__, __LINE__, "postcondition violation: %s\n", \
		      # condition); \
	} \
} while (0)

GTB_END_NAMESPACE

#include <gtb/error/err.h>

#endif // GTB_ERROR_INCLUDED

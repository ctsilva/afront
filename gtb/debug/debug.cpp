#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/debug/debug.hpp>
#include <stdarg.h>
#include <stdio.h>
#endif // WIN32


GTB_BEGIN_NAMESPACE

int debug::_indent = 0;


debug::debug(const char *s)
	: _s(s)
{
	fprintf(stderr, "\n");
	indent();
	fprintf(stderr, "%s {\n", _s);
	_indent++;
}


debug::~debug()
{
	_indent--;
	indent();
	fprintf(stderr, "} // %s\n\n", _s);
}


void debug::print(const char *fmt, ...) const
{
	va_list args;
	va_start(args, fmt);
	indent();
	vfprintf(stderr, fmt, args);
	va_end(args);
}


void debug::update(const char *fmt, ...) const
{
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "\r");
	indent();
	vfprintf(stderr, fmt, args);
	va_end(args);
}


void debug::indent() const
{
	for (int i = 0; i < _indent; i++) {
		fprintf(stderr, "    ");
	}
}


GTB_END_NAMESPACE

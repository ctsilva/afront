#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/debug/debug.hpp>
#include <stdlib.h>
#endif // WIN32
using namespace gtb;


void count(int n)
{
	debug d("counting");
	d.print("i: ");
	for (int i = 0; i < n; i++) {
		d.update("i: %d", i);
	}
	d.print("\n");
}


void foo()
{
	debug d("foo");
	count(100000);
}


int main(int, char *[])
{
	debug d("main");
	foo();
	return EXIT_SUCCESS;
}

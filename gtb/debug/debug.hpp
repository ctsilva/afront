#ifndef DEBUG_INCLUDED
#define DEBUG_INCLUDED

#include <gtb/common.hpp>

GTB_BEGIN_NAMESPACE


class debug {
public:
	debug(const char *s);
	~debug();
	void print(const char *fmt, ...) const;
	void update(const char *fmt, ...) const;
private:
	void indent() const;
	static int _indent;
	const char *_s;
};


GTB_END_NAMESPACE

#endif // DEBUG_INCLUDED

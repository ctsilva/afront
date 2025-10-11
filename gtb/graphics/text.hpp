#ifndef GTB_TEXT_INCLUDED
#define GTB_TEXT_INCLUDED

#include <gtb/common.hpp>

GTB_BEGIN_NAMESPACE


void draw_string(const char *s, int x, int y);
void draw_int(int n, int x, int y);
void draw_text_begin(int l, int r, int b, int t);
void draw_text_end();


GTB_END_NAMESPACE

#endif // GTB_TEXT_INCLUDED

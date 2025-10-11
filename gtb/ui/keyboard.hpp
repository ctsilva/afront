#ifndef GTB_KEYBOARD_INCLUDED
#define GTB_KEYBOARD_INCLUDED

#include <gtb/common.hpp>


#ifdef WIN32

GTB_BEGIN_NAMESPACE

class keyboard {
public:
	enum t_modifiers {key_ctrl, key_shift, key_alt};

	// Hack to work around bug in GLUT, which does not generate
	// KeyPress or KeyRelease events fot the modifiers.
	static bool is_key_pressed(t_modifiers key);
	static bool is_shift_pressed();
	static bool is_control_pressed();
	static bool is_alt_pressed();
};

GTB_END_NAMESPACE

#else // WIN32

#include <X11/Xlib.h>
#include <X11/keysym.h>

GTB_BEGIN_NAMESPACE

class keyboard {
public:
	// Hack to work around bug in GLUT, which does not generate
	// KeyPress or KeyRelease events fot the modifiers.
	static bool is_key_pressed(int keysym);
	static bool is_shift_pressed();
	static bool is_control_pressed();
	static bool is_alt_pressed();
	static void print_keys_pressed();

protected:
	static Display *init_display();
	static Display *_disp;
};

GTB_END_NAMESPACE

#endif // WIN32


#endif // GTB_KEYBOARD_INCLUDED

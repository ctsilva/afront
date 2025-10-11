#ifndef GTB_MOUSE_INCLUDED
#define GTB_MOUSE_INCLUDED


#include <assert.h>
#include <gtb/graphics/ogltools.h>

class Mouse {
public:
	Mouse() {
		_buttons[GLUT_LEFT_BUTTON] = GLUT_UP;
		_buttons[GLUT_MIDDLE_BUTTON] = GLUT_UP;
		_buttons[GLUT_RIGHT_BUTTON] = GLUT_UP;
		_start_x = 0;
		_start_y = 0;
		_x = 0;
		_y = 0;
		_old_x = 0;
		_old_y = 0;
	}

	int start_x() const { return _start_x; }
	int start_y() const { return _start_y; }
	int x() const { return _x; }
	int y() const { return _y; }
	int dx() const { return _x - _old_x; }
	int get_dx() const { return _x - _old_x; }
	int dy() const { return _y - _old_y; }
	int get_dy() const { return _y - _old_y; }

	bool is_button_pressed(unsigned button) const {
		assert(button < 3);
		return _buttons[button] == GLUT_DOWN;
	}

	void set_button_state(unsigned button, unsigned state) {
		assert(button < 3);
		assert((state == GLUT_UP) || (state == GLUT_DOWN));
		_buttons[button] = state;
	}

	void set_starting_position(int arg_x, int arg_y) {
		_start_x = arg_x;
		_start_y = arg_y;
	}

	void set_position(int arg_x, int arg_y) {
		_old_x = _x;
		_old_y = _y;
		_x = arg_x;
		_y = arg_y;
	}

protected:
	int _buttons[3];
	int _start_x, _start_y;
	int _x, _y;
	int _old_x, _old_y;
};


#endif // GTB_MOUSE_INCLUDED

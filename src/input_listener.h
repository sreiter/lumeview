#ifndef __H__input_listener
#define __H__input_listener

#include <vector>
#include <chrono>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

enum MouseButton {
	LEFT = 0,
	RIGHT = 1,
	MIDDLE = 2,

	NUM_BUTTONS
};


enum MouseButtonAction {
	UP = 0,
	DOWN = 1,

	NUM_ACTIONS
};

/** When you override a virtual method from this class in a derived class,
 * make sure to first call the base class implementation before any other
 * operation. This makes sure, that helper methods return the correct values
 * (e.g. 'was_double_click' or 'cursor_position')
 */
class InputListener {
public:
	InputListener ();
	virtual ~InputListener ();

	virtual void mouse_button (int button, int action, int mods);
  	virtual void mouse_move (const glm::vec2& c);
  	virtual void mouse_scroll (const glm::vec2& o);

  	glm::vec2 cursor_position () const;

  	bool was_double_click (int button) const;
  	bool mouse_button_is_down (int button) const;
  	glm::vec2 mouse_button_down_pos (int button) const;


private:
	using clock = std::chrono::steady_clock;

	struct MouseBtnInfo {
		MouseBtnInfo ();
		clock::time_point	lastDown;
		glm::vec2			downPos;
		bool				isDown;
		bool				wasDoubleClick;
	};

	glm::vec2					m_cursorPos;
	std::vector<MouseBtnInfo>	m_mouseBtnInfo;
};

#endif	//__H__input_listener

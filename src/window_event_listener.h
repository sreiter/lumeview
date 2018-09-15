#ifndef __H__window_event_listener
#define __H__window_event_listener

#include <vector>
#include <chrono>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace lumeview {

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

/** 
 * \note All buttons, keys, actions, mods, etc correspond to glfw
 *
 * When you override a virtual method from this class in a derived class,
 * make sure to first call the base class implementation before any other
 * operation. This makes sure, that helper methods return the correct values
 * (e.g. 'was_double_click' or 'cursor_position').
 * An exception may be that you want to compare to a previous state. In that
 * case it could make sense to store the current state of the base class before
 * calling the corresponding base class method.
 */
class WindowEventListener {
public:
	WindowEventListener ();
	virtual ~WindowEventListener ();

//	mouse
	virtual void mouse_button (int button, int action, int mods);
  	virtual void mouse_move (const glm::vec2& c);
  	virtual void mouse_scroll (const glm::vec2& o);

  	glm::vec2 cursor_position () const;

  	bool was_double_click (int button) const;
  	bool mouse_button_is_down (int button) const;
  	glm::vec2 mouse_button_down_pos (int button) const;

//	window
  	virtual void set_viewport (const glm::ivec4& vp);
  	virtual const glm::ivec4& viewport () const;

//	keyboard
  	virtual void key (int key, int scancode, int action, int mods);
  	virtual void character (unsigned int c);

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
	glm::ivec4  				m_viewport;
	std::vector<MouseBtnInfo>	m_mouseBtnInfo;
};

}// end of namespace lumeview

#endif	//__H__window_event_listener

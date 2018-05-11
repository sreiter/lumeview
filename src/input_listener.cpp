#include "input_listener.h"

using namespace std;

InputListener::MouseBtnInfo::MouseBtnInfo () :
		downPos (0),
		isDown (false),
		wasDoubleClick (false)
{
}


InputListener::InputListener () :
	m_cursorPos (0)
{
}

InputListener::~InputListener ()
{

}


void InputListener::mouse_button (int btn, int action, int mods)
{
	static const double dblClickInterval = 250.0;
	if (btn < 0)
		return;

	if(btn >= (int)m_mouseBtnInfo.size()){
		m_mouseBtnInfo.resize (btn + 1);
	}

	if(action == MouseButtonAction::DOWN) {
		const auto curTime = chrono::steady_clock::now();
		const double interval = chrono::duration <double, milli> (curTime - m_mouseBtnInfo [btn].lastDown).count();
		m_mouseBtnInfo [btn].wasDoubleClick = (interval < dblClickInterval);
		m_mouseBtnInfo [btn].lastDown = curTime;
		m_mouseBtnInfo [btn].downPos = m_cursorPos;
		m_mouseBtnInfo [btn].isDown = true;
	}
	else if (action == MouseButtonAction::UP) {
		m_mouseBtnInfo [btn].wasDoubleClick = false;
		m_mouseBtnInfo [btn].isDown = false;
	}
}

void InputListener::mouse_move (const glm::vec2& c)
{
	m_cursorPos = c;
}

void InputListener::mouse_scroll (const glm::vec2& o)
{

}

glm::vec2 InputListener::cursor_position () const
{
	return m_cursorPos;
}

bool InputListener::was_double_click (int btn) const
{
	if (btn >= 0 && btn < m_mouseBtnInfo.size ())
		return m_mouseBtnInfo [btn].wasDoubleClick;
	return false;
}

bool InputListener::mouse_button_is_down (int btn) const
{
	if (btn >= 0 && btn < m_mouseBtnInfo.size ())
		return m_mouseBtnInfo [btn].isDown;
	return false;
}

glm::vec2 InputListener::mouse_button_down_pos (int btn) const
{
	if (btn >= 0 && btn < m_mouseBtnInfo.size ())
		return m_mouseBtnInfo [btn].downPos;
	return glm::vec2(0);
}

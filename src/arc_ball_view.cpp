#include <chrono>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "arc_ball_view.h"
#include "cond_throw.h"

using namespace std;

namespace slimesh {

ArcBallView::ArcBallView ()
{
	m_camera.set_rotation (m_arcBall.rotation_quaternion ());
	m_view.set_view_matrix (m_camera.view_matrix());
}


const View& ArcBallView::view () const
{
	return m_view;
}


void ArcBallView::set_viewport (const glm::ivec4& vp)
{
    COND_THROW((vp.x == vp.z) || (vp.y == vp.w),
               "Bad viewport spefified: " << vp);
    m_arcBall.set_frame (glm::vec2(vp.z - vp.x, vp.w - vp.y));
    m_arcBall.set_offset (glm::vec2(vp.x, vp.y));
    m_view.set_viewport (vp);
    m_view.set_view_matrix (m_camera.view_matrix());
}


const glm::ivec4& ArcBallView::viewport () const
{
	return m_view.viewport ();
}

void ArcBallView::mouse_button (int button, int action, int mods)
{
	base_t::mouse_button (button, action, mods);

	if(button == MouseButton::LEFT){
		switch(action) {
			case MouseButtonAction::DOWN: {
				const glm::vec2& cursorPos = base_t::cursor_position ();
				if (base_t::was_double_click (button)) {
					m_arcBall.end_drag ();
					float z = m_view.depth_at_screen_coord (cursorPos);
					if (z < 1.f) {
						glm::vec3 p = m_view.unproject (glm::vec3(cursorPos.x, cursorPos.y, z));
						m_camera.set_translation (p);
					}
				}
				else
					m_arcBall.begin_drag (glm::vec2(cursorPos.x, cursorPos.y));
			} break;

			case MouseButtonAction::UP: {
				if(m_arcBall.dragging ())
					m_arcBall.end_drag ();
			} break;
		}
	}

	m_view.set_view_matrix (m_camera.view_matrix());
}


void ArcBallView::mouse_move (const glm::vec2& c)
{
	glm::vec2 lastCursorPos = base_t::cursor_position();
	base_t::mouse_move (c);

	if(m_arcBall.dragging()){
		m_arcBall.drag_to (c);
		m_camera.set_rotation (m_arcBall.rotation_quaternion());
	}
	else {
		static const float translationSpeed = 0.003f;
		if(base_t::mouse_button_is_down (MouseButton::RIGHT)) {
			glm::vec2 d = base_t::cursor_position() - lastCursorPos;
			d *= translationSpeed;
			if (m_camera.scale().x != 0){
				d /= m_camera.scale().x;
				m_camera.translate (-d.x * m_camera.right () + d.y * m_camera.up ());
			}
		}
	}
	
	m_view.set_view_matrix (m_camera.view_matrix());
}


void ArcBallView::mouse_scroll (const glm::vec2& o)
{
	base_t::mouse_scroll (o);
	float s = 1.f + o.y / 10.f;
	m_camera.scale (glm::vec3(s));
	m_view.set_view_matrix (m_camera.view_matrix());
}

}// end of namespace slimesh


#include <chrono>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "arc_ball_view.h"
#include "lumeview_error.h"
#include "log.h"
#include "vec_math.h"

using namespace std;

namespace lumeview {

ArcBallView::ArcBallView ()
{
	m_view.camera().set_rotation (m_arcBall.rotation_quaternion ());
}


View& ArcBallView::view ()
{
	return m_view;
}


const View& ArcBallView::view () const
{
	return m_view;
}


void ArcBallView::set_viewport (const glm::ivec4& vp)
{
	base_t::set_viewport (vp);
    COND_THROW((vp.x == vp.z) || (vp.y == vp.w),
               "Bad viewport spefified: " << vp);
    m_arcBall.set_frame (glm::vec2(vp.z - vp.x, vp.w - vp.y));
    m_arcBall.set_offset (glm::vec2(vp.x, vp.y));
    m_view.set_viewport (vp);
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
						m_view.camera().set_translation (p);
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
}


void ArcBallView::mouse_move (const glm::vec2& c)
{
	glm::vec2 lastCursorPos = base_t::cursor_position();
	base_t::mouse_move (c);

	if(m_arcBall.dragging()){
		m_arcBall.drag_to (c);
		m_view.camera().set_rotation (m_arcBall.rotation_quaternion());
	}
	else {
		if(base_t::mouse_button_is_down (MouseButton::RIGHT)) {
			glm::vec2 d = base_t::cursor_position() - lastCursorPos;
			glm::vec3 fp = m_view.project (m_view.camera().to());
			fp += glm::vec3(d.x, d.y, 0);
			glm::vec3 nf = m_view.unproject (fp);

			//	due to rounding issues, we have to make sure that the resulting direction
			//	is parallel to the camera plane.
			glm::vec3 nfDir = nf - m_view.camera().from();
			if (rayPlaneIntersection (nf,
			                          m_view.camera().from(),
			                          nfDir,
			                          m_view.camera().to(),
			                          m_view.camera().forward()))
			{
				m_view.camera().translate (m_view.camera().to() - nf);
			}
		}
		// static const float translationSpeed = 0.003f;
		// if(base_t::mouse_button_is_down (MouseButton::RIGHT)) {
		// 	glm::vec2 d = base_t::cursor_position() - lastCursorPos;
		// 	d *= translationSpeed;
		// 	if (m_view.camera().scale().x != 0){
		// 		d *= m_view.camera().scale().x;
		// 		m_view.camera().translate (-d.x * m_view.camera().right () + d.y * m_view.camera().up ());
		// 	}
		// }
	}
}


void ArcBallView::mouse_scroll (const glm::vec2& o)
{
	base_t::mouse_scroll (o);
	float s = 1.f - o.y / 10.f;
	m_view.camera().scale (glm::vec3(s));
}

}// end of namespace lumeview


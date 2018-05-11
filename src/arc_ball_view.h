#ifndef __H__arc_ball_view
#define __H__arc_ball_view

#include "arc_ball.h"
#include "camera.h"
#include "input_listener.h"
#include "view.h"

class ArcBallView : public InputListener{
public:
	ArcBallView ();
	const View& view () const;

  	void set_viewport (const glm::ivec4& vp);
  	const glm::ivec4& viewport () const;
	
	void mouse_button (int button, int action, int mods) override;
	void mouse_move (const glm::vec2& c) override;
	void mouse_scroll (const glm::vec2& o) override;

private:
	using base_t = InputListener;
	
	View 		m_view;
  	ArcBall 	m_arcBall;
  	Camera 		m_camera;
};

#endif	//__H__arc_ball_view

// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_lumeview
#define __H__lumeview_lumeview

#include "arc_ball_view.h"
#include "scene.h"
#include "window_event_listener.h"

namespace lumeview {

void LumeviewInit ();

void LumeviewShutdown ();


class Lumeview : public WindowEventListener {
public:

	Lumeview ();

	void clear();
	
	// OVERRIDES FOR WindowEventListener
	void mouse_button (int button, int action, int mods) override;
  	void mouse_move (const glm::vec2& c) override;
  	void mouse_scroll (const glm::vec2& o) override;

  	void set_viewport (const glm::ivec4& vp) override;

  	void key (int key, int scancode, int action, int mods) override;
  	void character (unsigned int c) override;


  	void add_scene (SPScene scene);

  	void process_gui ();

  	void render ();

  	void add_sample_scene ();
  	
private:
	using base_t = WindowEventListener;

	WindowEventListener* m_imguiListener;
	ArcBallView			 m_arcBallView;

	std::vector <SPScene>	m_scenes;
	SPScene					m_activeScene;

	bool	m_guiShowScene;
	bool	m_guiShowLog;
	bool	m_guiShowDemo;
};

}//	end of namespace lumeview

#endif	//__H__lumeview_lumeview

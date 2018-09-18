// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_scene
#define __H__lumeview_scene

#include <vector>
#include <memory>
#include "lume/mesh.h"
#include "view.h"
#include "visualization.h"

namespace lumeview {

class Scene {
public:
	void add_entry (lume::SPMesh mesh, SPVisualization vis);
	void render (const View& view);
	void do_imgui (bool* pOpened = NULL);

	///	returns min (x) and max (y) z clip distances required to show all polygons.
	glm::vec2 estimate_z_clip_dists (const View& view) const;

private:
	struct Entry {
		Entry ()	{}
		Entry (lume::SPMesh _mesh, SPVisualization _vis) : mesh (_mesh), vis (_vis) {}
		
		lume::SPMesh mesh;
		SPVisualization vis;
	};

	std::vector <Entry>			m_entries;
	static const std::string	s_strVisualization;
};

using SPScene = std::shared_ptr <Scene>;

}//	end of namespace lumeview

#endif	//__H__lumeview_scene

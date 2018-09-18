// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_visualization
#define __H__lumeview_visualization

#include "view.h"

namespace lumeview {

class Visualization {
public:
	virtual void render (const View&) = 0;
	
	///	returns min (x) and max (y) z clip distances required to show all polygons.
	virtual glm::vec2 estimate_z_clip_dists (const View& view) const = 0;
};

using SPVisualization = std::shared_ptr <Visualization>;

}//	end of namespace lumeview

#endif	//__H__lumeview_visualization

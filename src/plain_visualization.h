// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_plain_visualization
#define __H__lumeview_plain_visualization

#include "lume/mesh.h"
#include "renderer.h"
#include "visualization.h"


namespace lumeview {

class PlainVisualization : public Visualization
{
public:
	PlainVisualization ();
	void set_mesh (lume::SPMesh mesh);

	void refresh ();

	void render (const View& view) override;

	glm::vec2 estimate_z_clip_dists (const View& view) const override;

private:
	Renderer					m_renderer;
	lume::SPMesh				m_mesh;
};
	
}//	end of namespace lumeview

#endif	//__H__lumeview_plain_visualization

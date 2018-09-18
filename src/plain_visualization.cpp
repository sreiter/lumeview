// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include "plain_visualization.h"
#include "lume/rim_mesh.h"
#include "lume/normals.h"
#include "lume/topology.h"

using namespace lume;
using namespace std;

namespace lumeview {
	
PlainVisualization::
PlainVisualization ()
{
}

void PlainVisualization::
set_mesh (lume::SPMesh mesh)
{
	m_mesh = mesh;
	refresh();
}

void PlainVisualization::
refresh ()
{
	m_renderer.clear();

	const glm::vec4 solidColor (1.0f, 0.843f, 0.f, 1.0f);
	const glm::vec4 wireColor (0.2f, 0.2f, 0.2f, 1.0f);
	const glm::vec4 bndColor (1.0f, 0.2f, 0.2f, 1.0f);

	if (m_mesh->has (CELLS)) {
		auto bndMesh = CreateRimMesh (m_mesh, CELLS);
		ComputeFaceVertexNormals3 (*bndMesh, "normals");
		CreateEdgeInds (*bndMesh);
		m_renderer.add_stage ("solid", bndMesh, FACES, FLAT);
		m_renderer.stage_set_color (solidColor);
		m_renderer.add_stage ("wire", bndMesh, EDGES, FLAT);
		m_renderer.stage_set_color (wireColor);
	}
	else if (m_mesh->has (FACES)) {
		ComputeFaceVertexNormals3 (*m_mesh, "normals");
		CreateEdgeInds (*m_mesh);
		m_renderer.add_stage ("solid", m_mesh, FACES, FLAT);
		m_renderer.stage_set_color (solidColor);
		m_renderer.add_stage ("wire", m_mesh, EDGES, FLAT);
		m_renderer.stage_set_color (wireColor);
		auto bndMesh = CreateRimMesh (m_mesh, FACES);
		m_renderer.add_stage ("bnd", bndMesh, EDGES, NONE);
		m_renderer.stage_set_color (bndColor);
	}
	else if (m_mesh->has (EDGES)) {
		ComputeFaceVertexNormals3 (*m_mesh, "normals");
		m_renderer.add_stage ("wire", m_mesh, EDGES, NONE);
		m_renderer.stage_set_color (wireColor);
	}
}

void PlainVisualization::
render (const View& view)
{
	m_renderer.render (view);
}

glm::vec2 PlainVisualization::
estimate_z_clip_dists (const View& view) const
{
	return m_renderer.estimate_z_clip_dists (view);
}


}//	end of namespace lumeview

// This file is part of lumeview, a lightweight viewer for unstructured meshes
//
// Copyright (C) 2018 Sebastian Reiter
// Copyright (C) 2018 G-CSC, Goethe University Frankfurt
// Author: Sebastian Reiter <s.b.reiter@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

PlainVisualization::
PlainVisualization (const lume::SPMesh& mesh)
{
	set_mesh (mesh);
}

void PlainVisualization::
set_mesh (const lume::SPMesh& mesh)
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
		CreateSideGrobs (*bndMesh, 1);
		m_renderer.add_stage ("solid", bndMesh, FACES, FLAT);
		m_renderer.stage_set_color (solidColor);
		m_renderer.add_stage ("wire", bndMesh, EDGES, FLAT);
		m_renderer.stage_set_color (wireColor);
	}
	else if (m_mesh->has (FACES)) {
		ComputeFaceVertexNormals3 (*m_mesh, "normals");
		CreateSideGrobs (*m_mesh, 1);
		m_renderer.add_stage ("solid", m_mesh, FACES, FLAT);
		m_renderer.stage_set_color (solidColor);
		m_renderer.add_stage ("wire", m_mesh, EDGES, FLAT);
		m_renderer.stage_set_color (wireColor);
		auto bndMesh = CreateRimMesh (m_mesh, FACES);
		if (bndMesh->has (EDGES)) {
			m_renderer.add_stage ("bnd", bndMesh, EDGES, NONE);
			m_renderer.stage_set_color (bndColor);
		}
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

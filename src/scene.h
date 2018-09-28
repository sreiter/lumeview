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

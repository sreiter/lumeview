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

#include "scene.h"
#include <algorithm>
#include <string>
#include "visualization.h"
#include "imgui/imgui.h"

using namespace lume;
using namespace std;

namespace lumeview {

const std::string	Scene::s_strVisualization = "visualization";


void Scene::add_entry (SPMesh mesh, SPVisualization vis)
{
	m_entries.push_back (Entry (mesh, vis));
}

void Scene::render (const View& view)
{
	for(auto entry : m_entries) {
		if (entry.vis) {
			entry.vis->render (view);
		}
	}
}

void Scene::do_imgui (bool* pOpened)
{
	ImGui::SetNextWindowSize(ImVec2(300,500), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Scene", pOpened, 0)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

	for(auto& entry : m_entries) {
		ImGui::PushID (entry.mesh.get());
		if (ImGui::CollapsingHeader("entry")) {
			// if (entry.vis && entry.vis->has_imgui()) {
			// 	entry.vis->do_imgui();
			// }

			for (auto annexIter = entry.mesh->annex_begin();
			     annexIter != entry.mesh->annex_end(); ++annexIter)
			{
				if (annexIter->second->has_imgui()) {
					if (annexIter != entry.mesh->annex_begin())
						ImGui::Separator();
					
					string label = string (annexIter->second->class_name()) + ": " + annexIter->first.name.c_str();
					if (ImGui::TreeNode (label.c_str()))
					{
						annexIter->second->do_imgui();
						ImGui::TreePop();
					}
				}
			}
		}
		ImGui::PopID();
	}

    ImGui::End(); // Scene
}

///	returns min (x) and max (y) z clip distances required to show all polygons.
glm::vec2 Scene::estimate_z_clip_dists (const View& view) const
{
	glm::vec2 cdOut (0, 0);
	bool first = true;
	for(auto entry : m_entries) {
		if (entry.vis) {
			const glm::vec2 cd = entry.vis->estimate_z_clip_dists(view);
			if (first) {
				cdOut = cd;
				first = false;
			}
			else {
				cdOut.x = min (cdOut.x, cd.x);
				cdOut.y = min (cdOut.y, cd.y);
			}
		}
	}
	return cdOut;
}

}//	end of namespace lumeview

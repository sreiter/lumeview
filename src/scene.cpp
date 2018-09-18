// This file is part of lumeview, a C++ entry library
//
// Copyright (C) 2018 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

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

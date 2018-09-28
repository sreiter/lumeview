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

#ifndef __H__lumeview_subset_info_annex_imgui
#define __H__lumeview_subset_info_annex_imgui

#include "lume/subset_info_annex.h"
#include "imgui/imgui.h"
#include "message_queue.h"
#include "subset_info_annex_message.h"

namespace lumeview {

inline void SubsetInfoAnnex_ImGui (lume::SubsetInfoAnnex* sia)
{
	using namespace lume;

	const index_t numSubs = sia->num_subset_properties ();

	
	if (numSubs > 0) {
		ImGui::NewLine ();
		ImGui::SameLine (60);
		ImGui::Text ("name");
		ImGui::SameLine (160);
		ImGui::Text ("color");
		ImGui::SameLine (210);
		ImGui::Text ("visible");
		ImGui::Separator();
	}

	for(index_t isub = 0; isub < numSubs; ++isub) {
		SubsetInfoAnnex::SubsetProperties& prop = sia->subset_properties (isub);
		auto oldColor = prop.color;
		bool oldVisible = prop.visible;

		ImGui::PushID(&prop);
		ImGui::AlignTextToFramePadding();
	    ImGui::Text("%d:", isub);
	    ImGui::SameLine(60);
	    ImGui::Text(prop.name.c_str());
		ImGui::SameLine(160);
		ImGui::ColorEdit4("", prop.color.data_ptr(), ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(210);
		ImGui::Checkbox ("", &prop.visible);

		if (isub + 1 != numSubs)
			ImGui::Separator();

		ImGui::PopID();

		if (prop.color != oldColor || prop.visible != oldVisible) {
			MessageQueue::post <SubsetInfoAnnexMessage> (
					sia,
					isub,
					prop.color != oldColor,
					prop.visible != oldVisible);
		}
	}
}

}//	end of namespace lumeview

#endif	//__H__lumeview_subset_info_annex_imgui

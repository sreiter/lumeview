// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

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

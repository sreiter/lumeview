// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__slimesh_subset_info_attachment_imgui
#define __H__slimesh_subset_info_attachment_imgui

#include "subset_info_attachment.h"

namespace slimesh {

void SubsetInfoAttachment_ImGui	(std::vector<SubsetInfo::Properties>& properties)
{
	if (!properties.empty()) {
		ImGui::NewLine ();
		ImGui::SameLine (60);
		ImGui::Text ("name");
		ImGui::SameLine (160);
		ImGui::Text ("color");
		ImGui::SameLine (210);
		ImGui::Text ("visible");
		ImGui::Separator();
	}

	for(size_t iprop = 0; iprop < properties.size(); ++iprop) {
		SubsetInfo::Properties& prop = properties [iprop];
		ImGui::PushID(&prop);
		ImGui::AlignTextToFramePadding();
	    ImGui::Text("%d:", iprop);
	    ImGui::SameLine(60);
	    ImGui::Text(prop.name.c_str());
		ImGui::SameLine(160);
		ImGui::ColorEdit4("", glm::value_ptr(prop.color), ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(210);
		ImGui::Checkbox ("", &prop.visible);

		if (iprop + 1 != properties.size())
			ImGui::Separator();

		ImGui::PopID();
	}
}

}//	end of namespace slimesh

#endif	//__H__slimesh_subset_info_attachment_imgui

// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include "subset_info_attachment.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

using namespace std;

namespace slimesh {

SubsetInfo::
SubsetInfo(const std::string& name) :
	m_name (name)
{}

SubsetInfo::
~SubsetInfo()
{}

void SubsetInfo::
set_name (const std::string& name)
{
	m_name = name;
}

const std::string& SubsetInfo::
name () const
{
	return m_name;
}

void SubsetInfo::
add_subset (const Properties& p)
{
	m_properties.push_back (p);
}

void SubsetInfo::
add_subset (Properties&& p)
{
	m_properties.push_back (std::move (p));
}


void SubsetInfo::
set_subset (const size_t i, const Properties& p)
{
	if (m_properties.size() <= i)
		m_properties.resize (i+1);
	m_properties [i] = p;
}

void SubsetInfo::
set_subset (const size_t i, Properties&& p)
{
	if (m_properties.size() <= i)
		m_properties.resize (i+1);
	m_properties [i] = std::move (p);
}

void SubsetInfo::
do_imgui ()
{
	if (!m_properties.empty()) {
		ImGui::NewLine ();
		ImGui::SameLine (60);
		ImGui::Text ("name");
		ImGui::SameLine (160);
		ImGui::Text ("color");
		ImGui::SameLine (210);
		ImGui::Text ("visible");
		ImGui::Separator();
	}

	for(size_t iprop = 0; iprop < m_properties.size(); ++iprop) {
		Properties& prop = m_properties [iprop];
		ImGui::PushID(&prop);
		ImGui::AlignTextToFramePadding();
	    ImGui::Text("%d:", iprop);
	    ImGui::SameLine(60);
	    ImGui::Text(prop.name.c_str());
		ImGui::SameLine(160);
		ImGui::ColorEdit4("", glm::value_ptr(prop.color), ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine(210);
		ImGui::Checkbox ("", &prop.visible);

		if (iprop + 1 != m_properties.size())
			ImGui::Separator();

		ImGui::PopID();
	}
}
	
}//	end of namespace slimesh

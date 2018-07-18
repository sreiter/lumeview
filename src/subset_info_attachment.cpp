// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include "subset_info_attachment.h"

using namespace std;

namespace slimesh {

SubsetInfo::ImGuiExecutor SubsetInfo::s_imguiExecutor = nullptr;

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
	
}//	end of namespace slimesh

// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__slimesh_subset_info_attachment
#define __H__slimesh_subset_info_attachment

#include <string>
#include <vector>
#include "mesh.h"
#include <glm/vec4.hpp>

namespace slimesh {

class SubsetInfo : public MeshData
{
public:
	struct Properties {
		Properties () : name ("_default_"), color (1.f, 1.f, 1.f, 1.f), visible (true) {}
		Properties (const std::string& _name, const glm::vec4& _color, const bool _visible) :
			name (_name),
			color (_color),
			visible (_visible)
		{}

		std::string		name;
		glm::vec4		color;
		bool			visible;
	};


	SubsetInfo(const std::string& name);
	virtual ~SubsetInfo();

	const char* class_name () const override	{return "SubsetInfo";}
		
	void set_name (const std::string& name);
	const std::string& name () const;

	void add_subset (const Properties& p);
	void add_subset (Properties&& p);

	void set_subset (const size_t i, const Properties& p);

	void set_subset (const size_t i, Properties&& p);

	void do_imgui () override;
	bool has_gui () const override	{return true;}

private:
	std::string					m_name;
	std::vector <Properties>	m_properties;
};

using SPSubsetInfo = std::shared_ptr <SubsetInfo>;
using CSPSubsetInfo = std::shared_ptr <const SubsetInfo>;

}//	end of namespace slimesh

#endif	//__H__slimesh_subset_info_attachment

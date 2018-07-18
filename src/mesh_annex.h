// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__slimesh_mesh_annex
#define __H__slimesh_mesh_annex

#include <memory>

namespace slimesh {

class MeshAnnex {
public:
	virtual ~MeshAnnex () {};
	virtual const char* class_name () const = 0;
	virtual void do_imgui () {};
	virtual bool has_gui () const	{return false;}
};

using SPMeshAnnex	= std::shared_ptr <MeshAnnex>;
using CSPMeshAnnex	= std::shared_ptr <const MeshAnnex>;

}//	end of namespace slimesh

#endif	//__H__slimesh_mesh_annex

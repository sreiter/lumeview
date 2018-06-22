// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__slimesh_mesh_data
#define __H__slimesh_mesh_data

#include <memory>

namespace slimesh {

class MeshData {
public:
	virtual ~MeshData () {};
	virtual const char* class_name () const = 0;
	virtual void do_imgui () {};
	virtual bool has_gui () const	{return false;}
};

using SPMeshData	= std::shared_ptr <MeshData>;
using CSPMeshData	= std::shared_ptr <const MeshData>;

}//	end of namespace slimesh

#endif	//__H__slimesh_mesh_data

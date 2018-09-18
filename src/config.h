// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_config
#define __H__lumeview_config

#include <string>
#include "lume/types.h"

namespace lumeview {
	using lume::uint;
	using lume::index_t;
	using lume::real_t;

	extern const std::string SHADER_PATH;
	extern const std::string MESH_PATH;
}// end of namespace

#endif	//__H__lumeview_config

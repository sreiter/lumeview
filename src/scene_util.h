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

#ifndef __H__lumeview_scene_util
#define __H__lumeview_scene_util

#include "scene.h"
#include "lume/file_io.h"
#include "lume/mesh.h"

namespace lumeview {

///	Creates a scene and adds the given mesh with the specified visualization
template <class TVisualization>
SPScene CreateSceneForMesh (const lume::SPMesh& mesh)
{
	auto scene = std::make_shared <Scene> ();
	scene->add_entry (mesh, std::make_shared <TVisualization> (mesh));
	return scene;
}

/// Creates a scene and adds the given mesh with the best matching visualization
SPScene CreateSceneForMesh (const lume::SPMesh& mesh);

/// Creates a scene and adds the given mesh from file with the specified visualization
template <class TVisualization>
SPScene CreateSceneForMesh (const std::string& filename)
{
	auto mesh = lume::CreateMeshFromFile (filename);
	return CreateSceneForMesh <TVisualization> (mesh);
}

/// Creates a scene and adds the given mesh from file with the best matching visualization
SPScene CreateSceneForMesh (const std::string& filename);

///	Creates a scene with a predefined mesh and visualization. Useful mainly for debugging and testing.
SPScene CreateSampleScene ();

}//	end of namespace lumeview

#endif	//__H__lumeview_scene_util

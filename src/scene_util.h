// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

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

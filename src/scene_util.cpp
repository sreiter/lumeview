// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include "plain_visualization.h"
#include "scene_util.h"
#include "subset_visualization.h"

namespace lumeview {

SPScene CreateSceneForMesh (const lume::SPMesh& mesh)
{
	try {
		return CreateSceneForMesh <SubsetVisualization> (mesh);
	}
	catch (...) {	
		return CreateSceneForMesh <PlainVisualization> (mesh);
	}
}

SPScene CreateSceneForMesh (const std::string& filename)
{
	auto mesh = lume::CreateMeshFromFile (filename);
	return CreateSceneForMesh (mesh);
}


SPScene CreateSampleScene ()
{
	// const std::string filename = MESH_PATH + "tet.ugx";
	const std::string filename = MESH_PATH + "elems_refined.ugx";
	// const std::string filename = MESH_PATH + "2-spheres-tets.ugx";
	// const std::string filename = MESH_PATH + "bbox_jet.1.ugx";
	// const std::string filename = MESH_PATH + "tri_and_quad.ugx";
	// const std::string filename = MESH_PATH + "bunny.stl";
	// const std::string filename = MESH_PATH + "circle_with_subsets.ugx";


	return CreateSceneForMesh (filename);
}

}//	end of namespace lumeview

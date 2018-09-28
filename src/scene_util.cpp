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

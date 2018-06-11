#include <string>
#include <algorithm>
#include "mesh.h"
#include "cond_throw.h"
#include "stl_reader/stl_reader.h"

using namespace std;

namespace slimesh {

std::shared_ptr <Mesh> CreateMeshFromSTL (std::string filename)
{
	auto mesh = make_shared <Mesh> ();
	std::vector <real_t> normals;
	std::vector <index_t> solids;

	stl_reader::ReadStlFile (filename.c_str(),
	                         mesh->coords()->data(),
							 normals,
							 mesh->inds(TRI)->data(),
							 solids);

	mesh->coords()->set_tuple_size(3);
	return mesh;
}


std::shared_ptr <Mesh> CreateMeshFromELE(std::string filename)
{
//	build the correct filenames
	string nodesFilename, facesFilename;
	COND_THROW (filename.find(".ele", filename.size() - 4) == string::npos,
	            "CreateMeshFromELE: File not found: " << filename);

//	the filename ends as we would expect
	nodesFilename = facesFilename = filename.substr(0, filename.size() - 4);
	nodesFilename.append(".node");
	facesFilename.append(".face");

	auto mesh = make_shared <Mesh> ();

//	read nodes and store them in an array for index access
	mesh->coords()->set_tuple_size (3);
	vector <real_t>& vertices = mesh->coords()->data();

	{
		ifstream in(nodesFilename);
		COND_THROW (!in, "CreateMeshFromELE: Required node file not found: " << nodesFilename);

		uint numNodes, dim, numAttribs, numBoundaryMarkers;
		in >> numNodes;
		in >> dim;
		in >> numAttribs;
		in >> numBoundaryMarkers;

		vertices.reserve(numNodes + 1);

	//	read the vertices
		int index;
		for(uint i = 0; i < numNodes; ++i)
		{
		//	read index and coords
			in >> index;
			if(index > (int) vertices.size())
				vertices.resize(index * 3, 0);

			for(uint j = 0; j < 3; ++j) {
				real_t coord;
				in >> coord;
				vertices.push_back (coord);
			}

		//	todo: apply attributes
			if(numAttribs > 0) {
				for(uint j = 0; j < numAttribs; ++j) {
					float tmp;
					in >> tmp;
				}
			}

		//	todo: apply boundary marker
			if(numBoundaryMarkers > 0) {
				int bm;
				in >> bm;
			}
		}
	}

//	todo: read faces
	// {
	// 	ifstream in(facesFilename);
	// 	if(in)
	// 	{
	// 		int numFaces, numBoundaryMarkers;
	// 		in >> numFaces;
	// 		in >> numBoundaryMarkers;

	// 		Grid::FaceAttachmentAccessor<AInt> aaBMFACE;
	// 		if(paFaceBoundaryMarker != NULL)
	// 			aaBMFACE.access(grid, *paFaceBoundaryMarker);

	// 		for(int i = 0; i < numFaces; ++i)
	// 		{
	// 			int index, i1, i2, i3;
	// 			in >> index;
	// 			in >> i1;
	// 			in >> i2;
	// 			in >> i3;

	// 			Triangle* t = *grid.create<Triangle>(TriangleDescriptor(vVertices[i1], vVertices[i2], vVertices[i3]));



	// 			if(numBoundaryMarkers > 0)
	// 			{
	// 				int bm;
	// 				in >> bm;
	// 				if(aaBMFACE.valid())
	// 					aaBMFACE[t] = bm;
	// 			}
	// 			else
	// 			{
	// 				if(aaBMFACE.valid())
	// 					aaBMFACE[t] = -1;
	// 			}
	// 		}
	// 	}

	// 	in.close();
	// }

//	read volumes
	{
		ifstream in(filename);
		COND_THROW (!in, "CreateMeshFromELE: Required elements file not found: " << filename);
		int numTets, numNodesPerTet, numAttribs;
		in >> numTets;
		in >> numNodesPerTet;
		in >> numAttribs;

		COND_THROW (numNodesPerTet != 4, "CreateMeshFromELE: Unsupported number "
		            "of tetrahedron corners in file '" << filename << "': " << numNodesPerTet);
		
		vector <index_t>& tets = mesh->inds (TET)->data();
		tets.resize (numTets * numNodesPerTet);

		for(int i = 0; i < numTets; ++i)
		{
			int index;
			in >> index;
			for(int j = 0; j < numNodesPerTet; ++j)
				in >> tets[i * numNodesPerTet + j];

			//	todo: apply attributes
			if(numAttribs > 0) {
				int a;
				in >> a;
			}
		}
	}

	return mesh;
}


std::shared_ptr <Mesh> CreateMeshFromFile (std::string filename)
{
	string suffix = filename.substr(filename.size() - 4, 4);
	transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

	if (suffix == ".stl")
		return CreateMeshFromSTL (filename);

	else if (suffix == ".ele" )
		return CreateMeshFromELE (filename);

	else {
		THROW("CreateMeshFromFile: Unsupported file format: '" << suffix << "'");
	}
	return SPMesh ();
}

}// end of namespace slimesh

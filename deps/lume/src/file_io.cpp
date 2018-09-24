// This file is part of lume, a C++ library for lightweight unstructured meshes
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


#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include "lume/annex_table.h"
#include "lume/file_io.h"
#include "lume/subset_info_annex.h"
#include "lume/vec_math_raw.h"
#include "lume/topology.h"

#include "stl_reader/stl_reader.h"
#include "rapidxml/rapidxml.hpp"

// disables warnings generated for strtok through MSVC
#pragma warning(disable:4996)

using namespace std;
using namespace rapidxml;

namespace lume {

std::shared_ptr <Mesh> CreateMeshFromSTL (std::string filename)
{
	auto mesh = make_shared <Mesh> ();
	ArrayAnnex <real_t> normals;
	ArrayAnnex <index_t> solids;

	stl_reader::ReadStlFile (filename.c_str(),
	                         *mesh->coords(),
							 normals,
							 mesh->grobs(TRI).underlying_array(),
							 solids);

	mesh->coords()->set_tuple_size(3);
	return mesh;
}


std::shared_ptr <Mesh> CreateMeshFromELE(std::string filename)
{
//	build the correct filenames
	string nodesFilename, facesFilename;
	if (filename.find(".ele", filename.size() - 4) == string::npos)
	    throw FileSuffixError (filename);

//	the filename ends as we would expect
	nodesFilename = facesFilename = filename.substr(0, filename.size() - 4);
	nodesFilename.append(".node");
	facesFilename.append(".face");

	auto mesh = make_shared <Mesh> ();

//	read nodes and store them in an array for index access
	auto& coords = *mesh->coords();
	coords.set_tuple_size (3);

	{
		ifstream in(nodesFilename);
		if (!in) throw FileNotFoundError (nodesFilename);

		uint numNodes, dim, numAttribs, numBoundaryMarkers;
		in >> numNodes;
		in >> dim;
		in >> numAttribs;
		in >> numBoundaryMarkers;

		coords.reserve((numNodes + 1) * 3);

	//	read the coords
		int index;
		for(uint i = 0; i < numNodes; ++i)
		{
		//	read index and coords
			in >> index;
			if(index > (int) coords.size())
				coords.resize(index * 3, 0);

			for(uint j = 0; j < 3; ++j) {
				real_t coord;
				in >> coord;
				coords.push_back (coord);
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
		if (!in)
			throw FileNotFoundError (filename);
		int numTets, numNodesPerTet, numAttribs;
		in >> numTets;
		in >> numNodesPerTet;
		in >> numAttribs;

		if (numNodesPerTet != GrobDesc (TET).num_corners ())
			throw FileParseError (string ("Bad number of nodes in tetrahedron in ") + filename);
		
		auto& tets = mesh->grobs (TET).underlying_array ();
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

static void ReadIndicesToArrayAnnex (IndexArrayAnnex& indsOut, xml_node<>* node)
{
	char* p = strtok (node->value(), " ");
	while (p) {
		indsOut.push_back (atoi(p));
		p = strtok (nullptr, " ");
	}
}

static SubsetInfoAnnex::Color ParseColor (char* colStr)
{
	char* p = strtok (colStr, " ");
	int i = 0;
	SubsetInfoAnnex::Color col (1.f);
	while (p && i < 4) {
		col [i] = real_t (atof(p));
		p = strtok (nullptr, " ");
		++i;
	}
	return col;
}


// ugx uses a different order of 3d elements
static std::vector <grob_t>
UGXGrobTypeArrayFromGrobSet (const GrobSet& gs)
{
	switch (gs.dim()) {
		case 0: return {VERTEX};
		case 1: return {EDGE};
		case 2: return {TRI, QUAD};
		case 3: return {TET, HEX, PRISM, PYRA};
		default: throw LumeError ("UGXGrobTypeArrayFromGrobSet: Unsupported grob set dimension");
	}
}

template <class T>
static void ParseElementIndicesToArrayAnnex (SPMesh& mesh,
                                            const string& annexName,
                                            xml_node<>* node,
                                            const T value,
                                            const GrobSet& gs)
{
	if (!node) return;
	
	// indices in the node are referring to all elements of one dimension.
	// we have to map them to indices of individual grob types.
	TotalToGrobIndexMap indMap (*mesh, UGXGrobTypeArrayFromGrobSet (gs));

	ArrayAnnexTable <ArrayAnnex<T>> annexTable (mesh, annexName, gs, true);
	annexTable.resize_annexes_to_match_grobs (1);

	// parse the node values and assign indices
	char* p = strtok (node->value(), " ");
	while (p) {
		const auto ig = indMap (index_t (atoi(p)));
		annexTable [ig] = value;
		p = strtok (nullptr, " ");
	}
}

template <class T>
static void ParseElementIndicesToArrayAnnex (SPMesh& mesh,
                                            const string& annexName,
                                            xml_node<>* node,
                                            const T value)
{
	ParseElementIndicesToArrayAnnex (mesh, annexName, node->first_node ("vertices"), value, VERTICES);
	ParseElementIndicesToArrayAnnex (mesh, annexName, node->first_node ("edges"), value, EDGES);
	ParseElementIndicesToArrayAnnex (mesh, annexName, node->first_node ("faces"), value, FACES);
	ParseElementIndicesToArrayAnnex (mesh, annexName, node->first_node ("volumes"), value, CELLS);
}


std::shared_ptr <Mesh> CreateMeshFromUGX (std::string filename)
{
	xml_document<> doc;
	char* fileContent = nullptr;

	{
		ifstream in(filename, ios::binary);
		if (!in)
			throw FileNotFoundError (filename);

	//	get the length of the file
		streampos posStart = in.tellg();
		in.seekg(0, ios_base::end);
		streampos posEnd = in.tellg();
		streamsize size = posEnd - posStart;

	//	go back to the start of the file
		in.seekg(posStart);

	//	read the whole file en-block and terminate it with 0
		fileContent = doc.allocate_string(0, size + 1);
		in.read(fileContent, size);
		fileContent[size] = 0;
		in.close();
	}

	doc.parse<0>(fileContent);

	xml_node<>* gridNode = doc.first_node("grid");
	if (!gridNode)
		throw FileParseError (string ("no grid found in ") + filename);

	auto mesh = make_shared <Mesh> ();
	auto& coords = *mesh->coords();

	int lastNumSrcCoords = -1;
	xml_node<>* curNode = gridNode->first_node();
	for(;curNode; curNode = curNode->next_sibling()) {
		const char* name = curNode->name();

		if(strcmp(name, "vertices") == 0 || strcmp(name, "constrained_vertices") == 0)
		{
			int numSrcCoords = -1;
			xml_attribute<>* attrib = curNode->first_attribute("coords");
			if(attrib)
				numSrcCoords = atoi(attrib->value());

			if (numSrcCoords < 1)
				throw FileParseError (string ("Not enough coordinates provided in ") + filename);

			if (lastNumSrcCoords >= 0 && lastNumSrcCoords != numSrcCoords)
				throw FileParseError (string ("Can't read vertices with differing numbers "
			            "of coordinates from ") + filename);

			lastNumSrcCoords = numSrcCoords;
			coords.set_tuple_size (numSrcCoords);
			
		//	create a buffer with which we can access the data
			char* p = strtok (curNode->value(), " ");
			while (p) {
			//	read the data
				coords.push_back (real_t (atof(p)));
				p = strtok (nullptr, " ");
			}
		}

		else if(strcmp(name, "edges") == 0
		        || strcmp(name, "constraining_edges") == 0
		        || strcmp(name, "constrained_edges") == 0)
		{
			ReadIndicesToArrayAnnex (mesh->grobs (EDGE).underlying_array (), curNode);
		}

		else if(strcmp(name, "triangles") == 0
		        || strcmp(name, "constraining_triangles") == 0
		        || strcmp(name, "constrained_triangles") == 0)
		{
			ReadIndicesToArrayAnnex (mesh->grobs (TRI).underlying_array (), curNode);
		}

		else if(strcmp(name, "quadrilaterals") == 0
		        || strcmp(name, "constraining_quadrilaterals") == 0
		        || strcmp(name, "constrained_quadrilaterals") == 0)
		{
			ReadIndicesToArrayAnnex (mesh->grobs (QUAD).underlying_array (), curNode);
		}

		else if(strcmp(name, "tetrahedrons") == 0)
			ReadIndicesToArrayAnnex (mesh->grobs (TET).underlying_array (), curNode);

		else if(strcmp(name, "hexahedrons") == 0)
			ReadIndicesToArrayAnnex (mesh->grobs (HEX).underlying_array (), curNode);

		else if(strcmp(name, "pyramids") == 0)
			ReadIndicesToArrayAnnex (mesh->grobs (PYRA).underlying_array (), curNode);

		else if(strcmp(name, "prisms") == 0)
			ReadIndicesToArrayAnnex (mesh->grobs (PRISM).underlying_array (), curNode);

		// else if(strcmp(name, "octahedrons") == 0)
		// 	bSuccess = create_octahedrons(volumes, grid, curNode, vertices);

		else if(strcmp(name, "subset_handler") == 0) {
		//	make sure that vertex indices are present
			impl::GenerateVertexIndicesFromCoords (*mesh);

			string siName = "subsetHandler";
			if (xml_attribute<>* attrib = curNode->first_attribute("name"))
				siName = attrib->value();

			SPSubsetInfoAnnex subsetInfo = make_shared <SubsetInfoAnnex> (siName);
			subsetInfo->add_subset (SubsetInfoAnnex::SubsetProperties ());

			xml_node<>* subsetNode = curNode->first_node("subset");
			index_t subsetIndex = 1;
			for(;subsetNode; subsetNode = subsetNode->next_sibling()) {
				SubsetInfoAnnex::SubsetProperties props;
				if (xml_attribute<>* attrib = subsetNode->first_attribute("name"))
					props.name = attrib->value();
				if (xml_attribute<>* attrib = subsetNode->first_attribute("color"))
					props.color = ParseColor (attrib->value());

				ParseElementIndicesToArrayAnnex (mesh, siName, subsetNode, subsetIndex);

				subsetInfo->add_subset (std::move (props));
				++subsetIndex;
			}

			mesh->set_annex (siName, NO_GROB, subsetInfo);
		}

		// else if(strcmp(name, "vertex_attachment") == 0)
		// 	bSuccess = read_attachment<Vertex>(grid, curNode);
		// else if(strcmp(name, "edge_attachment") == 0)
		// 	bSuccess = read_attachment<Edge>(grid, curNode);
		// else if(strcmp(name, "face_attachment") == 0)
		// 	bSuccess = read_attachment<Face>(grid, curNode);
		// else if(strcmp(name, "volume_attachment") == 0)
		// 	bSuccess = read_attachment<Volume>(grid, curNode);
	}

	return mesh;
}


std::shared_ptr <Mesh> CreateMeshFromFile (std::string filename)
{
	string suffix = filename.substr(filename.size() - 4, 4);
	transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

	SPMesh mesh;
	if (suffix == ".stl")
		mesh = CreateMeshFromSTL (filename);

	else if (suffix == ".ele" )
		mesh = CreateMeshFromELE (filename);

	else if (suffix == ".ugx" )
		mesh = CreateMeshFromUGX (filename);

	else {
		throw FileSuffixError (filename);
	}

	impl::GenerateVertexIndicesFromCoords (*mesh);
	return mesh;
}

}// end of namespace lume

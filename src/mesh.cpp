#include <string>
#include <sstream>
#include <algorithm>
#include "mesh.h"
#include "cond_throw.h"
#include "subset_info_attachment.h"
#include "vec_math_raw.h"
#include "topology.h"

#include "stl_reader/stl_reader.h"
#include "rapidxml/rapidxml.hpp"

// disables warnings generated for strtok through MSVC
#pragma warning(disable:4996)

using namespace std;
using namespace rapidxml;

namespace slimesh {

std::shared_ptr <Mesh> CreateMeshFromSTL (std::string filename)
{
	auto mesh = make_shared <Mesh> ();
	std::vector <real_t> normals;
	std::vector <index_t> solids;

	stl_reader::ReadStlFile (filename.c_str(),
	                         mesh->coords()->vector(),
							 normals,
							 mesh->inds(TRI)->vector(),
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
	auto& coords = *mesh->coords();
	coords.set_tuple_size (3);

	{
		ifstream in(nodesFilename);
		COND_THROW (!in, "CreateMeshFromELE: Required node file not found: " << nodesFilename);

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
		COND_THROW (!in, "CreateMeshFromELE: Required elements file not found: " << filename);
		int numTets, numNodesPerTet, numAttribs;
		in >> numTets;
		in >> numNodesPerTet;
		in >> numAttribs;

		COND_THROW (numNodesPerTet != 4, "CreateMeshFromELE: Unsupported number "
		            "of tetrahedron corners in file '" << filename << "': " << numNodesPerTet);
		
		auto& tets = *mesh->inds (TET);
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

static glm::vec4 ParseColor (char* colStr)
{
	char* p = strtok (colStr, " ");
	int i = 0;
	glm::vec4 col (1.f);
	while (p && i < 4) {
		col [i] = real_t (atof(p));
		p = strtok (nullptr, " ");
		++i;
	}
	return col;
}


template <class T>
static void ParseElementIndicesToArrayAnnex (Mesh& mesh,
                                            const string& annexName,
                                            xml_node<>* node,
                                            const T value,
                                            const GrobSet& gs)
{
	if (!node) return;
	
	// indices in the node are referring to all elements of one dimension.
	// we have to map them to indices of individual grob types.
	TotalToGrobIndexMap indMap (mesh, gs);

	// For fast access, we store the raw pointers to data arrays of individual
	// grob types in an array
	T* rawData [NUM_GROB_TYPES];
	VecSet (rawData, NUM_GROB_TYPES, nullptr);

	// allocate data arrays and set up the rawData array
	for(auto gt : gs) {
		if (gt != VERTEX && !mesh.has (gt))
			continue;

		auto& arrayAnnex = *mesh.annex <ArrayAnnex<T>> (annexName, gt);

		if (gt == VERTEX)
			arrayAnnex.resize (mesh.coords ()->num_tuples());
		else
			arrayAnnex.resize (mesh.inds (gt)->num_tuples());

		VecSet (UNPACK_DS(arrayAnnex), 0);
		rawData [gt] = arrayAnnex.raw_ptr();
	}

	// parse the node values and assign indices
	char* p = strtok (node->value(), " ");
	while (p) {
		const auto ig = indMap (index_t (atoi(p)));
		rawData [ig.second] [ig.first] = value;
		p = strtok (nullptr, " ");
	}

}

template <class T>
static void ParseElementIndicesToArrayAnnex (Mesh& mesh,
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
		COND_THROW(!in, "CreateMeshFromUGX: Couldn't read file " << filename);

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
	COND_THROW (!gridNode, "The specified ugx file '" << filename
	            << "' does not contain a grid");

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

			COND_THROW (numSrcCoords < 1,
			            "CreateMeshFromUGX: Not enough coordinates provided in file "
			            << filename);

			COND_THROW (lastNumSrcCoords >= 0 && lastNumSrcCoords != numSrcCoords,
			            "CreateMeshFromUGX: Can't read vertices with differing numbers "
			            "of coordinates from file " << filename);

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
			ReadIndicesToArrayAnnex (*mesh->inds (EDGE), curNode);
		}

		else if(strcmp(name, "triangles") == 0
		        || strcmp(name, "constraining_triangles") == 0
		        || strcmp(name, "constrained_triangles") == 0)
		{
			ReadIndicesToArrayAnnex (*mesh->inds (TRI), curNode);
		}

		else if(strcmp(name, "quadrilaterals") == 0
		        || strcmp(name, "constraining_quadrilaterals") == 0
		        || strcmp(name, "constrained_quadrilaterals") == 0)
		{
			ReadIndicesToArrayAnnex (*mesh->inds (QUAD), curNode);
		}

		else if(strcmp(name, "tetrahedrons") == 0)
			ReadIndicesToArrayAnnex (*mesh->inds (TET), curNode);

		else if(strcmp(name, "hexahedrons") == 0)
			ReadIndicesToArrayAnnex (*mesh->inds (HEX), curNode);

		else if(strcmp(name, "pyramids") == 0)
			ReadIndicesToArrayAnnex (*mesh->inds (PYRA), curNode);

		else if(strcmp(name, "prisms") == 0)
			ReadIndicesToArrayAnnex (*mesh->inds (PRISM), curNode);

		// else if(strcmp(name, "octahedrons") == 0)
		// 	bSuccess = create_octahedrons(volumes, grid, curNode, vertices);

		else if(strcmp(name, "subset_handler") == 0) {
			string siName = "subsetHandler";
			if (xml_attribute<>* attrib = curNode->first_attribute("name"))
				siName = attrib->value();

			SPSubsetInfo subsetInfo = make_shared <SubsetInfo> (siName);
			subsetInfo->add_subset (SubsetInfo::Properties ());

			xml_node<>* subsetNode = curNode->first_node("subset");
			index_t subsetIndex = 1;
			for(;subsetNode; subsetNode = subsetNode->next_sibling()) {
				SubsetInfo::Properties props;
				if (xml_attribute<>* attrib = subsetNode->first_attribute("name"))
					props.name = attrib->value();
				if (xml_attribute<>* attrib = subsetNode->first_attribute("color"))
					props.color = ParseColor (attrib->value());

				ParseElementIndicesToArrayAnnex (*mesh, siName, subsetNode, subsetIndex);

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

	if (suffix == ".stl")
		return CreateMeshFromSTL (filename);

	else if (suffix == ".ele" )
		return CreateMeshFromELE (filename);

	else if (suffix == ".ugx" )
		return CreateMeshFromUGX (filename);

	else {
		THROW("CreateMeshFromFile: Unsupported file format: '" << suffix << "'");
	}
	return SPMesh ();
}

}// end of namespace slimesh

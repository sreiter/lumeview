#include "mesh.h"
#include "stl_reader/stl_reader.h"

using namespace std;

namespace msh {

std::shared_ptr <Mesh> CreateMeshFromFile (std::string filename)
{
	auto vrts = make_shared <CoordArray> (3);
	auto mesh = make_shared <Mesh> (msh::TRI, vrts);
	std::vector <real_t> normals;
	std::vector <index_t> solids;

	stl_reader::ReadStlFile (filename.c_str(),
	                         vrts->data(),
							 normals,
							 mesh->inds()->data(),
							 solids);

	mesh->set_grob_type (msh::TRI);
	mesh->inds()->set_tuple_size (3);
	return mesh;
}

}// end of namespace msh
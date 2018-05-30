#include "mesh.h"
#include "stl_reader/stl_reader.h"

using namespace std;

namespace slimesh {

std::shared_ptr <Mesh> CreateMeshFromFile (std::string filename)
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

}// end of namespace slimesh
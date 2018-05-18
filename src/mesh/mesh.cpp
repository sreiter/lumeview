#include "mesh.h"
#include "stl_reader/stl_reader.h"

using namespace std;

namespace msh {

std::shared_ptr <Mesh> CreateMeshFromFile (std::string filename)
{
	auto vrts = make_shared <VertexArray> (3);
	auto mesh = make_shared <Mesh> (msh::TRI, vrts);
	std::vector <real_t> normals;
	std::vector <index_t> solids;

	stl_reader::ReadStlFile (filename.c_str(),
	                         vrts->coords(),
							 normals,
							 mesh->inds(),
							 solids);

	mesh->set_grob_type (msh::TRI);
	return mesh;
}

}// end of namespace msh
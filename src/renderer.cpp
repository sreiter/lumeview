
#include <glad/glad.h>	// include before other OpenGL related includes

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "stl_reader/stl_reader.h"

#include "arc_ball_view.h"
#include "cond_throw.h"
#include "file_util.h"
#include "geometry.h"
#include "log.h"
#include "renderer.h"
#include "types.h"
#include "mesh/mesh.h"
#include "mesh/topology.h"
#include "mesh/geometry.h"
#include "visualization.h"

using namespace std;

const std::string SHADER_PATH = "shaders/";
const std::string MESH_PATH = "meshes/";

static ArcBallView g_arcBallView;
static Visualization g_visualization (SHADER_PATH);


WindowEventListener* RendererGetEventListener ()
{
	return &g_arcBallView;
}

void RendererInit ()
{
	// if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
	// 	THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	// }

	if (!gladLoadGL ()) {
		THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	}

	string meshFilename = MESH_PATH + "pyra.stl";

	auto triMesh = msh::CreateMeshFromFile (meshFilename);
	msh::ComputeVertexNormals3 (*triMesh, "vrtNormals");

	auto lineMesh = make_shared <msh::Mesh> (msh::EDGE, triMesh->coords());
	lineMesh->set_data ("vrtNormals", triMesh->data("vrtNormals"));
	msh::UniqueSidesToIndexArray (lineMesh->inds()->data(),
	                         	  triMesh->inds()->raw_data(),
								  triMesh->num_inds(),
								  msh::TRI,
								  1);

	Box box = BoxFromCoordinates (triMesh->coords()->raw_data(),
	                              triMesh->coords()->size(),
								  triMesh->coords()->tuple_size());

	LOGT(mesh, "Loaded mesh '" << meshFilename << "'\n");
	LOGT(mesh, "  #vertices:    " << triMesh->coords()->num_tuples() << std::endl);
	LOGT(mesh, "  #triangles:   " << triMesh->inds()->num_tuples() << std::endl);
	LOGT(mesh, "  #edges:       " << lineMesh->inds()->num_tuples() << std::endl);
	LOGT(mesh, "  Bounding box -> min: " << box.minCorner << std::endl);
	LOGT(mesh, "               -> max: " << box.maxCorner << std::endl);

	g_visualization.add_stage ("solid", triMesh, FLAT);
	g_visualization.add_stage ("wire", lineMesh, FLAT);

	{
		auto& normals = *triMesh->data("vrtNormals");
		const real_t* data = normals.raw_data();
		const index_t stride = normals.tuple_size();
		LOGT(mesh, "normals:\n");
		for(index_t i = 0; i < normals.size(); i+=stride) {
			LOG(i/stride << ":\t(");
			for(index_t j = 0; j < stride; ++j) {
				LOG(data[i+j]);
				if (j+1 < stride){
					LOG(", ");
				}
			}
			LOG(")\n");
		}
		LOG("\n");
	}
}


void ActivateShader (const Shader& shader)
{
	shader.use ();
	g_arcBallView.view().apply(shader);
}


void RendererDraw ()
{
	glEnable (GL_DEPTH_TEST);

	glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_visualization.render (g_arcBallView.view());
}

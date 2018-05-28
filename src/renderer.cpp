
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
#include "mesh/vec_math_raw.h"
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

msh::SPMesh LoadMeshWithEdges (std::string filename)
{
	auto mesh = msh::CreateMeshFromFile (filename);
	msh::ComputeTriVertexNormals3 (*mesh, "vrtNormals");
	CreateEdgeInds (*mesh);

	Box box = BoxFromCoordinates (mesh->coords()->raw_data(),
	                              mesh->coords()->size(),
								  mesh->coords()->tuple_size());

	LOGT(mesh, "Loaded mesh '" << filename << "'\n");
	LOGT(mesh, "  #vertices:    " << mesh->coords()->num_tuples() << std::endl);
	LOGT(mesh, "  #triangles:   " << mesh->inds(msh::TRI)->num_tuples() << std::endl);
	LOGT(mesh, "  #edges:       " << mesh->inds(msh::EDGE)->num_tuples() << std::endl);
	LOGT(mesh, "  Bounding box -> min: " << box.minCorner << std::endl);
	LOGT(mesh, "               -> max: " << box.maxCorner << std::endl);

	return mesh;
}

void RendererInit ()
{
	// if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
	// 	THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	// }

	if (!gladLoadGL ()) {
		THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	}

	auto mainMesh = LoadMeshWithEdges (MESH_PATH + "bunny.stl");
	g_visualization.add_stage ("solid", mainMesh, msh::TRI, FLAT);
	g_visualization.add_stage ("wire", mainMesh, msh::EDGE, FLAT);

	{
		auto sphereMesh = LoadMeshWithEdges (MESH_PATH + "sphere.stl");
		auto meshCoords = mainMesh->coords();
		auto sphere = SphereFromCoordinates (meshCoords->raw_data(),
		                                     meshCoords->size(),
											 meshCoords->tuple_size());

		auto sphereCoords = sphereMesh->coords();
		msh::VecScale (sphereCoords->raw_data(),
		               sphere.radius,
		               sphereCoords->size());

		msh::VecTupAppend (sphereCoords->raw_data(),
		            	   glm::value_ptr (sphere.center),
		            	   sphereCoords->size(),
		            	   sphereCoords->tuple_size());

		g_visualization.add_stage ("wireSphere", sphereMesh, msh::EDGE, FLAT);
	}

	// {
	// 	auto& normals = *mesh->data("vrtNormals");
	// 	const real_t* data = normals.raw_data();
	// 	const index_t stride = normals.tuple_size();
	// 	LOGT(mesh, "normals:\n");
	// 	for(index_t i = 0; i < normals.size(); i+=stride) {
	// 		LOG(i/stride << ":\t(");
	// 		for(index_t j = 0; j < stride; ++j) {
	// 			LOG(data[i+j]);
	// 			if (j+1 < stride){
	// 				LOG(", ");
	// 			}
	// 		}
	// 		LOG(")\n");
	// 	}
	// 	LOG("\n");
	// }
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


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

	auto triMesh = msh::CreateMeshFromFile (MESH_PATH + "sphere.stl");
	auto lineMesh = make_shared <msh::Mesh> (msh::EDGE, triMesh->vrts());
	msh::UniqueSidesToIndexArray (lineMesh->inds(),
	                         	  triMesh->raw_inds(),
								  triMesh->num_inds(),
								  msh::TRI,
								  1);

	Box box = BoxFromCoordinates (triMesh->vrts()->raw_coords(),
	                              triMesh->vrts()->num_coords(),
								  triMesh->vrts()->tuple_size());

	LOGT(mesh, "Loaded mesh '" << MESH_PATH + "sphere.stl" << "'\n");
	LOGT(mesh, "  #vertices:    " << triMesh->vrts()->num_coords() / triMesh->vrts()->tuple_size() << std::endl);
	LOGT(mesh, "  #triangles:   " << triMesh->num_inds() / 3 << std::endl);
	LOGT(mesh, "  #edges:       " << lineMesh->num_inds() / 2 << std::endl);
	LOGT(mesh, "  Bounding box -> min: " << box.minCorner << std::endl);
	LOGT(mesh, "               -> max: " << box.maxCorner << std::endl);

	g_visualization.add_stage ("solid", triMesh);
	g_visualization.add_stage ("wire", lineMesh);
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

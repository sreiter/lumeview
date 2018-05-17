
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
#include "mesh/topology.h"

#include "shader.h"

using namespace std;


struct VisMesh {
	VisMesh () : vao (0), numIndices (0), primitiveType (0) {}
	VisMesh (uint _vao, uint _numInds, uint _primType) :
		vao (_vao), numIndices (_numInds), primitiveType (_primType) {}

	uint vao;
	uint numIndices;
	uint primitiveType;
};


const std::string SHADER_PATH = "shaders/";
const std::string MESH_PATH = "meshes/";

static ArcBallView g_arcBallView;

static VisMesh g_visMesh;
static Shader g_solidShader;

static VisMesh g_wireVisMesh;
static Shader g_wireShader;


WindowEventListener* RendererGetEventListener ()
{
	return &g_arcBallView;
}


VisMesh CreateVisMeshFromStl (const char* filename)
{
	stl_reader::StlMesh<float, uint> mesh (MESH_PATH + filename);
	
	Box box = BoxFromCoordinates (mesh.raw_coords(), mesh.num_vrts() * 3, 3);

	LOGT(mesh, "Loaded mesh '" << MESH_PATH + filename << "'\n");
	LOGT(mesh, "  #vertices:    " << mesh.num_vrts() << std::endl);
	LOGT(mesh, "  #triangles:   " << mesh.num_tris() << std::endl);
	LOGT(mesh, "  Bounding box -> min: " << box.minCorner << std::endl);
	LOGT(mesh, "               -> max: " << box.maxCorner << std::endl);

//	VAO stores the buffer and the attribute pointers and whether they are enabled.
	uint VAO;
	glGenVertexArrays (1, &VAO);
	glBindVertexArray (VAO);

	uint VBO;
	glGenBuffers (1, &VBO);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER,
	              mesh.num_vrts() * 3 * sizeof(float),
	              mesh.raw_coords(),
	              GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);

	uint EBO;
	glGenBuffers (1, &EBO);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER,
	              mesh.num_tris() * 3 * sizeof(uint),
	              mesh.raw_tris(),
	              GL_STATIC_DRAW);

	return VisMesh(VAO,
	               static_cast<uint>(mesh.num_tris()) * 3,
	               GL_TRIANGLES);
}

VisMesh CreateWireVisMeshFromStl (const char* filename)
{
	stl_reader::StlMesh<float, uint> mesh (MESH_PATH + filename);
	
	Box box = BoxFromCoordinates (mesh.raw_coords(), mesh.num_vrts() * 3, 3);

//	VAO stores the buffer and the attribute pointers and whether they are enabled.
	uint VAO;
	glGenVertexArrays (1, &VAO);
	glBindVertexArray (VAO);

	uint VBO;
	glGenBuffers (1, &VBO);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER,
	              mesh.num_vrts() * 3 * sizeof(float),
	              mesh.raw_coords(),
	              GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);

	std::vector <index_t> edges;
	msh::UniqueSidesToIndexArray (edges,
	                         	   mesh.raw_tris(),
								   mesh.num_tris() * 3,
								   msh::TRI,
								   1);

	uint EBO;
	glGenBuffers (1, &EBO);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER,
	              edges.size() * sizeof(index_t),
	              &edges[0],
	              GL_STATIC_DRAW);

	LOGT(mesh, "Loaded mesh '" << MESH_PATH + filename << "'\n");
	LOGT(mesh, "  #vertices:    " << mesh.num_vrts() << std::endl);
	LOGT(mesh, "  #triangles:   " << mesh.num_tris() << std::endl);
	LOGT(mesh, "  #edges:       " <<edges.size() / 2 << std::endl);
	LOGT(mesh, "  Bounding box -> min: " << box.minCorner << std::endl);
	LOGT(mesh, "               -> max: " << box.maxCorner << std::endl);

	// LOGT(dbg, "edges: ");
	// for(size_t i = 0; i < edges.size(); i += 2) {
	// 	LOG("(" << edges[i] << ", " << edges[i+1] << ") - ");
	// }
	// LOG("\n");


	return VisMesh(VAO,
	               static_cast<uint>(edges.size()),
	               GL_LINES);
}

void RendererInit ()
{
	// if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
	// 	THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	// }

	if (!gladLoadGL ()) {
		THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	}


	g_visMesh = CreateVisMeshFromStl ("sphere.stl");
	g_solidShader.add_source_vs (SHADER_PATH + "vertex-shader-0.vs");
	g_solidShader.add_source_gs (SHADER_PATH + "flat-shading.gs");
	g_solidShader.add_source_fs (SHADER_PATH + "light-intensity.fs");
	g_solidShader.link();


	g_wireVisMesh = CreateWireVisMeshFromStl ("sphere.stl");
	g_wireShader.add_source_vs (SHADER_PATH + "vertex-shader-0.vs");
	g_wireShader.add_source_fs (SHADER_PATH + "wireframe.fs");
	g_wireShader.link();

	// msh::impl::PrintGrobDescs ();
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

	ActivateShader (g_solidShader);
	glBindVertexArray (g_visMesh.vao);
	glDrawElements (g_visMesh.primitiveType, g_visMesh.numIndices, GL_UNSIGNED_INT, 0);

	ActivateShader (g_wireShader);
	glBindVertexArray (g_wireVisMesh.vao);
	glDrawElements (g_wireVisMesh.primitiveType, g_wireVisMesh.numIndices, GL_UNSIGNED_INT, 0);
}


#include <glad/glad.h>	// include before other OpenGL related includes

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "stl_reader/stl_reader.h"

#include "imgui/imgui.h"
#include "imgui/imgui_binding.h"

#include "arc_ball_view.h"
#include "cond_throw.h"
#include "file_util.h"
#include "geometry.h"
#include "log.h"
#include "renderer.h"
#include "types.h"
#include "mesh.h"
#include "topology.h"
#include "shapes.h"
#include "vec_math_raw.h"
#include "visualization.h"

using namespace std;

namespace slimesh {

const std::string SHADER_PATH = string(RESOURCE_ROOT_PATH) + "/shaders/";
const std::string MESH_PATH = string(RESOURCE_ROOT_PATH) + "/meshes/";

static ArcBallView g_arcBallView;
static Visualization* g_visualization = nullptr;

static bool g_guiShowLog = false;
static bool g_guiShowDemo = false;
static bool g_guiShowVisualization = true;

WindowEventListener* RendererGetEventListener ()
{
	return &g_arcBallView;
}

SPMesh CreateMeshWithEdges (std::string filename)
{
	auto mesh = CreateMeshFromFile (filename);
	ComputeTriVertexNormals3 (*mesh, "normals");
	CreateEdgeInds (*mesh);

	Box box = BoxFromCoords (UNPACK_DST(*mesh->coords()));

	LOGT(mesh, "Loaded mesh '" << filename << "'\n");
	LOGT(mesh, "  #vertices:    " << mesh->coords()->num_tuples() << std::endl);
	LOGT(mesh, "  #edges:       " << mesh->inds(EDGE)->num_tuples() << std::endl);
	LOGT(mesh, "  #triangles:   " << mesh->inds(TRI)->num_tuples() << std::endl);
	LOGT(mesh, "  #tetrahedra:  " << mesh->inds(TET)->num_tuples() << std::endl);
	LOGT(mesh, "  Bounding box -> min: " << box.minCorner << std::endl);
	LOGT(mesh, "               -> max: " << box.maxCorner << std::endl);

	return mesh;
}

void RendererInit ()
{
	if (g_visualization)
		return;

	// if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
	// 	THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	// }

	if (!gladLoadGL ()) {
		THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	}

	g_visualization = new Visualization (SHADER_PATH);

	auto mainMesh = CreateMeshWithEdges (MESH_PATH + "bunny.stl");
	// auto mainMesh = CreateMeshWithEdges (MESH_PATH + "tet.ele");
	g_visualization->add_stage ("solid", mainMesh, TRI, FLAT);
	g_visualization->add_stage ("wire", mainMesh, EDGE, FLAT);

	{
		auto bndMesh = CreateBoundaryMesh (*mainMesh, FACES);
		g_visualization->add_stage ("bnd", bndMesh, EDGE, NONE);
	}

	{
		// CreateFaceInds (*mainMesh);
		// cout << "mainMesh num tris: " << mainMesh->inds(TRI)->num_tuples() << endl;
		// auto bndMesh = CreateBoundaryMesh (*mainMesh, TET, nullptr);
		// // // bndMesh->set_data <real_t>("vrtNormals", mainMesh->data <real_t>("vrtNormals"));
		// // // g_visualization->add_stage ("bnd", bndMesh, EDGE, NONE);

		// cout << "bndMesh num tris: " << bndMesh->inds(TRI)->num_tuples() << endl;
		// // ComputeTriVertexNormals3 (*bndMesh, "vrtNormals");
		// // CreateEdgeInds (*bndMesh);
		
		// // // g_visualization->add_stage ("solid", bndMesh, TRI, FLAT);
		// // g_visualization->add_stage ("wire", bndMesh, EDGE, FLAT);
	}

	// auto eleMesh = CreateMeshWithEdges (MESH_PATH + "box_with_spheres.ele");
	// g_visualization->add_stage ("wire", eleMesh, EDGE, NONE);

	// impl::PrintGrobDescs();
	// impl::PrintGrobSetDescs();
}


void RendererDispose ()
{
	if (g_visualization)
		delete g_visualization;
	g_visualization = nullptr;
}


void RendererDraw ()
{
	glEnable (GL_DEPTH_TEST);

	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const glm::vec2 clipDists = g_visualization->estimate_z_clip_dists(g_arcBallView.view());
	g_arcBallView.view().set_z_clip_dists (clipDists);
	g_visualization->render (g_arcBallView.view());
}


void GUI_DoHeader ()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Panels"))
        {
            ImGui::MenuItem("Show Visualization", NULL, &g_guiShowVisualization);
            ImGui::MenuItem("Show Log", NULL, &g_guiShowLog);
            ImGui::MenuItem("Show ImGui Demo", NULL, &g_guiShowDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void RendererProcessGUI (bool draw)
{
	GUI_DoHeader ();

	if (g_guiShowVisualization)
		g_visualization->do_imgui (&g_guiShowVisualization);

	if (g_guiShowLog)
		DefLog().draw("log", &g_guiShowLog);

	if (g_guiShowDemo)
		ImGui::ShowDemoWindow (&g_guiShowDemo);

	ImGui::Render();
	if (draw) {
		slimesh::ImGui_RenderDrawData(ImGui::GetDrawData());
	}
}

}// end of namespace slimesh

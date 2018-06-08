
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

WindowEventListener* RendererGetEventListener ()
{
	return &g_arcBallView;
}

SPMesh LoadMeshWithEdges (std::string filename)
{
	auto mesh = CreateMeshFromFile (filename);
	ComputeTriVertexNormals3 (*mesh, "vrtNormals");
	CreateEdgeInds (*mesh);

	Box box = BoxFromCoords (UNPACK_DST(*mesh->coords()));

	LOGT(mesh, "Loaded mesh '" << filename << "'\n");
	LOGT(mesh, "  #vertices:    " << mesh->coords()->num_tuples() << std::endl);
	LOGT(mesh, "  #triangles:   " << mesh->inds(TRI)->num_tuples() << std::endl);
	LOGT(mesh, "  #edges:       " << mesh->inds(EDGE)->num_tuples() << std::endl);
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

	auto mainMesh = LoadMeshWithEdges (MESH_PATH + "bunny.stl");
	g_visualization->add_stage ("solid", mainMesh, TRI, FLAT);
	g_visualization->add_stage ("wire", mainMesh, EDGE, FLAT);

	{
		auto sphere = SphereFromCoords (UNPACK_DST(*mainMesh->coords()));

		auto sphereMesh = LoadMeshWithEdges (MESH_PATH + "sphere.stl");
		VecScale (UNPACK_DS(*sphereMesh->coords()), sphere.radius);
		VecTupAppend (UNPACK_DST(*sphereMesh->coords()), glm::value_ptr (sphere.center));

		// g_visualization->add_stage ("wireSphere", sphereMesh, EDGE, FLAT);
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

	// g_arcBallView.camera().translate(glm::vec3(0, 0, -1));
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

	glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const glm::vec2 clipDists = g_visualization->estimate_z_clip_dists(g_arcBallView.view());
	g_arcBallView.view().set_z_clip_dists (clipDists);
	g_visualization->render (g_arcBallView.view());
}


void GUI_DoHeader ()
{
	static const ImGuiWindowFlags menuWindowFlags =	ImGuiWindowFlags_NoTitleBar
												| 	ImGuiWindowFlags_NoScrollbar
												| 	ImGuiWindowFlags_MenuBar
												| 	ImGuiWindowFlags_NoMove
												| 	ImGuiWindowFlags_NoResize
												| 	ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowPos(ImVec2(0,0));
    if (!ImGui::Begin("SLIMESH", NULL, menuWindowFlags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Panels"))
        {
            ImGui::MenuItem("Show Log", NULL, &g_guiShowLog);
            ImGui::MenuItem("Show ImGui Demo", NULL, &g_guiShowDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End(); // SLIMESH
}

void RendererProcessGUI (bool draw)
{
	GUI_DoHeader ();

	if (g_guiShowLog)
		DefLog().draw("log");

	if (g_guiShowDemo)
		ImGui::ShowDemoWindow (&g_guiShowDemo);

	if (draw) {
		ImGui::Render();
		slimesh::ImGui_RenderDrawData(ImGui::GetDrawData());
	}
}

}// end of namespace slimesh

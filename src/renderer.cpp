
#include <glad/glad.h>	// include before other OpenGL related includes

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_binding.h"

#include "arc_ball_view.h"
#include "cond_throw.h"
#include "file_util.h"
#include "log.h"
#include "renderer.h"
#include "config.h"
#include "shapes.h"
#include "visualization.h"

#include "slimesh/normals.h"
#include "slimesh/mesh.h"
#include "slimesh/topology.h"
#include "slimesh/vec_math_raw.h"
#include "slimesh/file_io.h"

#include "subset_info_annex_imgui.h"

using namespace std;

namespace slimesh {

const std::string SHADER_PATH = string(RESOURCE_ROOT_PATH) + "/shaders/";
const std::string MESH_PATH = string(RESOURCE_ROOT_PATH) + "/meshes/";

static ArcBallView g_arcBallView;
static Visualization* g_visualization = nullptr;

static bool g_guiShowLog = false;
static bool g_guiShowDemo = false;
static bool g_guiShowVisualization = true;
static bool g_guiShowMeshes = true;


std::vector <SPMesh> g_meshes;


WindowEventListener* RendererGetEventListener ()
{
	return &g_arcBallView;
}

void PrintMeshInfo (SPMesh mesh)
{
	Box box = BoxFromCoords (UNPACK_DST(*mesh->coords()));

	LOGT(mesh, "  #vertices:    " << mesh->coords()->num_tuples() << std::endl);
	LOGT(mesh, "  #edges:       " << mesh->inds(EDGE)->num_tuples() << std::endl);
	LOGT(mesh, "  #triangles:   " << mesh->inds(TRI)->num_tuples() << std::endl);
	LOGT(mesh, "  #quads:       " << mesh->inds(QUAD)->num_tuples() << std::endl);
	LOGT(mesh, "  #tetrahedra:  " << mesh->inds(TET)->num_tuples() << std::endl);
	LOGT(mesh, "  #hexahedra:   " << mesh->inds(HEX)->num_tuples() << std::endl);
	LOGT(mesh, "  #pyramids:    " << mesh->inds(PYRA)->num_tuples() << std::endl);
	LOGT(mesh, "  #prisms:      " << mesh->inds(PRISM)->num_tuples() << std::endl);
	LOGT(mesh, "  Bounding box -> min: " << box.minCorner << std::endl);
	LOGT(mesh, "               -> max: " << box.maxCorner << std::endl);
}

// SPMesh CreateMeshWithEdges (std::string filename)
// {
// 	auto mesh = CreateMeshFromFile (filename);
// 	
// 	

// 	return mesh;
// }

void RendererInit ()
{
	if (g_visualization)
		return;

	SubsetInfoAnnex::set_imgui_executor (&SubsetInfoAnnex_ImGui);
	// impl::PrintGrobDescs();
	// impl::PrintGrobSetDescs();

	// if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
	// 	THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	// }

	if (!gladLoadGL ()) {
		THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	}

	g_visualization = new Visualization (SHADER_PATH);

	// const std::string filename = MESH_PATH + "tet.ugx";
	const std::string filename = MESH_PATH + "elems.ugx";
	// const std::string filename = MESH_PATH + "tri_and_quad.ugx";
	// const std::string filename = MESH_PATH + "bunny.stl";
	auto mainMesh = CreateMeshFromFile (filename);
	LOGT(mesh, "Loaded mesh '" << filename << "'\n");
	PrintMeshInfo (mainMesh);

	const glm::vec4 solidColor (1.0f, 0.843f, 0.f, 1.0f);
	const glm::vec4 wireColor (0.2f, 0.2f, 0.2f, 1.0f);
	const glm::vec4 bndColor (1.0f, 0.2f, 0.2f, 1.0f);

	g_meshes.push_back (mainMesh);

	if (mainMesh->has (CELLS)) {
		auto bndMesh = CreateBoundaryMesh (*mainMesh, CELLS);
		ComputeFaceVertexNormals3 (*bndMesh, "normals");
		CreateEdgeInds (*bndMesh);
		g_visualization->add_stage ("solid", bndMesh, FACES, FLAT);
		g_visualization->stage_set_color (solidColor);
		g_visualization->add_stage ("wire", bndMesh, EDGES, FLAT);
		g_visualization->stage_set_color (wireColor);
	}
	else if (mainMesh->has (FACES)) {
		ComputeFaceVertexNormals3 (*mainMesh, "normals");
		CreateEdgeInds (*mainMesh);
		g_visualization->add_stage ("solid", mainMesh, FACES, FLAT);
		g_visualization->stage_set_color (solidColor);
		g_visualization->add_stage ("wire", mainMesh, EDGES, FLAT);
		g_visualization->stage_set_color (wireColor);
		auto bndMesh = CreateBoundaryMesh (*mainMesh, FACES);
		g_visualization->add_stage ("bnd", bndMesh, EDGES, NONE);
		g_visualization->stage_set_color (bndColor);
	}
	else if (mainMesh->has (EDGES)) {
		ComputeFaceVertexNormals3 (*mainMesh, "normals");
		g_visualization->add_stage ("wire", mainMesh, EDGES, NONE);
		g_visualization->stage_set_color (wireColor);
	}
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

	glClearColor (0.25f, 0.25f, 0.25f, 1.0f);
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

	if (g_guiShowMeshes) {
		if (!ImGui::Begin("Meshes", &g_guiShowMeshes, 0)) {
	        // Early out if the window is collapsed, as an optimization.
	        ImGui::End();
	        return;
	    }
		for(auto& mesh : g_meshes) {
			ImGui::PushID (mesh.get());
			if (ImGui::CollapsingHeader("mesh")) {
				for (auto annexIter = mesh->annex_begin();
				     annexIter != mesh->annex_end(); ++annexIter)
				{
					if (annexIter->second->has_gui()) {
						if (annexIter != mesh->annex_begin())
							ImGui::Separator();
						
						string label = string (annexIter->second->class_name()) + ": " + annexIter->first.name.c_str();
						if (ImGui::TreeNode (label.c_str()))
						{
							annexIter->second->do_imgui();
							ImGui::TreePop();
						}
					}
				}
			}
			ImGui::PopID();
		}
		ImGui::End();
	}

	ImGui::Render();
	if (draw) {
		slimesh::ImGui_RenderDrawData(ImGui::GetDrawData());
	}
}

}// end of namespace slimesh

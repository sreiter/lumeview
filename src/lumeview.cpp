// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <string>

#include <glad/glad.h>	// include before other OpenGL related includes

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "log.h"
#include "lumeview.h"
#include "imgui/imgui.h"
#include "imgui/imgui_binding.h"

#include "shapes.h"
#include "plain_visualization.h"
#include "subset_visualization.h"
#include "subset_info_annex_imgui.h"

#include "lume/normals.h"
#include "lume/mesh.h"
#include "lume/topology.h"
#include "lume/vec_math_raw.h"
#include "lume/file_io.h"

using namespace std;
using namespace lume;


namespace lumeview {

void LumeviewInit ()
{
	SubsetInfoAnnex::set_imgui_executor (&SubsetInfoAnnex_ImGui);

	// if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
	// 	THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	// }

	if (!gladLoadGL ()) {
		THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	}

	lumeview::ImGui_Init();
}

void LumeviewShutdown ()
{
	ImGui_Shutdown ();
}


////////////////////////////////////////////////////////////////////////////////
// Lumeview

void Lumeview::clear ()
{
	m_scenes.clear();
	m_activeScene.reset();
}

Lumeview::Lumeview () :
	m_guiShowScene (true),
	m_guiShowLog (true),
	m_guiShowDemo (false)
{
	m_imguiListener = ImGui_GetEventListener ();
}

void Lumeview::mouse_button (int button, int action, int mods)
{
	base_t::mouse_button (button, action, mods);

	m_imguiListener->mouse_button (button, action, mods);

	if(!ImGui::GetIO().WantCaptureMouse)
		m_arcBallView.mouse_button (button, action, mods);
}


void Lumeview::mouse_move (const glm::vec2& c)
{
	base_t::mouse_move (c);

	m_imguiListener->mouse_move (c);

	if(!ImGui::GetIO().WantCaptureMouse)
		m_arcBallView.mouse_move (c);
}


void Lumeview::mouse_scroll (const glm::vec2& o)
{
	base_t::mouse_scroll (o);

	m_imguiListener->mouse_scroll (o);

	if(!ImGui::GetIO().WantCaptureMouse)
		m_arcBallView.mouse_scroll (o);
}



void Lumeview::set_viewport (const glm::ivec4& vp)
{
	base_t::set_viewport (vp);

	m_imguiListener->set_viewport (vp);
	m_arcBallView.set_viewport (vp);
}



void Lumeview::key (int key, int scancode, int action, int mods)
{
	base_t::key (key, scancode, action, mods);

	m_imguiListener->key (key, scancode, action, mods);

	if(!ImGui::GetIO().WantCaptureKeyboard)
		m_arcBallView.key (key, scancode, action, mods);
}


void Lumeview::character (unsigned int c)
{
	base_t::character (c);

	m_imguiListener->character (c);

	if(!ImGui::GetIO().WantCaptureKeyboard)
		m_arcBallView.character (c);
}

void Lumeview::add_scene (SPScene scene)
{
	if (!scene)
		return;

	if (m_scenes.empty())
		m_activeScene = scene;

	m_scenes.push_back (scene);
}

void Lumeview::process_gui ()
{
	lumeview::ImGui_NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Panels"))
        {
            ImGui::MenuItem("Show Visualization", NULL, &m_guiShowScene);
            ImGui::MenuItem("Show Log", NULL, &m_guiShowLog);
            ImGui::MenuItem("Show ImGui Demo", NULL, &m_guiShowDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

	if (m_guiShowLog)
		DefLog().draw("log", &m_guiShowLog);

	if (m_guiShowDemo)
		ImGui::ShowDemoWindow (&m_guiShowDemo);

	if (m_guiShowScene && m_activeScene)
		m_activeScene->do_imgui (&m_guiShowScene);

	ImGui::Render();

	MessageQueue::dispatch ();
}

void Lumeview::render ()
{
	glEnable (GL_DEPTH_TEST);

	glClearColor (0.25f, 0.25f, 0.25f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_activeScene) {
		const glm::vec2 clipDists = m_activeScene->estimate_z_clip_dists(m_arcBallView.view());
		m_arcBallView.view().set_z_clip_dists (clipDists);
		m_activeScene->render (m_arcBallView.view());
	}

	lumeview::ImGui_Display();
}


static void PrintMeshInfo (SPMesh mesh)
{
	Box box = BoxFromCoords (UNPACK_DST(*mesh->coords()));

	LOGT(mesh, "  #vertices:    " << mesh->coords()->num_tuples() << std::endl);
	LOGT(mesh, "  #edges:       " << mesh->grobs(EDGE).size() << std::endl);
	LOGT(mesh, "  #triangles:   " << mesh->grobs(TRI).size() << std::endl);
	LOGT(mesh, "  #quads:       " << mesh->grobs(QUAD).size() << std::endl);
	LOGT(mesh, "  #tetrahedra:  " << mesh->grobs(TET).size() << std::endl);
	LOGT(mesh, "  #hexahedra:   " << mesh->grobs(HEX).size() << std::endl);
	LOGT(mesh, "  #pyramids:    " << mesh->grobs(PYRA).size() << std::endl);
	LOGT(mesh, "  #prisms:      " << mesh->grobs(PRISM).size() << std::endl);
	LOGT(mesh, "  Bounding box -> min: " << box.minCorner << std::endl);
	LOGT(mesh, "               -> max: " << box.maxCorner << std::endl);
}


void Lumeview::add_sample_scene ()
{
	// const std::string filename = MESH_PATH + "tet.ugx";
	// const std::string filename = MESH_PATH + "elems_refined.ugx";
	// const std::string filename = MESH_PATH + "tri_and_quad.ugx";
	// const std::string filename = MESH_PATH + "bunny.stl";
	const std::string filename = MESH_PATH + "circle_with_subsets.ugx";

	auto mesh = CreateMeshFromFile (filename);
	LOGT(mesh, "Loaded mesh '" << filename << "'\n");
	PrintMeshInfo (mesh);

	auto vis = make_shared <SubsetVisualization> ();
	vis->set_mesh (mesh);
	auto scene = make_shared <Scene>();
	scene->add_entry (mesh, vis);
	add_scene (scene);
}
}//	end of namespace lumeview

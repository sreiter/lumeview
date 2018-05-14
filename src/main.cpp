#include <iostream>

#include <glad/glad.h>	// include before other OpenGL related includes
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "imgui_log.h"
#include "cond_throw.h"
#include "renderer.h"

using namespace std;


static WindowEventListener* g_eventListener = nullptr;


void HandleGLFWError (int error, const char* description)
{
	THROW ("GLFW ERROR (code " << error << "):\n" <<
	       description);
}


void ResizeFramebuffer (GLFWwindow* window, int width, int height)
{
	if(g_eventListener)
		g_eventListener->set_viewport (glm::ivec4(0, 0, width, height));
}


void ProcessInput (GLFWwindow* window)
{
	if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose (window, true);
}

void CursorPositionCallback(GLFWwindow* window, double x, double y)
{
	if(g_eventListener && !ImGui::GetIO().WantCaptureMouse)
		g_eventListener->mouse_move (glm::vec2(x, y));
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	if(g_eventListener && !ImGui::GetIO().WantCaptureMouse)
		g_eventListener->mouse_button (button, action, mods);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	if(g_eventListener && !ImGui::GetIO().WantCaptureMouse)
		g_eventListener->mouse_scroll (glm::vec2 (xoffset, yoffset));
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void CharCallback(GLFWwindow* window, unsigned int c)
{
	ImGui_ImplGlfw_CharCallback(window, c);
}


int main (int argc, char** argv)
{
	try {
		glfwSetErrorCallback (HandleGLFWError);
		glfwInit ();
		glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, USE_GL_VERSION_MAJOR);
		glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, USE_GL_VERSION_MINOR);
		if((USE_GL_VERSION_MAJOR >= 3) && (USE_GL_VERSION_MINOR >= 2))
			glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//	add for OSX:
		#ifdef __APPLE__
			glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		#endif

	//	Set up window
		GLFWwindow* window = glfwCreateWindow (800, 600, "OpenGL3-Tests-0", NULL, NULL);
		COND_THROW (window == NULL, "GLFW::INIT\n" <<
		            "Failed to create glfw window");

		glfwMakeContextCurrent (window);
		glfwSwapInterval(1);

		//	init IMGUI
		// Setup Dear ImGui binding
	    IMGUI_CHECKVERSION();
	    ImGui::CreateContext();
	    ImGui_ImplGlfwGL3_Init(window, false);

	    glfwSetCursorPosCallback (window, CursorPositionCallback);
	    glfwSetMouseButtonCallback (window, MouseButtonCallback);
	    glfwSetScrollCallback (window, ScrollCallback);
	    glfwSetKeyCallback (window, KeyCallback);
	    glfwSetCharCallback (window, CharCallback);

	    RendererInit ();
	    g_eventListener = RendererGetEventListener ();

	    // Setup style
	    ImGui::StyleColorsDark();
	    //ImGui::StyleColorsClassic();

	//	setup view
		ResizeFramebuffer (window, 800, 600);
		glfwSetFramebufferSizeCallback (window, ResizeFramebuffer);


		while (!glfwWindowShouldClose (window))
		{
			ImGui_ImplGlfwGL3_NewFrame();

			DefLog().draw("log");

			ProcessInput (window);

			RendererDraw ();

			ImGui::Render();
        	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers (window);
			glfwPollEvents ();
		}

	} catch (std::exception& e) {
		cout << "\nAn ERROR occurred during execution:\n\n";
		cout << e.what() << endl;
		glfwTerminate ();
		return 1;
	}

	glfwTerminate ();
	return 0;
}


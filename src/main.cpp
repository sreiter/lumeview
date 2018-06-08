#include <iostream>

#include <glad/glad.h>	// include before other OpenGL related includes
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_binding.h"

#include "log.h"
#include "cond_throw.h"
#include "renderer.h"

using namespace std;
using namespace slimesh;

static WindowEventListener*	g_imguiListener = nullptr;
static WindowEventListener*	g_renderListener = nullptr;
static glm::vec2			g_pixelScale (1);

void HandleGLFWError (int error, const char* description)
{
	THROW ("GLFW ERROR (code " << error << "):\n" <<
	       description);
}


void FramebufferResized (GLFWwindow* window, int width, int height)
{
	int winWidth, winHeight;
    glfwGetWindowSize (window, &winWidth, &winHeight);
	if(winWidth > 0 && winHeight > 0 && width > 0 && height > 0)
		g_pixelScale = glm::vec2 ((float) width / (float) winWidth,
		                          (float) height / (float) winHeight);

	if(g_imguiListener)
		g_imguiListener->set_viewport (glm::ivec4(0, 0, width, height));

	if(g_renderListener)
		g_renderListener->set_viewport (glm::ivec4(0, 0, width, height));
}


void ProcessInput (GLFWwindow* window)
{
	if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose (window, true);
}

void CursorPositionCallback(GLFWwindow* window, double x, double y)
{
	if (g_imguiListener)
		g_imguiListener->mouse_move (glm::vec2(x, y) * g_pixelScale);

	if(g_renderListener && !ImGui::GetIO().WantCaptureMouse)
		g_renderListener->mouse_move (glm::vec2(x, y) * g_pixelScale);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (g_imguiListener)
		g_imguiListener->mouse_button (button, action, mods);

	if(g_renderListener && !ImGui::GetIO().WantCaptureMouse)
		g_renderListener->mouse_button (button, action, mods);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (g_imguiListener)
		g_imguiListener->mouse_scroll (glm::vec2 (xoffset, yoffset));

	if(g_renderListener && !ImGui::GetIO().WantCaptureMouse)
		g_renderListener->mouse_scroll (glm::vec2 (xoffset, yoffset));
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void CharCallback(GLFWwindow* window, unsigned int c)
{
	// ImGui_ImplGlfw_CharCallback(window, c);
}

static const char* ImGui_GetClipboardText(void* user_data)
{
    return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void ImGui_SetClipboardText(void* user_data, const char* text)
{
    glfwSetClipboardString((GLFWwindow*)user_data, text);
}

void InitImGui (GLFWwindow* window)
{
	//	init IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    slimesh::ImGui_Init();
    g_imguiListener = ImGui_GetEventListener ();

    // Setup style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    // ImGui::StyleColorsClassic();

    ImGuiIO& io = ImGui::GetIO();
	io.SetClipboardTextFn = ImGui_SetClipboardText;
    io.GetClipboardTextFn = ImGui_GetClipboardText;
    io.ClipboardUserData = window;
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
		GLFWwindow* window = glfwCreateWindow (800, 600, "slimesh", NULL, NULL);
		COND_THROW (window == NULL, "GLFW::INIT\n" <<
		            "Failed to create glfw window");

		glfwMakeContextCurrent (window);
		glfwSwapInterval(1);

		RendererInit ();
	    g_renderListener = RendererGetEventListener ();

	    InitImGui (window);

	    glfwSetCursorPosCallback (window, CursorPositionCallback);
	    glfwSetMouseButtonCallback (window, MouseButtonCallback);
	    glfwSetScrollCallback (window, ScrollCallback);
	    glfwSetKeyCallback (window, KeyCallback);
	    glfwSetCharCallback (window, CharCallback);

	//	setup view
	    int frmBufWidth, frmBufHeight;
	    glfwGetFramebufferSize (window, &frmBufWidth, &frmBufHeight);
		FramebufferResized (window, frmBufWidth, frmBufHeight);
		glfwSetFramebufferSizeCallback (window, FramebufferResized);

		while (!glfwWindowShouldClose (window))
		{
			slimesh::ImGui_NewFrame();

			ProcessInput (window);

			RendererDraw ();

			RendererProcessGUI (true);
			
			glfwSwapBuffers (window);
			glfwPollEvents ();
		}

	} catch (std::exception& e) {
		cout << "\nAn ERROR occurred during execution:\n";
		cout << e.what() << endl << endl;
		slimesh::ImGui_Shutdown ();
		RendererDispose ();
		glfwTerminate ();
		return 1;
	}

	slimesh::ImGui_Shutdown ();
	RendererDispose ();
	glfwTerminate ();
	return 0;
}


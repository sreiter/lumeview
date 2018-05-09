#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#include <glad/glad.h>	// include before other OpenGL related includes
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "stl_reader/stl_reader.h"

#include "camera/arc_ball.h"

#define THROW(msg)	{std::stringstream ss; ss << msg; throw(std::runtime_error(ss.str()));}
#define COND_THROW(cond, msg)	if(cond){THROW(msg);}

const std::string SHADER_PATH = "../shaders/";
const std::string MESH_PATH = "../meshes/";

using uint = unsigned int;
using namespace std;

class WindowData {
  public:
	WindowData () : io (ImGui::GetIO())
	{
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	}

	ArcBall arcBall;
	ImGuiIO& io;
};



struct VisMesh {
	VisMesh () : vao (0), numVrts (0), numTris (0) {}
	VisMesh (uint _vao, uint _numVrts, uint _numTris) :
		vao (_vao), numVrts (_numVrts), numTris (_numTris) {}

	uint vao;
	uint numVrts;
	uint numTris;
};


WindowData& GetWindowData (GLFWwindow* window)
{
	WindowData* windowData = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer (window));
	COND_THROW (!windowData, "No window data is associated with the specifed glfw window.");
	return *windowData;
}

void HandleGLFWError (int error, const char* description)
{
	THROW ("GLFW ERROR (code " << error << "):\n" <<
	       description);
}


void ResizeFramebuffer (GLFWwindow* window, int width, int height)
{
	glViewport (0, 0, width, height);
	GetWindowData (window).arcBall.set_frame (glm::vec2(width, height));
}


void ProcessInput (GLFWwindow* window)
{
	if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose (window, true);
}

void CursorPositionCallback(GLFWwindow* window, double x, double y)
{
	ArcBall& arcBall = GetWindowData (window).arcBall;
	if(arcBall.dragging())
		arcBall.drag_to (glm::vec2(x, y));
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == 0){
		ArcBall& arcBall = GetWindowData (window).arcBall;
		switch(action) {
			case GLFW_PRESS: {
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				arcBall.begin_drag (glm::vec2(x, y));
			} break;

			case GLFW_RELEASE: {
				if(arcBall.dragging ())
					arcBall.end_drag ();
			} break;
		}
	}
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void CharCallback(GLFWwindow* window, unsigned int c)
{
	ImGui_ImplGlfw_CharCallback(window, c);
}


string LoadStringFromFile (const char* filename)
{
	ifstream t(filename);
	COND_THROW (!t, "FILE::LOAD_FROM_STRING" << endl <<
	            "Couldn't find file " << filename);
	t.seekg(0, ios::end);
	size_t size = t.tellg();
	string buffer(size, ' ');
	t.seekg(0);
	t.read(&buffer[0], size);
	return std::move(buffer);
}

string LoadStringFromFile (const std::string& filename)
{
	return LoadStringFromFile (filename.c_str());
}


VisMesh CreateVisMeshFromStl (const char* filename)
{
	stl_reader::StlMesh<float, uint> mesh (MESH_PATH + filename);

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
	               static_cast<uint>(mesh.num_vrts()),
	               static_cast<uint>(mesh.num_tris()));
}

uint CreateSampleVAO ()
{
	float vertices[] = {
		 0.5f,	 0.5f,	0.0f,
		 0.5f,	-0.5f,	0.0f,
		 -0.5f,	-0.5f,	0.0f,
		 -0.5f,	0.5f,	0.0f };

	uint indices[] = {
		0, 1, 3,
		1, 2, 3 };

//	VAO stores the buffer and the attribute pointers and whether they are enabled.
	uint VAO;
	glGenVertexArrays (1, &VAO);
	glBindVertexArray (VAO);

	uint VBO;
	glGenBuffers (1, &VBO);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);

	uint EBO;
	glGenBuffers (1, &EBO);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray (0);
	return VAO;
}


uint CreateShader (const char* filename, GLenum shaderType)
{
//	load the sources from a file
	string shaderSrc = LoadStringFromFile (SHADER_PATH + filename);
	const char* shaderSrcCStr = shaderSrc.c_str();

//	create the shader object and compile the sources
	uint shader;
	shader = glCreateShader (shaderType);
	glShaderSource (shader, 1, &shaderSrcCStr, NULL);
	glCompileShader (shader);

//	check for errors
	int success;
	glGetShaderiv (shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog (shader, 512, NULL, infoLog);
		THROW("SHADER::COMPILATION_FAILED in '" << filename << "'\n" << infoLog);
	}

	return shader;
}


uint CreateVertexShader (const char* filename)
{
	return CreateShader (filename, GL_VERTEX_SHADER);
}


uint CreateFragmentShader (const char* filename)
{
	return CreateShader (filename, GL_FRAGMENT_SHADER);
}


uint CreateShaderProgramVF (	const char* vertexShaderFilename,
                                    	const char* fragmentShaderFilename)
{
	uint vertexShader = CreateVertexShader (vertexShaderFilename);
	uint fragmentShader = CreateFragmentShader (fragmentShaderFilename);

	uint shaderProg;
	shaderProg = glCreateProgram ();
	glAttachShader (shaderProg, vertexShader);
	glAttachShader (shaderProg, fragmentShader);
	glLinkProgram (shaderProg);

//	check for errors
	int success;
	glGetProgramiv (shaderProg, GL_LINK_STATUS, &success);

	glDeleteShader (vertexShader);
	glDeleteShader (fragmentShader);

	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog (shaderProg, 512, NULL, infoLog);
		THROW("SHADER_PROGRAM::LINK_FAILED for\n"
		      "  vertex shader:   '" << vertexShaderFilename << "'\n" <<
		      "  fragment shader: '"  << "'\n" <<
		      infoLog);
	}

	return shaderProg;
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

		auto windowData = make_unique<WindowData> ();
		glfwSetWindowUserPointer (window, windowData.get());

	//	initialize GLAD
		if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
			THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
		}


	    glfwSetCursorPosCallback (window, CursorPositionCallback);
	    glfwSetMouseButtonCallback (window, MouseButtonCallback);
	    glfwSetScrollCallback (window, ScrollCallback);
	    glfwSetKeyCallback (window, KeyCallback);
	    glfwSetCharCallback (window, CharCallback);

	    // Setup style
	    ImGui::StyleColorsDark();
	    //ImGui::StyleColorsClassic();

	//	setup view
		ResizeFramebuffer (window, 800, 600);
		glfwSetFramebufferSizeCallback (window, ResizeFramebuffer);

		glEnable (GL_DEPTH_TEST);

		//uint vertexArray = CreateSampleVAO ();
		VisMesh visMesh = CreateVisMeshFromStl ("box.stl");

		uint shaderProgram =
			CreateShaderProgramVF ("vertex-shader-0.vs",
			                       "fragment-shader-0.fs");

		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");

		while (!glfwWindowShouldClose (window))
		{
			ImGui_ImplGlfwGL3_NewFrame();

			ImGui::Begin("First window");
			ImGui::Text("Hello world!");
			ImGui::End();

			ProcessInput (window);

			glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 rotMat = windowData->arcBall.rotation_matrix ();

			glUseProgram (shaderProgram);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(rotMat));

			glBindVertexArray (visMesh.vao);

			glDrawElements (GL_TRIANGLES, visMesh.numTris * 3, GL_UNSIGNED_INT, 0);

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

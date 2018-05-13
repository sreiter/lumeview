
#include <glad/glad.h>	// include before other OpenGL related includes

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "stl_reader/stl_reader.h"

#include "renderer.h"
#include "arc_ball_view.h"
#include "cond_throw.h"
#include "file_util.h"

using uint = unsigned int;
using namespace std;


struct VisMesh {
	VisMesh () : vao (0), numVrts (0), numTris (0) {}
	VisMesh (uint _vao, uint _numVrts, uint _numTris) :
		vao (_vao), numVrts (_numVrts), numTris (_numTris) {}

	uint vao;
	uint numVrts;
	uint numTris;
};


const std::string SHADER_PATH = "shaders/";
const std::string MESH_PATH = "meshes/";

static ArcBallView g_arcBallView;

static VisMesh g_visMesh;
static uint g_shaderProgram = 0;


WindowEventListener* RendererGetEventListener ()
{
	return &g_arcBallView;
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


uint CreateShaderProgramVF (const char* vertexShaderFilename,
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

void RendererInit ()
{
	// if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
	// 	THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	// }

	if (!gladLoadGL ()) {
		THROW("GLAD::INITIALIZATION\n  Failed to initialize GLAD" << endl);
	}

	//uint vertexArray = CreateSampleVAO ();
	g_visMesh = CreateVisMeshFromStl ("box.stl");

	g_shaderProgram =
		CreateShaderProgramVF ("vertex-shader-0.vs",
		                       "fragment-shader-0.fs");
}


void ActivateShader (const uint shaderProg)
{
	glUseProgram (shaderProg);

	const uint viewLoc = glGetUniformLocation(shaderProg, "view");
	g_arcBallView.view().apply(viewLoc);
}


void RendererDraw ()
{
	glEnable (GL_DEPTH_TEST);

	glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ActivateShader (g_shaderProgram);
	glBindVertexArray (g_visMesh.vao);
	glDrawElements (GL_TRIANGLES, g_visMesh.numTris * 3, GL_UNSIGNED_INT, 0);
}

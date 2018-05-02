#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <glad/glad.h>	// include before other OpenGL related includes
#include <GLFW/glfw3.h>


#define THROW(msg)	{std::stringstream ss; ss << msg; throw(std::runtime_error(ss.str()));}
#define COND_THROW(cond, msg)	if(cond){THROW(msg);}

const char* SHADER_PATH = "../shaders/";

using namespace std;


void ResizeFramebuffer (GLFWwindow* window, int width, int height)
{
	glViewport (0, 0, width, height);
}


void ProcessInput (GLFWwindow* window)
{
	if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose (window, true);
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


unsigned int CreateSampleVAO ()
{
	float vertices[] = {
		 0.5f,	 0.5f,	0.0f,
		 0.5f,	-0.5f,	0.0f,
		 -0.5f,	-0.5f,	0.0f,
		 -0.5f,	0.5f,	0.0f };

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3 };

//	VAO stores the buffer and the attribute pointers and whether they are enabled.
	unsigned int VAO;
	glGenVertexArrays (1, &VAO);
	glBindVertexArray (VAO);

	unsigned int VBO;
	glGenBuffers (1, &VBO);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);

	glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);

	unsigned int EBO;
	glGenBuffers (1, &EBO);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	return VAO;
}


unsigned int CreateShader (const char* filename, GLenum shaderType)
{
//	load the sources from a file
	string fullFilename = string(SHADER_PATH).append(filename);
	string shaderSrc = LoadStringFromFile (fullFilename.c_str());
	const char* shaderSrcCStr = shaderSrc.c_str();

//	create the shader object and compile the sources
	unsigned int shader;
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


unsigned int CreateVertexShader (const char* filename)
{
	return CreateShader (filename, GL_VERTEX_SHADER);
}


unsigned int CreateFragmentShader (const char* filename)
{
	return CreateShader (filename, GL_FRAGMENT_SHADER);
}


unsigned int CreateShaderProgramVF (	const char* vertexShaderFilename,
                                    	const char* fragmentShaderFilename)
{
	unsigned int vertexShader = CreateVertexShader (vertexShaderFilename);
	unsigned int fragmentShader = CreateFragmentShader (fragmentShaderFilename);

	unsigned int shaderProg;
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
	glfwInit ();
	try {
		glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//	add for OSX:
		//glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	//	Set up window
		GLFWwindow* window = glfwCreateWindow (800, 600, "OpenGL3-Tests-0", NULL, NULL);
		COND_THROW (window == NULL, "GLFW::INIT\n" <<
		            "Failed to create glfw window");

		glfwMakeContextCurrent (window);


	//	initialize GLAD
		if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
			cout << "Failed to initialize GLAD" << endl;
			return -1;
		}

	//	setup view
		ResizeFramebuffer (window, 800, 600);
		glfwSetFramebufferSizeCallback (window, ResizeFramebuffer);

		unsigned int vertexArray = CreateSampleVAO ();

		unsigned int shaderProgram =
			CreateShaderProgramVF ("vertex-shader-0.glsl",
			                       "fragment-shader-0.glsl");

		while (!glfwWindowShouldClose (window)) {
			ProcessInput (window);

			glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
			glClear (GL_COLOR_BUFFER_BIT);

			glUseProgram (shaderProgram);
			glBindVertexArray (vertexArray);

			glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

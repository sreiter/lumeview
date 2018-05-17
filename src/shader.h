#ifndef __H__shader
#define __H__shader

#include <glad/glad.h>	// include before other OpenGL related includes
#include <string>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <glm/gtc/type_ptr.hpp>

#include "cond_throw.h"
#include "file_util.h"
#include "types.h"

class Shader
{
public:
	Shader() : m_shaderProg (0), m_linked (false)
	{
	}

	~Shader()
	{
		for(auto shader : m_shaders) {
			glDeleteShader (shader);
		}
	}
	
	void use () {
		if (!m_linked)
			link ();
		glUseProgram (m_shaderProg);
	}

	void use () const {
		COND_THROW (!m_linked, "The shader has not been linked yet, and thus can't be used.");
		glUseProgram (m_shaderProg);
	}

	uint get_uniform_location (const char* name) const
	{
		return glGetUniformLocation(m_shaderProg, name);
	}

	void add_source (std::string filename, GLenum shaderType) {
		if (!m_shaderProg)
			m_shaderProg = glCreateProgram ();

	//	load the sources from a file
		std::string shaderSrc = LoadStringFromFile (filename);
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

		glAttachShader (m_shaderProg, shader);
		m_linked = false;
		m_shaders.push_back (shader);
		m_shaderNames.push_back (filename);
		m_shaderTypes.push_back (shaderType);
	}

	void add_source_vs (std::string filename)
	{
		add_source (filename, GL_VERTEX_SHADER);
	}

	void add_source_gs (std::string filename)
	{
		add_source (filename, GL_GEOMETRY_SHADER);
	}

	void add_source_fs (std::string filename)
	{
		add_source (filename, GL_FRAGMENT_SHADER);
	}

	void link () {
		if (m_linked)
			return;

		m_linked = true;

		if (!m_shaderProg)
			m_shaderProg = glCreateProgram ();

		glLinkProgram (m_shaderProg);

		int success;
		glGetProgramiv (m_shaderProg, GL_LINK_STATUS, &success);

		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog (m_shaderProg, 512, NULL, infoLog);
			THROW("SHADER_PROGRAM::LINK_FAILED for shaders\n" <<
			      m_shaderNames << "\n" <<
			      infoLog);
		}
	}

	void set_uniform (const char* name, const glm::mat4& m) const
	{
    	glUniformMatrix4fv(glGetUniformLocation(m_shaderProg, name),
    	                   1, GL_FALSE, glm::value_ptr(m));
	}

private:
	uint m_shaderProg;
	std::vector<uint>			m_shaders;
	std::vector<std::string>	m_shaderNames;
	std::vector<GLenum>			m_shaderTypes;
	bool m_linked;
};

#endif	//__H__shader

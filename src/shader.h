#ifndef __H__shader
#define __H__shader

#include <memory>
#include <string>

#include <glad/glad.h>	// include before other OpenGL related includes

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <glm/gtc/type_ptr.hpp>

#include "cond_throw.h"
#include "file_util.h"
#include "config.h"
#include "view.h"

namespace lumeview {

class Shader
{
public:
	Shader ()
	{
		m_shaderData = std::make_shared <ShaderData> ();
	}

	explicit operator bool () const	{return data().m_shaderProg != 0;}
	bool operator ! () const		{return data().m_shaderProg == 0;}
	
	void set_view (const View& view) {
	    set_uniform ("view", view.view_matrix());
	    set_uniform ("projection", view.projection_matrix());
	}

	void use () {
		if (!data().m_linked)
			link ();
		glUseProgram (data().m_shaderProg);
	}

	void use () const {
		COND_THROW (!data().m_linked, "The shader has not been linked yet, and thus can't be used.");
		glUseProgram (data().m_shaderProg);
	}

	uint get_uniform_location (const char* name) const
	{
		return glGetUniformLocation(data().m_shaderProg, name);
	}

	void add_source (std::string filename, GLenum shaderType) {
		if (!data().m_shaderProg)
			data().m_shaderProg = glCreateProgram ();

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

		glAttachShader (data().m_shaderProg, shader);
		data().m_linked = false;
		data().m_shaders.push_back (shader);
		data().m_shaderNames.push_back (filename);
		data().m_shaderTypes.push_back (shaderType);
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
		if (data().m_linked)
			return;

		data().m_linked = true;

		if (!data().m_shaderProg)
			data().m_shaderProg = glCreateProgram ();

		glLinkProgram (data().m_shaderProg);

		int success;
		glGetProgramiv (data().m_shaderProg, GL_LINK_STATUS, &success);

		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog (data().m_shaderProg, 512, NULL, infoLog);
			THROW("SHADER_PROGRAM::LINK_FAILED for shaders\n" <<
			      data().m_shaderNames << "\n" <<
			      infoLog);
		}
	}

	void set_uniform (const char* name, const float v) const
	{
    	glUniform1f(glGetUniformLocation(data().m_shaderProg, name), v);
	}

	void set_uniform (const char* name, const glm::vec4& v) const
	{
    	glUniform4fv(glGetUniformLocation(data().m_shaderProg, name),
    	             1, glm::value_ptr(v));
	}

	void set_uniform (const char* name, const glm::mat4& m) const
	{
    	glUniformMatrix4fv(glGetUniformLocation(data().m_shaderProg, name),
    	                   1, GL_FALSE, glm::value_ptr(m));
	}

private:
	struct ShaderData {
		ShaderData () : m_shaderProg (0), m_linked (false)
		{
		}

		// ShaderData (ShaderData&& s) :
		// 	m_shaderProg (std::exchange (s.m_shaderProg, 0)),
		// 	m_shaders (std::exchange (s.m_shaders, std::vector<uint>())),
		// 	m_shaderNames (std::move (s.m_shaderNames)),
		// 	m_shaderTypes (std::move (s.m_shaderTypes)),
		// 	m_linked (std::move (s.m_linked))
		// {}

		~ShaderData()
		{
			if (m_shaderProg)
				glDeleteProgram (m_shaderProg);

			for(auto shader : m_shaders) {
				glDeleteShader (shader);
			}
		}

		uint m_shaderProg;
		std::vector<uint>			m_shaders;
		std::vector<std::string>	m_shaderNames;
		std::vector<GLenum>			m_shaderTypes;
		bool m_linked;
	};

	ShaderData& data ()					{return *m_shaderData;}
	const ShaderData& data () const		{return *m_shaderData;}

	std::shared_ptr <ShaderData>	m_shaderData;
};

}// end of namespace lumeview

#endif	//__H__shader

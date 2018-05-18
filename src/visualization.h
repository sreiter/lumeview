#ifndef __H__visualization
#define __H__visualization

#include <map>
#include <memory>
#include <vector>

#include "gl_buffer.h"
#include "shader.h"
#include "mesh/mesh.h"

enum ShaderPreset {
	NONE,
	SOLID_FLAT,
	WIRE_NONE
};


class Visualization {
public:
	Visualization()	{}

	Visualization(std::string shaderPath) :
		m_shaderPath (std::move (shaderPath))
	{
	}

	Visualization (const Visualization&) = delete;

	void add_stage (std::string name,
	                std::shared_ptr <msh::Mesh> mesh)
	{
		Stage newStage;
		newStage.mesh = mesh;

	//	create the vertex array object for this stage
		// glGenVertexArrays (1, &newStage.vao);
		glBindVertexArray (newStage.vao);

	//	check whether we can reuse buffer objects
		for(auto& stage : m_stages) {
			if (stage.mesh->vrts() == mesh->vrts()) {
				newStage.vrtBuf = stage.vrtBuf;
			}

			if (stage.mesh == mesh) {
				newStage.vrtBuf = stage.vrtBuf;
				newStage.indBuf = stage.indBuf;
				break;
			}
		}

		if (newStage.vrtBuf){
			newStage.vrtBuf->bind ();
			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (0);
		}
		else {
			newStage.vrtBuf = std::make_shared <GLBuffer> (GL_ARRAY_BUFFER);
			newStage.vrtBuf->set_data (mesh->vrts()->raw_coords(),
			                           sizeof(real_t) * mesh->vrts()->num_coords());
			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (0);
		}

		if (newStage.indBuf)
			newStage.indBuf->bind ();
		else {
			newStage.indBuf = std::make_shared <GLBuffer> (GL_ELEMENT_ARRAY_BUFFER);
			newStage.indBuf->set_data (mesh->raw_inds(),
			                           sizeof(index_t) * mesh->num_inds());
		}

	//todo: create shader
		switch (newStage.mesh->grob_type()) {
			case msh::EDGE:
				newStage.primType = GL_LINES;
				newStage.shader = get_shader (WIRE_NONE);
				break;
			
			case msh::TRI:
				newStage.primType = GL_TRIANGLES;
				newStage.shader = get_shader (SOLID_FLAT);
				break;
			default:
				THROW("Visualization::ad_stage: Unsupported grid object type in specified mesh.");
		}

		newStage.name = std::move (name);
		m_stages.push_back (std::move(newStage));
	}

	void render (const View& view)
	{
		for(auto& stage : m_stages) {
			stage.shader.use ();
			view.apply (stage.shader);
			glBindVertexArray (stage.vao);
			glDrawElements (stage.primType, stage.mesh->num_inds(), GL_UNSIGNED_INT, 0);
		}
	}

// private:
	Shader get_shader (ShaderPreset preset)
	{
		Shader& s = m_shaders [preset];
		if (s)
			return s;

		switch (preset) {
			case NONE:
				return s;

			case SOLID_FLAT:
				s.add_source_vs (m_shaderPath + "vertex-shader-0.vs");
				s.add_source_gs (m_shaderPath + "flat-shading.gs");
				s.add_source_fs (m_shaderPath + "light-intensity.fs");
				s.link();
				return s;

			case WIRE_NONE:
				s.add_source_vs (m_shaderPath + "vertex-shader-0.vs");
				s.add_source_fs (m_shaderPath + "wireframe.fs");
				s.link();
				return s;

			default:
				THROW ("Shader preset " << preset << " not yet supported.");
		};

		return s;
	}


	struct Stage {
		Stage ()	{glGenVertexArrays (1, &vao);}
		Stage (const Stage&) = delete;
		Stage (Stage&& s) :
			name (std::move (s.name)),
			mesh (std::move (s.mesh)),
			shader (std::move (s.shader)),
			vao (std::exchange (s.vao, 0)),
			vrtBuf (std::move (s.vrtBuf)),
			indBuf (std::move (s.indBuf)),
			primType (std::move (s.primType))
		{}

		~Stage ()	{if (vao) glDeleteVertexArrays (1, &vao);}

		std::string 				name;
		std::shared_ptr <msh::Mesh>	mesh;
		Shader						shader;

		uint 						vao;
		std::shared_ptr <GLBuffer>	vrtBuf;
		std::shared_ptr <GLBuffer>	indBuf;
		GLenum						primType;
	};

	std::vector <Stage> 			m_stages;
	std::map <ShaderPreset, Shader>	m_shaders;
	std::string						m_shaderPath;

};

#endif	//__H__visualization

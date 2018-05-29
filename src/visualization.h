#ifndef __H__visualization
#define __H__visualization

#include <map>
#include <memory>
#include <vector>

#include "gl_buffer.h"
#include "shader.h"
#include "mesh.h"

enum ShaderHint {
	NONE		= 0,
	FLAT		= 1,
	SMOOTH		= 1 << 1,
	LINES		= 1 << 2,
	TRIANGLES	= 1 << 3
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
	                std::shared_ptr <msh::Mesh> mesh,
	                msh::grob_t grobType,
	                uint shaderHints)
	{
		Stage newStage;
		newStage.mesh = mesh;

		//	create the vertex array object for this stage
		glBindVertexArray (newStage.vao);

		const bool curMeshNeedsVrtNormals =
					(shaderHints & SMOOTH)
				||	(grobType == msh::EDGE && (shaderHints & FLAT));

		COND_THROW(curMeshNeedsVrtNormals && !mesh->has_data<real_t>("vrtNormals"),
		           "Requested shader needs normal information!");

		//	check whether we can reuse buffer objects
		for(auto& stage : m_stages) {
			if (stage.mesh->coords() == mesh->coords()) {
				newStage.coordBuf = stage.coordBuf;
			}

			if (curMeshNeedsVrtNormals && stage.mesh->has_data<real_t>("vrtNormals")
			    && stage.mesh->data<real_t>("vrtNormals") == mesh->data<real_t>("vrtNormals"))
			{
				newStage.normBuf = stage.normBuf;
			}

			if (stage.mesh == mesh && stage.grobType == grobType) {
				newStage.indBuf = stage.indBuf;
				break;
			}
		}

		//	coordinates
		if (newStage.coordBuf){
			newStage.coordBuf->bind ();
			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (0);
		}
		else {
			newStage.coordBuf = std::make_shared <GLBuffer> (GL_ARRAY_BUFFER);
			newStage.coordBuf->set_data (mesh->coords()->raw_data(),
			                           sizeof(real_t) * mesh->num_coords());
			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (0);
		}

		//	normals
		if (newStage.normBuf){
			newStage.normBuf->bind ();
			glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (1);
		}
		else if (curMeshNeedsVrtNormals){
			newStage.normBuf = std::make_shared <GLBuffer> (GL_ARRAY_BUFFER);
			newStage.normBuf->set_data (mesh->data<real_t>("vrtNormals")->raw_data(),
			                            sizeof(real_t) * mesh->data<real_t>("vrtNormals")->size());
			glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (1);
		}
		else
			glDisableVertexAttribArray (1);

		//	indices
		if (newStage.indBuf)
			newStage.indBuf->bind ();
		else {
			newStage.indBuf = std::make_shared <GLBuffer> (GL_ELEMENT_ARRAY_BUFFER);
			newStage.indBuf->set_data (mesh->inds(grobType)->raw_data(),
			                           sizeof(index_t) * mesh->inds(grobType)->size());
		}

	//todo: create shader
		switch (grobType) {
			case msh::EDGE:
				newStage.primType = GL_LINES;
				newStage.shader = get_shader (shaderHints | ShaderHint::LINES);
				newStage.color = glm::vec4 (0.2f, 0.2f, 1.0f, 0.5f);
				newStage.zfac = 0.99f;
				break;
			
			case msh::TRI:
				newStage.primType = GL_TRIANGLES;
				newStage.shader = get_shader (shaderHints | ShaderHint::TRIANGLES);
				newStage.color = glm::vec4 (1.0f, 1.0f, 1.0f, 1.0f);
				newStage.zfac = 1.0f;
				break;
			default:
				THROW("Visualization::add_stage: Unsupported grid object type in specified mesh.");
		}

		newStage.numInds = mesh->inds(grobType)->size();
		newStage.name = std::move (name);
		newStage.grobType = grobType;
		m_stages.push_back (std::move(newStage));
	}

	void render (const View& view)
	{
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for(auto& stage : m_stages) {
			stage.shader.use ();
			view.apply (stage.shader);
			stage.shader.set_uniform("color", stage.color);
			stage.shader.set_uniform("zfac", stage.zfac);
			glBindVertexArray (stage.vao);
			glDrawElements (stage.primType, stage.numInds, GL_UNSIGNED_INT, 0);
		}
	}

// private:
	Shader get_shader (uint preset)
	{
		Shader& s = m_shaders [preset];
		if (s)
			return s;

		if (preset & TRIANGLES) {
			if (preset & SMOOTH){
				s.add_source_vs (m_shaderPath + "smooth-shading.vs");
				s.add_source_fs (m_shaderPath + "smooth-shading.fs");
			}
			else if (preset & FLAT){
				s.add_source_vs (m_shaderPath + "no-shading.vs");
				s.add_source_gs (m_shaderPath + "flat-tri-shading.gs");
				s.add_source_fs (m_shaderPath + "flat-shading.fs");
			}
			else{
				s.add_source_vs (m_shaderPath + "no-shading.vs");
				s.add_source_fs (m_shaderPath + "smooth-shading.fs");
			}

			s.link();
			return s;
		}

		if (preset & LINES) {
			if (preset & SMOOTH){
				s.add_source_vs (m_shaderPath + "smooth-shading.vs");
				s.add_source_fs (m_shaderPath + "smooth-shading.fs");
			}
			else if (preset & FLAT){
				s.add_source_vs (m_shaderPath + "smooth-shading.vs");
				s.add_source_gs (m_shaderPath + "flat-edge-shading.gs");
				s.add_source_fs (m_shaderPath + "flat-shading.fs");
			}
			else{
				s.add_source_vs (m_shaderPath + "no-shading.vs");
				s.add_source_fs (m_shaderPath + "smooth-shading.fs");
			}

			s.link();
			return s;
		}

		THROW ("Shader preset " << preset << " not yet supported.");

		return s;
	}


	struct Stage {
		Stage ()	{glGenVertexArrays (1, &vao);}
		Stage (const Stage&) = delete;
		Stage (Stage&& s) :
			name (std::move (s.name)),
			mesh (std::move (s.mesh)),
			shader (std::move (s.shader)),
			color (std::move (s.color)),
			zfac (std::move (s.zfac)),
			vao (std::exchange (s.vao, 0)),
			coordBuf (std::move (s.coordBuf)),
			normBuf (std::move (s.normBuf)),
			indBuf (std::move (s.indBuf)),
			primType (std::move (s.primType)),
			numInds (std::move (s.numInds)),
			grobType (std::move (s.grobType))
		{}

		~Stage ()	{if (vao) glDeleteVertexArrays (1, &vao);}

		std::string 				name;
		std::shared_ptr <msh::Mesh>	mesh;
		Shader						shader;
		glm::vec4					color;
		float						zfac;

		uint 						vao;
		std::shared_ptr <GLBuffer>	coordBuf;
		std::shared_ptr <GLBuffer>	normBuf;
		std::shared_ptr <GLBuffer>	indBuf;
		GLenum						primType;
		GLsizei						numInds;
		msh::grob_t					grobType;
	};

	std::vector <Stage> 			m_stages;
	std::map <uint, Shader>			m_shaders;
	std::string						m_shaderPath;

};

#endif	//__H__visualization

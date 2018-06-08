// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <limits>
#include <algorithm>

#include "visualization.h"
#include "imgui/imgui.h"


namespace slimesh {

Visualization::
Visualization()	{}

Visualization::
Visualization(std::string shaderPath) :
	m_shaderPath (std::move (shaderPath))
{
}


void Visualization::
add_stage (	std::string name,
            std::shared_ptr <Mesh> mesh,
            grob_t grobType,
            uint shaderHints)
{
	COND_THROW (!mesh->has_inds (grobType),
	            "Requested grob type '" << GrobName (grobType)
	            << "' is not provided by the specified mesh.");
	
	Stage newStage;
	newStage.mesh = mesh;

	//	create the vertex array object for this stage
	glBindVertexArray (newStage.vao);

	const bool curMeshNeedsVrtNormals =
				(shaderHints & SMOOTH)
			||	(grobType == EDGE && (shaderHints & FLAT));

	COND_THROW(curMeshNeedsVrtNormals && !mesh->has_data<real_t>("vrtNormals"),
	           "Requested shader needs normal information!");

	//	check whether we can reuse buffer objects
	for(auto& stage : m_stages) {
		if (stage.mesh->coords() == mesh->coords()) {
			newStage.coordBuf = stage.coordBuf;
			newStage.bndSphere = stage.bndSphere;
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
		newStage.bndSphere = SphereFromCoords (UNPACK_DST(*mesh->coords()));
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
		case EDGE:
			newStage.primType = GL_LINES;
			newStage.shader = get_shader (shaderHints | ShaderHint::LINES);
			newStage.color = glm::vec4 (0.2f, 0.2f, 1.0f, 0.5f);
			newStage.zfacNear = 0.99f;
			newStage.zfacFar = 0.9999999f;
			break;
		
		case TRI:
			newStage.primType = GL_TRIANGLES;
			newStage.shader = get_shader (shaderHints | ShaderHint::TRIANGLES);
			newStage.color = glm::vec4 (1.0f, 1.0f, 1.0f, 1.0f);
			newStage.zfacNear = 1.0f;
			newStage.zfacFar = 1.0f;
			break;
		default:
			THROW("Visualization::add_stage: Unsupported grid object type in specified mesh.");
	}

	newStage.numInds = mesh->inds(grobType)->size();
	newStage.name = std::move (name);
	newStage.grobType = grobType;
	m_stages.push_back (std::move(newStage));
}


glm::vec2 Visualization::
estimate_z_clip_dists (const View& view) const
{
	using std::min;
	using std::max;

	const glm::vec3 n = normalize (view.camera().forward());
	const glm::vec3 p = view.camera().from();

	glm::vec2 zDist (std::numeric_limits<real_t>::max(), 0);

	for(const auto& stage : m_stages) {
		const Sphere& s = stage.bndSphere;
		const real_t o = offsetPointToPlane (s.center, p, n);
		zDist.x = min (zDist.x, o - s.radius);
		zDist.y = max (zDist.y, o + s.radius);
	}

	if (zDist.y <= 0)
		zDist.y = 1.f;

	if (zDist.x <= 0 || zDist.x >= zDist.y)
		zDist.x = zDist.y * 1.e-5f;

	return zDist * glm::vec2(0.9f, 1.1f);
}


void Visualization::
render (const View& view)
{
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(auto& stage : m_stages) {
		stage.shader.use ();
		view.apply (stage.shader);
		stage.shader.set_uniform("color", stage.color);
		stage.shader.set_uniform("zfacNear", stage.zfacNear);
		stage.shader.set_uniform("zfacFar", stage.zfacFar);
		glBindVertexArray (stage.vao);
		glDrawElements (stage.primType, stage.numInds, GL_UNSIGNED_INT, 0);
	}
}


Shader Visualization::
get_shader (uint preset)
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


void Visualization::do_imgui ()
{

}
}//	end of namespace slimesh
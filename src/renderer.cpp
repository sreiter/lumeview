// This file is part of lumeview, a lightweight viewer for unstructured meshes
//
// Copyright (C) 2018 Sebastian Reiter
// Copyright (C) 2018 G-CSC, Goethe University Frankfurt
// Author: Sebastian Reiter <s.b.reiter@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <limits>
#include <algorithm>

#include "config.h"
#include "renderer.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

using namespace lume;

namespace lumeview {

static const char* shadingNames[] = {"none", "flat", "smooth"};

Renderer::
Renderer() :
	m_shaderPath (SHADER_PATH)
{}

Renderer::
Renderer(std::string shaderPath) :
	m_shaderPath (std::move (shaderPath))
{
}

void Renderer::
clear ()
{
	m_stages.clear();
}

void Renderer::
add_stage (	std::string name,
            SPMesh mesh,
            const GrobSet grobSet,
            ShadingPreset shading)
{
	COND_THROW (!mesh->has (grobSet),
	            "Requested grob type '" << grobSet.name()
	            << "' is not provided by the specified mesh.");

	Stage newStage;
	newStage.mesh = mesh;

	switch (grobSet.type()) {
		case EDGES:
			newStage.primType = GL_LINES;
			newStage.color = glm::vec4 (0.2f, 0.2f, 1.0f, 0.5f);
			newStage.zfacNear = 0.99f;
			newStage.zfacFar = 1.f;
			newStage.numInds = mesh->num_indices (EDGE);
			break;
		
		case TRIS:
		case QUADS:
		case FACES:
			newStage.primType = GL_TRIANGLES;
			newStage.color = glm::vec4 (1.0f, 1.0f, 1.0f, 1.0f);
			newStage.zfacNear = 1.0f;
			newStage.zfacFar = 1.0f;
			newStage.numInds = mesh->num_indices(TRI)
					 		 + 3 * mesh->num_indices(QUAD) / 2;
			break;
		default:
			THROW("Renderer::add_stage: Unsupported grid object type in specified mesh.");
	}

	newStage.shadingPreset = shading;
	newStage.name = std::move (name);
	newStage.grobSet = grobSet;
	m_stages.push_back (std::move(newStage));
}

Renderer::Stage&
Renderer::stage (int stageInd)
{
	if (stageInd < 0)
		return m_stages.at ((int)m_stages.size() + stageInd);
	return m_stages.at (stageInd);
}

const Renderer::Stage&
Renderer::stage (int stageInd) const
{
	if (stageInd < 0)
		return m_stages.at ((int)m_stages.size() + stageInd);
	return m_stages.at (stageInd);
}

void Renderer::
stage_set_color (const glm::vec4& color, int stageInd)
{
	if (stageInd < 0)
		stageInd = (int)m_stages.size() + stageInd;
	stage (stageInd).color = color;
}

glm::vec2 Renderer::
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

	if (zDist.x < zDist.y * 1.e-5f || zDist.x >= zDist.y)
		zDist.x = zDist.y * 1.e-5f;

	return zDist * glm::vec2(0.9f, 1.1f);
}

void Renderer::
prepare_buffers ()
{
	for(size_t istage = 0; istage < m_stages.size(); ++istage) {
		Stage& curStage = m_stages[istage];
		SPMesh mesh = curStage.mesh;

		//	create the vertex array object for this stage
		glBindVertexArray (curStage.vao);

		const bool curMeshNeedsVrtNormals =
					(curStage.shadingPreset == SMOOTH)
				||	(curStage.grobSet.type() == EDGES && (curStage.shadingPreset == FLAT));

		COND_THROW(curMeshNeedsVrtNormals && !mesh->has_annex<RealArrayAnnex>("normals", VERTEX),
		           "Requested shader needs normal information!");

		//	check whether we can reuse buffer objects
		for(size_t iOtherStage = 0; iOtherStage < istage; ++iOtherStage) {
			Stage& stage = m_stages[iOtherStage];

			if (stage.mesh->coords() == mesh->coords()) {
				curStage.coordBuf = stage.coordBuf;
				curStage.bndSphere = stage.bndSphere;
			}

			if (curMeshNeedsVrtNormals && stage.mesh->has_annex<RealArrayAnnex>("normals", VERTEX)
			    && stage.mesh->annex<RealArrayAnnex>("normals", VERTEX) == mesh->annex<RealArrayAnnex>("normals", VERTEX))
			{
				curStage.normBuf = stage.normBuf;
			}

			if (stage.mesh == mesh && stage.grobSet == curStage.grobSet) {
				curStage.indBuf = stage.indBuf;
				break;
			}
		}

		//	coordinates
		if (curStage.coordBuf){
			curStage.coordBuf->bind ();
			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (0);
		}
		else {
			curStage.bndSphere = SphereFromCoords (UNPACK_DST(*mesh->coords()));
			curStage.coordBuf = std::make_shared <GLBuffer> (GL_ARRAY_BUFFER);
			curStage.coordBuf->set_data (mesh->coords()->raw_ptr(),
			                           sizeof(real_t) * mesh->num_coords());
			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (0);
		}

		//	normals
		if (curStage.normBuf){
			curStage.normBuf->bind ();
			glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (1);
		}
		else if (curMeshNeedsVrtNormals){
			curStage.normBuf = std::make_shared <GLBuffer> (GL_ARRAY_BUFFER);
			curStage.normBuf->set_data (mesh->annex<RealArrayAnnex>("normals", VERTEX)->raw_ptr(),
			                            sizeof(real_t) * mesh->annex<RealArrayAnnex>("normals", VERTEX)->size());
			glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray (1);
		}
		else
			glDisableVertexAttribArray (1);

		//	indices
		if (curStage.indBuf)
			curStage.indBuf->bind ();
		else {
			curStage.indBuf = std::make_shared <GLBuffer> (GL_ELEMENT_ARRAY_BUFFER);

			curStage.indBuf->set_size (uint (curStage.numInds * sizeof (index_t)));
			const GrobSet gs = curStage.grobSet;
			
			uint fill = 0;
			for(auto gt : gs) {
				if (mesh->grobs(gt).empty())
					continue;

				switch (gt) {
					case EDGE:
					case TRI:
						curStage.indBuf->set_sub_data (
						                fill,
						                mesh->grobs(gt).raw_ptr(),
			                        	uint (sizeof(index_t) * mesh->grobs(gt).num_indices()));
						fill += uint (sizeof(index_t) * mesh->grobs(gt).num_indices());
						break;

					case QUAD: {
						const index_t numQuads = mesh->grobs(gt).size();
						const index_t numQuadInds = mesh->grobs(gt).num_indices();
						std::vector <index_t> tris;
						tris.reserve (numQuads * 6);

						const index_t* quads = mesh->grobs(gt).raw_ptr();
						for(index_t i = 0; i < numQuadInds; i += 4) {
							tris.push_back (quads[i]);
							tris.push_back (quads[i + 1]);
							tris.push_back (quads[i + 2]);

							tris.push_back (quads[i + 3]);
							tris.push_back (quads[i]);
							tris.push_back (quads[i + 2]);
						}

						curStage.indBuf->set_sub_data (
						                fill,
						                &tris.front(),
			                        	uint (sizeof(index_t) * tris.size()));
						fill += uint (sizeof(index_t) * tris.size());
					}	break;
				}
			}
		}
	}
}

void Renderer::
render (const View& view)
{
	prepare_buffers ();

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);

	for(auto& stage : m_stages) {
		Shader shader = get_shader (stage.grobSet, stage.shadingPreset);
		view.apply ();
		shader.use ();
		shader.set_view (view);
		shader.set_uniform("color", stage.color);
		shader.set_uniform("zfacNear", stage.zfacNear);
		shader.set_uniform("zfacFar", stage.zfacFar);
		glBindVertexArray (stage.vao);
		glDrawElements (stage.primType, stage.numInds, GL_UNSIGNED_INT, 0);
	}
}


Shader Renderer::
get_shader (const GrobSet grobSet, ShadingPreset shading)
{
	COND_THROW (grobSet.size() == 0, "Invalid grob set specified: " << grobSet.name());

	const grob_t grobType = grobSet.grob_type (0);
	Shader& s = m_shaders [grobType] [shading];
	if (s)
		return s;

	switch (grobType) {
		case TRI: {
			if (shading == SMOOTH){
				s.add_source_vs (m_shaderPath + "smooth-shading.vs");
				s.add_source_fs (m_shaderPath + "smooth-shading.fs");
			}
			else if (shading == FLAT){
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
		} break;

		case EDGE: {
			if (shading == SMOOTH){
				s.add_source_vs (m_shaderPath + "smooth-shading.vs");
				s.add_source_fs (m_shaderPath + "smooth-shading.fs");
			}
			else if (shading == FLAT){
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
		} break;
	}

	THROW ("Shading preset " << shading << " not yet supported for grob type " << GrobName(grobType));

	return s;
}


void Renderer::do_imgui (bool* pOpened)
{
	ImGui::SetNextWindowSize(ImVec2(300,500), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Renderer", pOpened, 0)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    for(auto& stage : m_stages) {
	    if (ImGui::TreeNode(stage.name.c_str())) {
	    	ImGui::ColorEdit4("color", glm::value_ptr(stage.color), ImGuiColorEditFlags_NoInputs);

	    	int index = stage.shadingPreset;
	    	ImGui::Combo ("shading", &index, shadingNames, IM_ARRAYSIZE(shadingNames));
	    	stage.shadingPreset = ShadingPreset(index);

	        ImGui::TreePop();
	    }
    }

    ImGui::End(); // Renderer
}
}//	end of namespace lumeview

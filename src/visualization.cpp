// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <limits>
#include <algorithm>

#include "visualization.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>


namespace slimesh {

static const char* shadingNames[] = {"none", "flat", "smooth"};

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
			break;
		
		case TRIS:
		case QUADS:
		case FACES:
			newStage.primType = GL_TRIANGLES;
			newStage.color = glm::vec4 (1.0f, 1.0f, 1.0f, 1.0f);
			newStage.zfacNear = 1.0f;
			newStage.zfacFar = 1.0f;
			break;
		default:
			THROW("Visualization::add_stage: Unsupported grid object type in specified mesh.");
	}

	newStage.shadingPreset = shading;
	newStage.numInds = mesh->num_inds (EDGE) + mesh->num_inds(TRI) + 3 * mesh->num_inds(QUAD) / 2;
	newStage.name = std::move (name);
	newStage.grobSet = grobSet;
	m_stages.push_back (std::move(newStage));
}

void Visualization::
stage_set_color (const glm::vec4& color, int stageInd)
{
	if (stageInd < 0)
		stageInd = (int)m_stages.size() + stageInd;
	m_stages.at (stageInd).color = color;
}

// void Visualization::
// provide_shading_requirements (Stage& stage)
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

	if (zDist.x < zDist.y * 1.e-5f || zDist.x >= zDist.y)
		zDist.x = zDist.y * 1.e-5f;

	return zDist * glm::vec2(0.9f, 1.1f);
}

void Visualization::
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
				if (mesh->inds(gt)->empty())
					continue;

				switch (gt) {
					case EDGE:
					case TRI:
						curStage.indBuf->set_sub_data (
						                fill,
						                mesh->inds(gt)->underlying_array().raw_ptr(),
			                        	uint (sizeof(index_t) * mesh->inds(gt)->num_indices()));
						fill += uint (sizeof(index_t) * mesh->inds(gt)->num_indices());
						break;

					case QUAD: {
						const index_t numQuads = mesh->inds(gt)->size();
						const index_t numQuadInds = mesh->inds(gt)->num_indices();
						std::vector <index_t> tris;
						tris.reserve (numQuads * 6);

						const index_t* quads = mesh->inds(gt)->underlying_array().raw_ptr();
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

void Visualization::
render (const View& view)
{
	prepare_buffers ();

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);

	for(auto& stage : m_stages) {
		Shader shader = get_shader (stage.grobSet, stage.shadingPreset);
		shader.use ();
		view.apply (shader);
		shader.set_uniform("color", stage.color);
		shader.set_uniform("zfacNear", stage.zfacNear);
		shader.set_uniform("zfacFar", stage.zfacFar);
		glBindVertexArray (stage.vao);
		glDrawElements (stage.primType, stage.numInds, GL_UNSIGNED_INT, 0);
	}
}


Shader Visualization::
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


void Visualization::do_imgui (bool* pOpened)
{
	ImGui::SetNextWindowSize(ImVec2(300,500), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Visualization", pOpened, 0)) {
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

    ImGui::End(); // Visualization
}
}//	end of namespace slimesh

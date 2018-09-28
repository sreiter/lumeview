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

#include <algorithm>
#include "subset_visualization.h"
#include "lume/annex_table.h"
#include "lume/normals.h"
#include "lume/rim_mesh.h"
#include "lume/subset_info_annex.h"
#include "lume/topology.h"
#include "subset_info_annex_message.h"

using namespace std;
using namespace lume;

namespace lumeview {

SubsetVisualization::SubsetVisualization () :
	m_refreshRequired (false)
{
}

SubsetVisualization::SubsetVisualization (lume::SPMesh mesh) :
	m_refreshRequired (false)
{
	set_mesh (mesh);
}


void SubsetVisualization::set_mesh (lume::SPMesh mesh)
{
	m_mesh = mesh;
	GrobSet grobSet = m_mesh->grob_set_type_of_highest_dim ();
	if (grobSet == CELLS)
		m_neighborhoods.refresh (mesh, FACES, CELLS);
	refresh ();
}



void SubsetVisualization::refresh ()
{
	m_refreshRequired = false;
	m_subsetInfo.reset();
	m_renderer.clear();

	if (!m_mesh)
		return;

	GrobSet grobSet = m_mesh->grob_set_type_of_highest_dim ();
		
	refresh_subset_info_annex_name ();

	if (m_subsetAnnexName.empty())
		throw (NoSubsetAnnexError ("Could not find any subset annex"));

	for(auto gt : grobSet) {
		if (!m_mesh->has (gt))
			continue;

		COND_THROW (!m_mesh->has_annex <IndexArrayAnnex> (m_subsetAnnexName, gt),
		            "Provided mesh does not provide an IndexArrayAnnex with "
		            "name '" << m_subsetAnnexName
		            << "' for the requested grob type " << GrobName (gt));

		COND_THROW (m_mesh->annex <IndexArrayAnnex> (m_subsetAnnexName, gt)->size() != m_mesh->num (gt),
		            "subset annex '" << m_subsetAnnexName << " 'has wrong size ("
		            << m_mesh->annex <IndexArrayAnnex> (m_subsetAnnexName, gt)->size()
		            << ") for grob type " << GrobName (gt) << ". Expected was "
		            << m_mesh->num (gt));
	}

	m_subsetInfo = m_mesh->annex<SubsetInfoAnnex> (m_subsetAnnexName, NO_GROB);

	create_subset_meshes (grobSet);
	prepare_renderer ();
}

void SubsetVisualization::create_subset_meshes (const GrobSet grobSet)
{
	if (grobSet == CELLS)
		subset_meshes_from_cell_rim ();
	else
		subset_meshes_from_grobs (m_mesh, grobSet, false);
}

void SubsetVisualization::prepare_renderer ()
{
	const glm::vec4 wireColor (0.2f, 0.2f, 0.2f, 1.0f);

	index_t subsetIndex = 0;
	m_subsetIndexToStageIndex.clear ();
	for(auto mesh : m_subsetMeshes) {

		m_subsetIndexToStageIndex.push_back ((int)m_renderer.num_stages());

		if (mesh->has (FACES)) {
			ComputeFaceVertexNormals3 (*mesh, "normals");
			CreateSideGrobs (*mesh, 1);

			m_renderer.add_stage ("solid", mesh, FACES, FLAT);
			m_renderer.stage_set_color (subset_color (subsetIndex));

			m_renderer.add_stage ("wire", mesh, EDGES, FLAT);
			m_renderer.stage_set_color (wireColor);
		}
		++subsetIndex;
	}
}

void SubsetVisualization::refresh_subset_info_annex_name ()
{
	if (!m_mesh->has_annex <SubsetInfoAnnex> (m_subsetAnnexName, NO_GROB)) {
		m_subsetAnnexName = "";
		for(auto iannex = m_mesh->annex_begin (); iannex != m_mesh->annex_end(); ++iannex) {
			if (iannex->first.grobType == NO_GROB
			    && dynamic_pointer_cast<SubsetInfoAnnex> (iannex->second))
			{
				m_subsetAnnexName = iannex->first.name;
				break;
			}
		}
	}
}

void SubsetVisualization::render (const View& view)
{
	if (m_refreshRequired)
		refresh ();
	m_renderer.render (view);
}



glm::vec2 SubsetVisualization::estimate_z_clip_dists (const View& view) const
{
	return m_renderer.estimate_z_clip_dists (view);
}


void SubsetVisualization::
subset_meshes_from_grobs (SPMesh mesh, const GrobSet grobSet, bool ignoreSubsetVisibilities)
{
//	reuse subset meshes to avoid reallocations
	for (auto& m : m_subsetMeshes)
		m->clear_grobs ();

	int maxSI = -1;

	for(auto grobType : grobSet) {
		auto pinds = mesh->optional_annex <IndexArrayAnnex> (m_subsetAnnexName, grobType);
		if (!pinds)
			continue;

		auto& inds = *pinds;
		COND_THROW (inds.size () != mesh->num (grobType),
		            "IndexArrayAnnex of subset indices has wrong size: " << inds.size()
		            << " expected: " << mesh->num (grobType) << ")");

		index_t counter = 0;
		for(auto grob : mesh->grobs (grobType)) {
			const index_t si = inds [counter];
			maxSI = max (maxSI, (int)si);
			if (ignoreSubsetVisibilities || m_subsetInfo->subset_properties (si).visible)
				subset_mesh (si).insert (grob);
			++counter;
		}
	}

//	make sure that all subset meshes exist
//	first add missing subset meshes (e.g. if the last subset is not visible)
	for(int i = (int)m_subsetMeshes.size(); i <= maxSI; ++i)
		subset_mesh (i);
//	now remove subsets which were not required (e.g. because the number of subsets decreased)
	m_subsetMeshes.resize (maxSI + 1);
}


void SubsetVisualization::subset_meshes_from_cell_rim ()
{
	if (!m_rimMesh)
		m_rimMesh = make_shared <Mesh> ();
	m_rimMesh->clear_grobs ();
	m_rimMesh->set_annex (m_subsetAnnexName, NO_GROB, m_subsetInfo);

	//	extract the rim of the visible elements
	auto srcSubset = ArrayAnnexTable <IndexArrayAnnex> (m_mesh, m_subsetAnnexName, CELLS, false); // last param: createIfMissing==false
	auto rimSubset = ArrayAnnexTable <IndexArrayAnnex> (m_rimMesh, m_subsetAnnexName, FACES, true); // last param: createIfMissing==true
	rimSubset.clear_arrays ();

	auto isVisible = [subsetInfo=m_subsetInfo, &srcSubset] (const GrobIndex& srcGrob)
				 	 {return subsetInfo->subset_properties (srcSubset[srcGrob]).visible;};

	auto gotRimElem = [&rimSubset, &srcSubset] (const GrobIndex& rimGrob, const GrobIndex& srcGrob)
					  {rimSubset.annex(rimGrob.grobType)->push_back (srcSubset[srcGrob]);};

	CreateRimMesh (m_rimMesh, m_mesh, CELLS, isVisible, gotRimElem, &m_neighborhoods);

	subset_meshes_from_grobs (m_rimMesh, FACES, true);
}


Mesh& SubsetVisualization::subset_mesh (const index_t si)
{
	while (si >= m_subsetMeshes.size()) {
		m_subsetMeshes.push_back (make_shared<Mesh>());
		m_subsetMeshes.back()->set_coords (m_mesh->coords());
	}
	return *m_subsetMeshes[si];
}


void SubsetVisualization::
receive_message (const Message& msg)
{
	if (!m_subsetInfo || (msg.subject() != m_subsetInfo.get()))
		return;

	if (auto simsg = dynamic_cast <const SubsetInfoAnnexMessage*> (&msg)) {
		COND_THROW (m_subsetInfo.get() != simsg->subset_info_annex(),
		            "SubsetInfoAnnex instances do not match, even though the subject "
		            "of the message indicates that they should match");

		if (simsg->color_changed ()){
			const index_t si = simsg->subset_index();
			const int stageIndex = m_subsetIndexToStageIndex[si];
			m_renderer.stage_set_color (subset_color (si), stageIndex);
		}
		if (simsg->visibility_changed ())
			m_refreshRequired = true;
	}
}

glm::vec4 SubsetVisualization::subset_color (const index_t si) const
{
	if (m_subsetInfo) {
		auto c = m_subsetInfo->subset_properties (si).color;
		return glm::vec4 (c.r(), c.g(), c.b(), c.a());
	}
	return glm::vec4 (0.f);
}

bool SubsetVisualization::subset_visible (const index_t si) const
{
	if (m_subsetInfo)
		return m_subsetInfo->subset_properties (si).visible;

	return false;
}


}//	end of namespace lumeview

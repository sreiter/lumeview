// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <algorithm>
#include "subset_visualization.h"
#include "lume/normals.h"
#include "lume/subset_info_annex.h"
#include "lume/topology.h"

using namespace std;
using namespace lume;

namespace lumeview {

SubsetVisualization::SubsetVisualization ()
{
}


void SubsetVisualization::set_mesh (lume::SPMesh mesh)
{
	m_mesh = mesh;
	refresh ();
}



void SubsetVisualization::refresh ()
{
	m_renderer.clear();
	if (!m_mesh)
		return;

	GrobSet grobSet = m_mesh->grob_set_type_of_highest_dim ();
		
	refresh_subset_info_annex_name ();

	cout << "m_subsetAnnexName: " << m_subsetAnnexName << endl;

	if (m_subsetAnnexName.empty())
		return;

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

	if (grobSet == CELLS) {
		subset_meshes_from_cell_rim ();
	}
	else
		subset_meshes_from_grobs (grobSet);

//todo: render subset meshes
	cout << "m_subsetMeshes.size()" << m_subsetMeshes.size() << endl;

	const glm::vec4 wireColor (0.2f, 0.2f, 0.2f, 1.0f);
	auto subsetInfo = m_mesh->annex<SubsetInfoAnnex> (m_subsetAnnexName, NO_GROB);
	index_t counter = 0;
	for(auto mesh : m_subsetMeshes) {
		if (mesh->has (FACES)) {
			ComputeFaceVertexNormals3 (*mesh, "normals");
			CreateEdgeInds (*mesh);
			m_renderer.add_stage ("solid", mesh, FACES, FLAT);
			
			auto c = subsetInfo->subset_properties (counter).color;
			const glm::vec4 subsetColor (c.r(), c.g(), c.b(), c.a());
			m_renderer.stage_set_color (subsetColor);
			m_renderer.add_stage ("wire", mesh, EDGES, FLAT);
			m_renderer.stage_set_color (wireColor);
		}
		++counter;
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
	m_renderer.render (view);
}



glm::vec2 SubsetVisualization::estimate_z_clip_dists (const View& view) const
{
	return m_renderer.estimate_z_clip_dists (view);
}


void SubsetVisualization::subset_meshes_from_grobs (const GrobSet grobSet)
{
//	reuse subset meshes to avoid reallocations
	for (auto& m : m_subsetMeshes)
		m->clear_grobs ();

	int maxSI = -1;

	for(auto grobType : grobSet) {
		auto pinds = m_mesh->optional_annex <IndexArrayAnnex> (m_subsetAnnexName, grobType);
		if (!pinds)
			continue;

		auto& inds = *pinds;
		index_t counter = 0;
		for(auto grob : m_mesh->grobs (grobType)) {
			const index_t si = inds [counter];
			maxSI = max (maxSI, (int)si);
			cout << "inserting " << grob.desc().name() << " into subset mesh " << si << endl;
			subset_mesh (si).insert (grob);
			++counter;
		}
	}

	m_subsetMeshes.resize (maxSI + 1);
}


void SubsetVisualization::subset_meshes_from_cell_rim ()
{
	// m_neighborhood.refresh (m_mesh, FACES, CELLS);
}

Mesh& SubsetVisualization::subset_mesh (const index_t si)
{
	while (si >= m_subsetMeshes.size()) {
		m_subsetMeshes.push_back (make_shared<Mesh>());
		m_subsetMeshes.back()->set_coords (m_mesh->coords());
	}
	return *m_subsetMeshes[si];
}



}//	end of namespace lumeview

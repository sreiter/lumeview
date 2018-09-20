// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_subset_visualization
#define __H__lumeview_subset_visualization

#include "lume/mesh.h"
#include "lume/neighborhoods.h"
#include "renderer.h"
#include "visualization.h"


namespace lumeview {

class SubsetVisualization : public Visualization
{
public:
	SubsetVisualization ();
	void set_mesh (lume::SPMesh mesh);

	void refresh ();

	void render (const View& view) override;

	glm::vec2 estimate_z_clip_dists (const View& view) const override;

private:
	void subset_meshes_from_grobs (const lume::GrobSet grobSet);
	void subset_meshes_from_cell_rim ();
	void refresh_subset_info_annex_name ();
	lume::Mesh& subset_mesh (const index_t si);

	Renderer					m_renderer;
	lume::Neighborhoods			m_neighborhood;
	lume::SPMesh				m_mesh;
	std::vector <lume::SPMesh>	m_subsetMeshes;
	std::string					m_subsetAnnexName;
};

}//	end of namespace lumeview

#endif	//__H__lumeview_subset_visualization

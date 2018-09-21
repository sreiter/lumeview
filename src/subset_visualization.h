// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__lumeview_subset_visualization
#define __H__lumeview_subset_visualization

#include "lume/mesh.h"
#include "lume/neighborhoods.h"

#include "message_receiver.h"
#include "renderer.h"
#include "visualization.h"

namespace lume {
	class SubsetInfoAnnex;
}// end of namespace lume


namespace lumeview {

class SubsetVisualization : public Visualization, public MessageReceiver
{
public:
	SubsetVisualization ();
	void set_mesh (lume::SPMesh mesh);

	void refresh ();

	void render (const View& view) override;

	glm::vec2 estimate_z_clip_dists (const View& view) const override;

	
	void receive_message (const Message& msg) override;

private:
	void subset_meshes_from_grobs (const lume::GrobSet grobSet);
	void subset_meshes_from_cell_rim ();
	void refresh_subset_info_annex_name ();
	lume::Mesh& subset_mesh (const index_t si);
	
	glm::vec4 subset_color (const index_t si) const;
	bool subset_visible (const index_t si) const;

	Renderer					m_renderer;
	lume::Neighborhoods			m_neighborhood;
	lume::SPMesh				m_mesh;
	std::shared_ptr<lume::SubsetInfoAnnex>		m_subsetInfo;
	std::vector <lume::SPMesh>	m_subsetMeshes;
	std::string					m_subsetAnnexName;
	std::vector <int>			m_subsetIndexToStageIndex;

	const void*					m_subject;
	bool						m_refreshRequired;
};

}//	end of namespace lumeview

#endif	//__H__lumeview_subset_visualization

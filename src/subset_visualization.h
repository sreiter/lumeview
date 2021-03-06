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

#ifndef __H__lumeview_subset_visualization
#define __H__lumeview_subset_visualization

#include "lume/mesh.h"
#include "lume/neighborhoods.h"

#include "lumeview_error.h"
#include "message_receiver.h"
#include "renderer.h"
#include "visualization.h"

namespace lume {
	class SubsetInfoAnnex;
}// end of namespace lume


namespace lumeview {

DECLARE_CUSTOM_EXCEPTION (NoSubsetAnnexError, LumeviewError);

class SubsetVisualization : public Visualization, public MessageReceiver
{
public:
	SubsetVisualization ();
	SubsetVisualization (lume::SPMesh mesh);
	
	void set_mesh (lume::SPMesh mesh);

	void refresh ();

	void render (const View& view) override;

	glm::vec2 estimate_z_clip_dists (const View& view) const override;

	void receive_message (const Message& msg) override;

private:
	void create_subset_meshes (const lume::GrobSet grobSet);
	void prepare_renderer ();
	void subset_meshes_from_grobs (lume::SPMesh mesh,
	                               const lume::GrobSet grobSet,
	                               bool ignoreSubsetVisibilities);
	void subset_meshes_from_cell_rim ();
	void refresh_subset_info_annex_name ();
	lume::Mesh& subset_mesh (const index_t si);
	
	glm::vec4 subset_color (const index_t si) const;
	bool subset_visible (const index_t si) const;

	Renderer					m_renderer;
	lume::Neighborhoods			m_neighborhoods;
	lume::SPMesh				m_mesh;
	std::shared_ptr<lume::SubsetInfoAnnex>		m_subsetInfo;
	lume::SPMesh				m_rimMesh;
	std::vector <lume::SPMesh>	m_subsetMeshes;
	std::string					m_subsetAnnexName;
	std::vector <int>			m_subsetIndexToStageIndex;

	const void*					m_subject;
	bool						m_refreshRequired;
};

}//	end of namespace lumeview

#endif	//__H__lumeview_subset_visualization

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

#ifndef __H__lumeview_subset_info_annex_message
#define __H__lumeview_subset_info_annex_message

#include "message.h"

namespace lumeview {

///	Message about a change of properties of a SubsetInfoAnnex
class SubsetInfoAnnexMessage : public Message {
public:
	SubsetInfoAnnexMessage () : m_visibilityChanged (false), m_colorChanged (false), m_subsetIndex (0) {}

	SubsetInfoAnnexMessage (lume::SubsetInfoAnnex* subsetInfoAnnex,
							index_t subsetIndex,
	                        bool colorChanged,
							bool visibilityChanged) :
		m_subsetInfoAnnex (subsetInfoAnnex),
		m_subsetIndex (subsetIndex),
		m_colorChanged (colorChanged),
		m_visibilityChanged (visibilityChanged)
	{}

	/// returns a pointer to the corresponding SubsetInfoAnnex instance
	/** The returned pointer has the same value as `subset_info_annex()`*/
	const void* subject () const override				{return m_subsetInfoAnnex;}

	lume::SubsetInfoAnnex* subset_info_annex () const	{return m_subsetInfoAnnex;}
	index_t subset_index () const						{return m_subsetIndex;}
	bool visibility_changed () const					{return m_visibilityChanged;}
	bool color_changed () const							{return m_colorChanged;}

private:
	bool 					m_visibilityChanged;
	bool 					m_colorChanged;
	index_t					m_subsetIndex;
	lume::SubsetInfoAnnex*	m_subsetInfoAnnex;
};

}//	end of namespace lumeview

#endif	//__H__lumeview_subset_info_annex_message

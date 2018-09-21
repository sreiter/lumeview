// This file is part of lumeview, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

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

#ifndef __H__mesh
#define __H__mesh

#include <memory>
#include <vector>
#include <string>

#include "../types.h"
#include "grob.h"

namespace msh {


class VertexArray {
public:
	VertexArray ()	: m_tupleSize (1) {}
	VertexArray (const index_t tupleSize) : m_tupleSize (tupleSize) {}

	~VertexArray () {}

	/// total number of coordinates, counting individual components
	index_t num_coords () const			{return static_cast<index_t>(m_coords.size());}

	/// number of individual coordinate components making up a coordinate tuple
	index_t tuple_size () const				{return m_tupleSize;}
	void set_tuple_size (const index_t ts)	{m_tupleSize = ts;}

	real_t* raw_coords ()					{return &m_coords[0];}
	const real_t* raw_coords () const		{return &m_coords[0];}

	std::vector <real_t>& coords ()				{return m_coords;}
	const std::vector <real_t>& coords () const	{return m_coords;}

private:
	std::vector <real_t>	m_coords;
	index_t					m_tupleSize;
};


class Mesh {
public:
	Mesh (grob_t grobType) :
		m_grobType (grobType),
		m_vrts (std::make_shared <VertexArray> ()) {}
	Mesh (grob_t grobType, std::shared_ptr <const VertexArray> coords) :
		m_grobType (grobType),
		m_vrts (coords) {}
	~Mesh () {}
	
	std::shared_ptr <const VertexArray>	vrts () const	{return m_vrts;}
	
	std::vector <index_t>& inds ()						{return m_inds;}
	const std::vector <index_t>& inds () const			{return m_inds;}

	index_t* raw_inds ()								{return &m_inds[0];}
	const index_t* raw_inds () const					{return &m_inds[0];}

	index_t num_inds () const							{return static_cast<index_t>(m_inds.size());}

	grob_t grob_type () const							{return m_grobType;}
	void set_grob_type (const grob_t gt)				{m_grobType = gt;}

private:
	std::shared_ptr <const VertexArray>	m_vrts;
	std::vector <index_t>				m_inds;
	grob_t								m_grobType;
};


std::shared_ptr <Mesh> CreateMeshFromFile (std::string filename);

}// end of namespace msh

#endif	//__H__mesh

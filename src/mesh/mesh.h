#ifndef __H__msh__mesh
#define __H__msh__mesh

#include <map>
#include <memory>
#include <vector>
#include <string>

#include "../types.h"
#include "grob.h"

namespace msh {

template <class T>
class DataArray {
public:
	using value_type = T;
	using value_t = T;

	DataArray ()	: m_tupleSize (1) {}
	DataArray (const index_t tupleSize) : m_tupleSize (tupleSize) {}

	/// total number of entries, counting individual components
	index_t size () const			{return static_cast<index_t>(m_data.size());}

	index_t num_tuples () const		{return size() / tuple_size();}

	/// number of individual components making up a tuple
	index_t tuple_size () const				{return m_tupleSize;}
	void set_tuple_size (const index_t ts)	{m_tupleSize = ts;}

	T* raw_data ()					{return &m_data[0];}
	const T* raw_data () const		{return &m_data[0];}

	std::vector <T>& data ()				{return m_data;}
	const std::vector <T>& data () const	{return m_data;}

private:
	std::vector <T>	m_data;
	index_t			m_tupleSize;
};

using CoordArray	= DataArray <real_t>;
using IndexArray	= DataArray <index_t>;


class Mesh {
public:
	Mesh (grob_t grobType) :
		m_grobType (grobType),
		m_coords (std::make_shared <CoordArray> ()),
		m_inds (std::make_shared <IndexArray> ())
	{}
	
	Mesh (grob_t grobType, std::shared_ptr <const CoordArray> coords) :
		m_grobType (grobType),
		m_coords (coords),
		m_inds (std::make_shared <IndexArray> ())
	{}

	~Mesh () {}
	
	std::shared_ptr <const CoordArray>	coords () const	{return m_coords;}
	index_t num_coords () const							{return m_coords->size();}

	std::shared_ptr <IndexArray> inds ()				{return m_inds;}
	std::shared_ptr <const IndexArray> inds () const	{return m_inds;}
	index_t num_inds () const							{return m_inds->size();}

	grob_t grob_type () const							{return m_grobType;}
	void set_grob_type (const grob_t gt)				{m_grobType = gt;}

	bool has_data (const std::string& id)				{return m_dataMap[id].get() != nullptr;}

	///	returns the data array for the given id. If none was present, a new one will be created.
	std::shared_ptr <DataArray <real_t>> data (const std::string& id)
	{
		auto d = m_dataMap[id];
		if(!d)
			m_dataMap[id] = d = std::make_shared <DataArray <real_t>> ();
		return d;
	}

	///	explicitly set a data array of a mesh
	void set_data (const std::string& id, std::shared_ptr <DataArray <real_t>> data)
	{
		m_dataMap[id] = data;
	}

	// std::shared_ptr <const DataArray <real_t>> data (const std::string& id) const
	// {
	// 	 std::shared_ptr <const DataArray <real_t>> d = m_dataMap[id];
	// 	COND_THROW(!d, "Queried data '" << id << "'not available in the mesh.");
	// 	return d;
	// }

	///	removes a data array from a mesh.
	/** This will decrement the shared_ptr but not necessarily delete the array.*/
	void remove_data (const std::string& id)
	{
		m_dataMap.erase (id);
	}

private:
	std::shared_ptr <const CoordArray>								m_coords;
	std::shared_ptr <IndexArray>									m_inds;
	std::map <std::string, std::shared_ptr <DataArray <real_t>>>	m_dataMap;
	grob_t															m_grobType;
};


std::shared_ptr <Mesh> CreateMeshFromFile (std::string filename);

}// end of namespace msh

#endif	//__H__msh__mesh

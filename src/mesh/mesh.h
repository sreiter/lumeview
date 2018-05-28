#ifndef __H__msh__mesh
#define __H__msh__mesh

#include <map>
#include <memory>
#include <vector>
#include <string>

#include "../cond_throw.h"
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


using RealDataArray		= DataArray <real_t>;
using IndexDataArray	= DataArray <index_t>;

using SPRealDataArray	= std::shared_ptr <RealDataArray>;
using SPIndexDataArray	= std::shared_ptr <IndexDataArray>;
using SPCRealDataArray	= std::shared_ptr <const RealDataArray>;
using SPCIndexDataArray	= std::shared_ptr <const IndexDataArray>;



class Mesh {
public:
	Mesh () :
		m_coords (std::make_shared <RealDataArray> ())
	{
		set_data <real_t> ("coords", m_coords);
	}
	
	~Mesh () {}
	
	SPRealDataArray coords ()						{return m_coords;}
	SPCRealDataArray coords () const				{return m_coords;}
	index_t num_coords () const						{return m_coords->size();}

	SPIndexDataArray inds (const grob_t gt)
	{
		auto t = grob_storage().data(gt);
		t->set_tuple_size (GrobDesc(gt).num_corners());
		return t;
	}

	SPCIndexDataArray inds (const grob_t gt) const
	{
		return grob_storage().data(gt);
	}

	bool has_inds (const grob_t gt) const
	{
		return grob_storage().has_data(gt);
	}

	void remove_inds (const grob_t gt)
	{
		grob_storage().remove_data (gt);
	}

	std::vector <grob_t> grob_types() const
	{
		return grob_storage().collect_keys();
	}



	template <class T>
	bool has_data (const std::string& id) const		{return storage(T{}).has_data (id);}

	///	returns the data array for the given id. If none was present, a new one will be created.
	template <class T>
	std::shared_ptr <DataArray <T>>
	data (const std::string& id)					{return storage(T{}).data (id);}

	///	explicitly set a data array of a mesh
	template <class T>
	void set_data (const std::string& id,
	               const std::shared_ptr <DataArray <T>>& data)	{return storage(T{}).set_data (id, data);}

	// template <class T>
	// SPCRealDataArray data (const std::string& id) const

	///	removes a data array from a mesh.
	/** This will decrement the shared_ptr but not necessarily delete the array.*/
	template <class T>
	void remove_data (const std::string& id)		{return storage(T{}).remove_data (id);}

private:
	template <class TKey, class T> class DataStorage
	{
		using sp_data_array_t	= std::shared_ptr <DataArray<T>>;
		using spc_data_array_t	= std::shared_ptr <const DataArray<T>>;
		using data_map_t		= std::map <TKey, sp_data_array_t>;

		public:
		bool has_data (const TKey& id) const
		{
			auto i = m_dataMap.find (id);
			return i != m_dataMap.end ();
		}

		///	returns the data array for the given id. If none was present, a new one will be created.
		sp_data_array_t data (const TKey& id)
		{
			auto d = m_dataMap[id];
			if(!d)
				m_dataMap[id] = d = std::make_shared <DataArray <T>> ();
			return d;
		}

		///	explicitly set a data array of a mesh
		void set_data (const TKey& id, sp_data_array_t data)
		{
			m_dataMap[id] = data;
		}

		spc_data_array_t data (const TKey& id) const
		{
			auto i = m_dataMap.find (id);
			COND_THROW (i == m_dataMap.end(),
			            "Queried data '" << id << "'not available in the mesh.");
			return i->second;
		}

		///	removes a data array from a mesh.
		/** This will decrement the shared_ptr but not necessarily delete the array.*/
		void remove_data (const TKey& id)
		{
			m_dataMap.erase (id);
		}

		std::vector <TKey> collect_keys () const
		{
			std::vector <TKey> keys;
			for (auto& e : m_dataMap)
				keys.push_back (e.first);
			return keys;
		}
		
		private:
		data_map_t	m_dataMap;
	};

	template <class T>
	DataStorage<std::string, T>& storage (const T&) {// dummy parameter to allow for overloads
		THROW("Unsupported data type: " << typeid(T).name);
	}

	template <class T>
	const DataStorage<std::string, T>& storage (const T&) const {// dummy parameter to allow for overloads
		THROW("Unsupported data type: " << typeid(T).name);
	}

	DataStorage<std::string, real_t>& storage (const real_t&) {
		return m_realDataStorage;
	}

	const DataStorage<std::string, real_t>& storage (const real_t&) const {
		return m_realDataStorage;
	}

	DataStorage<std::string, index_t>& storage (const index_t&) {
		return m_indexDataStorage;
	}

	const DataStorage<std::string, index_t>& storage (const index_t&) const {
		return m_indexDataStorage;
	}

	DataStorage<grob_t, index_t>& grob_storage () {
		return m_grobDataStorage;
	}

	const DataStorage<grob_t, index_t>& grob_storage () const {
		return m_grobDataStorage;
	}


	//	MEMBER VARIABLES
	SPRealDataArray		m_coords;
	DataStorage <std::string, real_t>	m_realDataStorage;
	DataStorage <std::string, index_t>	m_indexDataStorage;
	
	DataStorage <grob_t, index_t>		m_grobDataStorage;
};

using SPMesh = std::shared_ptr <Mesh>;

SPMesh CreateMeshFromFile (std::string filename);

}// end of namespace msh

#endif	//__H__msh__mesh

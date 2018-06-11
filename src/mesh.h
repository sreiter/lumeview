#ifndef __H__slimesh__mesh
#define __H__slimesh__mesh

#include <map>
#include <memory>
#include <vector>
#include <string>

#include "cond_throw.h"
#include "types.h"
#include "grob.h"
#include "grob_hash.h"
#include "data_buffer.h"

namespace slimesh {


class Mesh {
public:
	struct DataKey {
		DataKey ()									: name (""), grobId (INVALID_GROB) {}
		DataKey (const char* _name)					: name (_name), grobId (INVALID_GROB) {}
		DataKey (std::string _name)					: name (std::move(_name)), grobId (INVALID_GROB) {}
		DataKey (std::string _name, grob_t _grobId)	: name (std::move(_name)), grobId (_grobId) {}
		bool operator < (const DataKey& key) const	{return grobId < key.grobId || (grobId == key.grobId && name < key.name);}

		std::string name;
		grob_t		grobId;
	};


	Mesh () :
		m_coords (std::make_shared <RealDataBuffer> ())
	{
		set_data <real_t> (DataKey ("coords", VERTEX), m_coords);
	}
	
	~Mesh () {}
	
	SPRealDataBuffer coords ()						{return m_coords;}
	SPCRealDataBuffer coords () const				{return m_coords;}
	index_t num_coords () const						{return m_coords->size();}

	void set_coords (const SPRealDataBuffer& coords)		{m_coords = coords;}

	SPIndexDataBuffer inds (const grob_t gt)
	{
		auto t = grob_storage().data(gt);
		t->set_tuple_size (GrobDesc(gt).num_corners());
		return t;
	}

	SPCIndexDataBuffer inds (const grob_t gt) const
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
	bool has_data (const DataKey& key) const		{return storage(T{}).has_data (key);}

	///	returns the data array for the given id. If none was present, a new one will be created.
	template <class T>
	std::shared_ptr <DataBuffer <T>>
	data (const DataKey& key)					{return storage(T{}).data (key);}

	///	explicitly set a data array of a mesh
	template <class T>
	void set_data (const DataKey& key,
	               const std::shared_ptr <DataBuffer <T>>& data)
	{
		storage(T{}).set_data (key, data);
		if (key.name == "coords")
			set_coords (data);
	}

	// template <class T>
	// SPCRealDataBuffer data (const std::string& id) const

	///	removes a data array from a mesh.
	/** This will decrement the shared_ptr but not necessarily delete the array.*/
	template <class T>
	void remove_data (const DataKey& key)		{storage(T{}).remove_data (key);}

private:
	template <class T>
	void set_coords (const std::shared_ptr<T>& coords) {THROW("Mesh::set_coords only supported for type real_t");}

	template <class TKey, class T> class DataStorage
	{
		using sp_data_array_t	= std::shared_ptr <DataBuffer<T>>;
		using spc_data_array_t	= std::shared_ptr <const DataBuffer<T>>;
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
				m_dataMap[id] = d = std::make_shared <DataBuffer <T>> ();
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
	DataStorage <DataKey, T>& storage (const T&) {// dummy parameter to allow for overloads
		THROW("Unsupported data type: " << typeid(T).name());
	}

	template <class T>
	const DataStorage <DataKey, T>& storage (const T&) const {// dummy parameter to allow for overloads
		THROW("Unsupported data type: " << typeid(T).name());
	}

	DataStorage <DataKey, real_t>& storage (const real_t&) {
		return m_realDataStorage;
	}

	const DataStorage <DataKey, real_t>& storage (const real_t&) const {
		return m_realDataStorage;
	}

	DataStorage <DataKey, index_t>& storage (const index_t&) {
		return m_indexDataStorage;
	}

	const DataStorage <DataKey, index_t>& storage (const index_t&) const {
		return m_indexDataStorage;
	}

	DataStorage<grob_t, index_t>& grob_storage () {
		return m_grobDataStorage;
	}

	const DataStorage<grob_t, index_t>& grob_storage () const {
		return m_grobDataStorage;
	}


	//	MEMBER VARIABLES
	SPRealDataBuffer				m_coords;
	DataStorage <DataKey, real_t>	m_realDataStorage;
	DataStorage <DataKey, index_t>	m_indexDataStorage;
	
	DataStorage <grob_t, index_t>	m_grobDataStorage;
};

using SPMesh = std::shared_ptr <Mesh>;

SPMesh CreateMeshFromFile (std::string filename);

}// end of namespace slimesh

#endif	//__H__slimesh__mesh

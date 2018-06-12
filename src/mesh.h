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
		// DataKey (const char* _name)					: name (_name), grobId (INVALID_GROB) {}
		// DataKey (std::string _name)					: name (std::move(_name)), grobId (INVALID_GROB) {}
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
	
	template <class T>
	void share_data_with (Mesh& target, grob_t gt) const
	{
		auto& dataMap = storage(T{}).data_map();

		for (auto& entry : dataMap) {
			if (entry.first.grobId == gt) {
				target.set_data (entry.first, entry.second);
			}
		}
	}

	void share_data_with (Mesh& target, grob_t gt) const
	{
		share_data_with <real_t> (target, gt);
		share_data_with <index_t> (target, gt);
	}

	template <class T>
	void share_data_with (Mesh& target) const
	{
		auto& dataMap = storage(T{}).data_map();

		for (auto& entry : dataMap)
			target.set_data (entry.first, entry.second);
	}

	void share_data_with (Mesh& target) const
	{
		share_data_with <real_t> (target);
		share_data_with <index_t> (target);
	}

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

	index_t num_inds (grob_t grob)
	{
		if (has_inds (grob))
			return inds (grob)->size();
		return 0;
	}

	index_t num_inds (const GrobSet& gs)
	{
		index_t num = 0;
		for(index_t i = 0; i < gs.size(); ++i)
			num += num_inds (gs.grob_type (i));
		return num;
	}

	index_t num_tuples (grob_t grob)
	{
		if (has_inds (grob))
			return inds (grob)->num_tuples();
		return 0;
	}

	index_t num_tuples (const GrobSet& gs)
	{
		index_t num = 0;
		for(index_t i = 0; i < gs.size(); ++i)
			num += num_tuples (gs.grob_type (i));
		return num;
	}



	template <class T>
	bool has_data (const DataKey& key) const		{return storage(T{}).has_data (key);}

	template <class T>
	bool has_data (const std::string& name, grob_t gt) const		{return has_data <T> (DataKey (name, gt));}


	///	returns the data array for the given id. If none was present, a new one will be created.
	template <class T>
	std::shared_ptr <DataBuffer <T>>
	data (const DataKey& key)							{return storage(T{}).data (key);}

	template <class T>
	std::shared_ptr <DataBuffer <T>>
	data (const std::string& name, grob_t gt)			{return data <T> (DataKey (name, gt));}
	template <class T>
	std::shared_ptr <const DataBuffer <T>>
	data (const DataKey& key) const						{return storage(T{}).data (key);}

	template <class T>
	std::shared_ptr <const DataBuffer <T>>
	data (const std::string& name, grob_t gt) const		{return data <T> (DataKey (name, gt));}


	///	explicitly set a data array of a mesh
	template <class T>
	void set_data (const DataKey& key,
	               const std::shared_ptr <DataBuffer <T>>& data)
	{
		storage(T{}).set_data (key, data);
		if (key.name == "coords")
			set_coords (data);
	}

	template <class T>
	void set_data (const std::string& name, grob_t gt,
	               const std::shared_ptr <DataBuffer <T>>& data)
	{
		set_data <T> (DataKey (name, gt), data);
	}

	// template <class T>
	// SPCRealDataBuffer data (const std::string& id) const

	///	removes a data array from a mesh.
	/** This will decrement the shared_ptr but not necessarily delete the array.*/
	template <class T>
	void remove_data (const DataKey& key)		{storage(T{}).remove_data (key);}

	template <class T>
	void remove_data (const std::string& name, grob_t gt) const		{remove_data <T> (DataKey (name, gt));}


private:
	template <class T>
	void set_coords (const std::shared_ptr<T>& coords) {THROW("Mesh::set_coords only supported for type real_t");}

	template <class TKey, class T> class DataStorage
	{
		using sp_data_array_t	= std::shared_ptr <DataBuffer<T>>;
		using spc_data_array_t	= std::shared_ptr <const DataBuffer<T>>;
		using data_map_t		= std::map <TKey, sp_data_array_t>;

		public:
		const data_map_t& data_map () const
		{
			return m_dataMap;
		}

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

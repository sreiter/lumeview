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
#include "mesh_data.h"

namespace slimesh {

class Mesh {
public:

	struct DataKey {
		DataKey ()									: name (""), grobId (NO_GROB) {}
		// DataKey (const char* _name)					: name (_name), grobId (NO_GROB) {}
		// DataKey (std::string _name)					: name (std::move(_name)), grobId (NO_GROB) {}
		DataKey (std::string _name, grob_t _grobId)	: name (std::move(_name)), grobId (_grobId) {}
		bool operator < (const DataKey& key) const			{return grobId < key.grobId || (grobId == key.grobId && name < key.name);}
		// std::ostream& operator << (std::ostream& out) const	{out << name; return out;}
		
		std::string name;
		grob_t		grobId;
	};

	using data_iterator_t = std::map <DataKey, SPMeshData>::iterator;
	using const_data_iterator_t = std::map <DataKey, SPMeshData>::const_iterator;



	Mesh () :
		m_coords (std::make_shared <RealBuffer> ())
	{
		set_data (DataKey ("coords", VERTEX), m_coords);
	}
	
	~Mesh () {}
	
	// COORDINATES
	SPRealBuffer coords ()						{return m_coords;}
	CSPRealBuffer coords () const				{return m_coords;}
	index_t num_coords () const						{return m_coords->size();}

	void set_coords (const SPRealBuffer& coords)		{m_coords = coords;}


	// INDICES
	SPIndexBuffer inds (const grob_t gt)
	{
		auto t = m_grobStorage.data(gt);
		t->set_tuple_size (GrobDesc(gt).num_corners());
		return t;
	}

	CSPIndexBuffer inds (const grob_t gt) const
	{
		return m_grobStorage.data(gt);
	}

	bool inds_allocated (const grob_t gt) const
	{
		return m_grobStorage.has_data(gt);
	}

	bool has (const grob_t gt) const
	{
		return inds_allocated (gt) && inds (gt)->size();
	}

	bool has (const GrobSet gs) const
	{
		for (auto gt : gs) {
			if (has (gt))
				return true;
		}
		return false;
	}

	void remove_inds (const grob_t gt)
	{
		m_grobStorage.remove_data (gt);
	}

	std::vector <grob_t> grob_types() const
	{
		return m_grobStorage.collect_keys();
	}

	index_t num_inds (grob_t grob)
	{
		if (inds_allocated (grob))
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
		if (inds_allocated (grob))
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


	// DATA
	bool has_data (const DataKey& key) const						{return m_dataStorage.has_data (key);}

	bool has_data (const std::string& name, grob_t gt) const		{return has_data (DataKey (name, gt));}

	template <class T>
	bool has_data (const DataKey& key) const						{return m_dataStorage.has_data (key) && data <T> (key);}

	template <class T>
	bool has_data (const std::string& name, grob_t gt) const		{return has_data <T> (DataKey (name, gt));}


	///	returns the data array for the given id. If none was present, a new one will be created.
	template <class T>
	std::shared_ptr <T>
	data (const DataKey& key)							{return std::dynamic_pointer_cast<T> (m_dataStorage.data <T> (key));}

	template <class T>
	std::shared_ptr <T>
	data (const std::string& name, grob_t gt)			{return data <T> (DataKey (name, gt));}

	template <class T>
	std::shared_ptr <const T>
	data (const DataKey& key) const						{return std::dynamic_pointer_cast<const T> (m_dataStorage.data (key));}

	template <class T>
	std::shared_ptr <const T>
	data (const std::string& name, grob_t gt) const		{return data <T> (DataKey (name, gt));}


	///	explicitly set data for a mesh
	void set_data (const DataKey& key,
	               const SPMeshData& data)
	{
		m_dataStorage.set_data (key, data);
		if (key.name == "coords")
			set_coords (data);
	}

	void set_data (const std::string& name, grob_t gt,
	               const SPMeshData& data)
	{
		set_data (DataKey (name, gt), data);
	}

	///	removes data from a mesh.
	/** This will decrement the shared_ptr but not necessarily delete the data.*/
	void remove_data (const DataKey& key)					{m_dataStorage.remove_data (key);}

	void remove_data (const std::string& name, grob_t gt)	{remove_data (DataKey (name, gt));}


	void share_data_with (Mesh& target) const
	{
		auto& dataMap = m_dataStorage.data_map();
		for (auto& entry : dataMap)
			target.set_data (entry.first, entry.second);
	}

	void share_data_with (Mesh& target, grob_t gt) const
	{
		auto& dataMap = m_dataStorage.data_map();
		for (auto& entry : dataMap) {
			if (entry.first.grobId == gt) {
				target.set_data (entry.first, entry.second);
			}
		}
	}

	data_iterator_t data_begin ()	{return m_dataStorage.data_map().begin();}
	data_iterator_t data_end ()		{return m_dataStorage.data_map().end();}

	const_data_iterator_t data_begin () const	{return m_dataStorage.data_map().begin();}
	const_data_iterator_t data_end () const		{return m_dataStorage.data_map().end();}

private:
	template <class T>
	void set_coords (const std::shared_ptr<T>& coords) {
		if (auto t = std::dynamic_pointer_cast <RealBuffer> (coords))
			set_coords (t);
		else
			THROW("Mesh::set_coords only supported for type real_t");
	}

	template <class TKey, class T> class DataStorage
	{
		using value_t		= std::shared_ptr <T>;
		using const_value_t	= std::shared_ptr <const T>;
		using data_map_t	= std::map <TKey, value_t>;

		public:
		data_map_t& data_map ()						{return m_dataMap;}

		const data_map_t& data_map () const			{return m_dataMap;}

		bool has_data (const TKey& id) const
		{
			auto i = m_dataMap.find (id);
			return i != m_dataMap.end ();
		}

		///	returns the data array for the given id. If none was present, a new one will be created.
		template <class TConstruct = T>
		value_t data (const TKey& id)
		{
			auto d = m_dataMap[id];
			if(!d)
				m_dataMap[id] = d = std::make_shared <TConstruct> ();
			return d;
		}

		///	explicitly set a data array of a mesh
		void set_data (const TKey& id, const value_t& data)
		{
			m_dataMap[id] = data;
		}

		const_value_t data (const TKey& id) const
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

	using index_data_storage_t	= DataStorage <grob_t, IndexBuffer>;
	using mesh_data_storage_t	= DataStorage <DataKey, MeshData>;

	//	MEMBER VARIABLES
	SPRealBuffer			m_coords;
	index_data_storage_t	m_grobStorage;
	mesh_data_storage_t		m_dataStorage;
};

inline std::ostream& operator<< (std::ostream& out, const Mesh::DataKey& v) {
    out << v.name;
    return out;
}

using SPMesh = std::shared_ptr <Mesh>;
using CSPMesh = std::shared_ptr <const Mesh>;

SPMesh CreateMeshFromFile (std::string filename);

}// end of namespace slimesh

#endif	//__H__slimesh__mesh

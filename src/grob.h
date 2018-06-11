#ifndef __H__slimesh__grob
#define __H__slimesh__grob

#include <cstdint>
#include "../types.h"
#include "../log.h"

namespace slimesh {

enum grob_t {
	VERTEX,
	EDGE,
	TRI,
	QUAD,
	TET,
	// PYRA,
	// PRISM,
	// HEX
	INVALID_GROB
};

enum grob_list_t {
	VERTICES	= VERTEX,
	EDGES		= EDGE,
	TRIS		= TRI,
	QUADS		= QUAD,
	TETS		= TET,

	INVALID_GROB_LIST	= INVALID_GROB,

	FACES,
	CELLS
};

static const index_t MAX_GROB_DIM = 2;
static const index_t NUM_GROB_TYPES = INVALID_GROB;

static const index_t MAX_CORNERS [] = {
	0,	// Max corners of elements with dim 0
	2,	// Max corners of elements with dim 1
	4	// Max corners of elements with dim 2
};

static const index_t TOTAL_MAX_CORNERS = 16; // Increasing this value has side effects!
											// See, e.g., cornerOffsets in Grob type.

namespace impl {
/// Plain integer array describing the properties of all *grid objects*.
/** Please do not work with this array directly, but use the `GrobDesc` class
 * instead.
 * 
 * The structure of each element entry is as follows:
 * \code
 * TYPE,
 * DIM,
 *
 * NUM 0D SIDES, // (if DIM > 0)
 * OFFSET TO 1D SIDES counting from the this entry, // (if DIM > 0)
 *
 * NUM 1D SIDES, // (if DIM > 1)
 * OFFSET TO 1D SIDES counting from the this entry, // (if DIM > 1)
 *
 * NUM 2D SIDES, //	(if DIM > 2)
 * OFFSET TO 2D SIDES counting from the this entry, // (if DIM > 2)
 * // ... if DIM > 3
 *
 * // (if DIM > 1)
 * ELEMENT_TYPE, corner1, ..., cornerN,	// (1D SIDE 1)
 * ...
 * ELEMENT_TYPE, corner1, ..., cornerN,	// (1D SIDE M)
 *
 * // (if DIM > 2)
 * ELEMENT_TYPE, corner1, ..., cornerN,	// (2D SIDE 1)
 * ...
 * ELEMENT_TYPE, corner1, ..., cornerN,	// (2D SIDE M)
 * \endcode
 *
 * Offsets to individual elements in this list are stored in GROB_DESC_OFFSETS.
 */

static const index_t GROB_DESCS[] = {
//	VERTEX
	VERTEX,	// TYPE
	0, 		// DIM

//	EDGE
	EDGE,	// TYPE
	1,		// DIM
	1,		// Offset to num 0D sides entry counting from this entry
	2,		// num 0D sides (CORNERS)
	2,		// Offset to first 0D side counting from the this entry
	3,		// Offset to second 0D side counting from the this entry
	VERTEX, 0,
	VERTEX, 1,

//	TRI
	TRI,	// TYPE
	2,		// DIM
	2,		// Offset to num 0D sides entry counting from this entry
	11,		// Offset to num 1D sides entry counting from this entry
	3,		// num 0D sides (CORNERS)
	3,		// offset to first 0D side counting from this entry
	4,		// offset to second 0D side counting from this entry
	5,		// offset to third 0D side counting from this entry
	VERTEX, 0,	// 0D side 1 (type, corners)
	VERTEX, 1,	// 0D side 2 (type, corners)
	VERTEX, 2,	// 0D side 3 (type, corners)
	3,		// num 1D sides
	3,		// offset to first 1D side counting from this entry
	5,		// offset to second 1D side counting from this entry
	7,		// offset to third 1D side counting from this entry
	EDGE, 0, 1,	// 1D side 1 (type, corners)
	EDGE, 1, 2,	// 1D side 2 (type, corners)
	EDGE, 2, 0,	// 1D side 3 (type, corners)

//	QUAD
	QUAD,	// TYPE
	2,		// DIM
	2,		// Offset to num 0D sides entry counting from this entry
	14,		// Offset to num 1D sides entry counting from this entry
	4,		// num 0D sides (CORNERS)
	4,		// offset to first 0D side counting from this entry
	5,		// offset to second 0D side counting from this entry
	6,		// offset to third 0D side counting from this entry
	7,		// offset to fourth 0D side counting from this entry
	VERTEX, 0,	// 0D side 1 (type, corners)
	VERTEX, 1,	// 0D side 2 (type, corners)
	VERTEX, 2,	// 0D side 3 (type, corners)
	VERTEX, 3,	// 0D side 4 (type, corners)
	4,		// num 1D sides
	4,		// offset to first 1D side counting from this entry
	6,		// offset to second 1D side counting from this entry
	8,		// offset to third 1D side counting from this entry
	10,		// offset to fourth 1D side counting from this entry
	EDGE, 0, 1,	// 1D side 1 (type, corners)
	EDGE, 1, 2,	// 1D side 2 (type, corners)
	EDGE, 2, 3,	// 1D side 3 (type, corners)
	EDGE, 3, 0,	// 1D side 4 (type, corners)

//	TET
	TET,	// TYPE
	3,		// DIM
	3,		// Offset to num 0D sides entry counting from this entry
	15,		// Offset to num 1D sides entry counting from this entry
	39,		// Offset to num 2D sides entry counting from this entry
	4,		// num 0D sides (CORNERS)
	4,		// offset to first 0D side counting from this entry
	5,		// offset to second 0D side counting from this entry
	6,		// offset to third 0D side counting from this entry
	7,		// offset to fourth 0D side counting from this entry
	VERTEX, 0,	// 0D side 1 (type, corners)
	VERTEX, 1,	// 0D side 2 (type, corners)
	VERTEX, 2,	// 0D side 3 (type, corners)
	VERTEX, 3,	// 0D side 4 (type, corners)
	6,		// num 1D sides
	6,		// offset to first 1D side counting from this entry
	8,		// offset to second 1D side counting from this entry
	10,		// offset to third 1D side counting from this entry
	12,		// offset to fourth 1D side counting from this entry
	14,		// offset to fifth 1D side counting from this entry
	16,		// offset to sixth 1D side counting from this entry
	EDGE, 0, 1,	// 1D side 1 (type, corners)
	EDGE, 1, 2,	// 1D side 2 (type, corners)
	EDGE, 2, 0,	// 1D side 3 (type, corners)
	EDGE, 0, 3,	// 1D side 4 (type, corners)
	EDGE, 1, 3,	// 1D side 5 (type, corners)
	EDGE, 2, 3,	// 1D side 6 (type, corners)
	4,		// num 2d sides
	4,		// offset to first 2D side counting from this entry
	7,		// offset to second 2D side counting from this entry
	10,		// offset to third 2D side counting from this entry
	13,		// offset to fourth 2D side counting from this entry
	TRI, 0, 2, 1,	// 2D side 1 (type, corners)
	TRI, 0, 1, 3,	// 2D side 2 (type, corners)
	TRI, 1, 2, 3,	// 2D side 3 (type, corners)
	TRI, 2, 0, 3	// 2D side 4 (type, corners)
};

///	Holds offsets to each *grid object type* in the `GROB_DESCS` array.
/** Please do not work with this array directly, but use the `GrobDesc` class
 * instead.
 */
static const index_t GROB_DESC_OFFSETS[] = {
	0,	//	VERTEX
	2,	//	EDGE
	12,	//	TRI
	39,	//	QUAD
	73	//	TET
};


static const index_t GROB_LIST_DESCS[] = {
	VERTICES,	// TYPE
	0,			// DIM
	1,			// SIZE
	VERTEX,		// ENTRY 1

	EDGES,		// TYPE
	1,			// DIM
	1,			// SIZE
	EDGE,		// ENTRY 1
	VERTICES,	// 0D side list

	TRIS,		// TYPE
	2,			// DIM
	1,			// SIZE
	TRI,		// ENTRY 1
	VERTICES,	// 0D side list
	EDGES,		// 1D side list

	QUADS,		// TYPE
	2,			// DIM
	1,			// SIZE
	QUAD,		// ENTRY 1
	VERTICES,	// 0D side list
	EDGES,		// 1D side list	

	TETS,		// TYPE
	3,			// DIM
	1,			// SIZE
	TET,		// ENTRY 1
	VERTICES,	// 0D side list
	EDGES,		// 1D side list
	TRIS,		// 2D side list

	INVALID_GROB_LIST,	// TYPE
	0,					// DIM
	0,					// SIZE

	FACES,		// TYPE
	2,			// DIM
	2,			// SIZE
	TRI,		// ENTRY 1
	QUAD,		// ENTRY 1
	VERTICES,	// 0D side list
	EDGES,		// 1D side list

	CELLS,		// TYPE
	3,			// DIM
	1,			// SIZE
	TET,		// ENTRY 1
	VERTICES,	// 0D side list
	EDGES,		// 1D side list
	FACES		// 2D side list
};

static const index_t GROB_LIST_DESC_OFFESTS[] = {
	0,	// VERTICES
	4,	// EDGES
	9,	// TRIS
	15,	// QUADS
	21,	// TETS
	28,	// INVALID_GROB_LIST

	31,	// FACES
	38,	// CELLS
};


/// Logs all grob-descs in a human readable way.
void PrintGrobDescs ();

}//	end of namespace impl

///	returns the name of a grob
const std::string& GrobName (grob_t grob);

///	Describes a class of **grid objects** in terms of local corner indices and sides.
/** A `GrobDesc` is a descriptor object for a given type of *grid object*.
 * An instance can be queried for different properties common to a type of
 * **grid object**.
 *
 * \note	An instance of `GrobDesc` corresponds to the **class** of *grid objects*
 *			of a certain type, not to an individual *grid object*.
 *
 * \note	*Local corner indices* always start from 0 and go up to `num_corners()`.
 */
class GrobDesc {
public:
	GrobDesc (grob_t grobType) :
		m_offset (impl::GROB_DESC_OFFSETS [grobType])
	{}

	inline grob_t type () const						{return static_cast<grob_t>(impl::GROB_DESCS [m_offset]);}
	const std::string& name () const				{return GrobName (type());}
	inline index_t dim () const						{return impl::GROB_DESCS [m_offset + 1];}
	inline index_t num_corners () const				{return impl::GROB_DESCS [side_offset (0)];}

	inline index_t local_corner (const index_t cornerIndex) const
	{
		return *local_side_corners (0, cornerIndex);
	}

	inline index_t num_sides (const index_t sideDim) const
	{
		if (sideDim >= dim ())
			return 0;
		return impl::GROB_DESCS [side_offset (sideDim)];
	}

	inline grob_t side_type (const index_t sideDim, const index_t sideIndex) const
	{
		return static_cast<grob_t> (impl::GROB_DESCS [side_desc_offset (sideDim, sideIndex)]);
	}
	
	inline GrobDesc side_desc (const index_t sideDim, const index_t sideIndex) const
	{
		return GrobDesc (side_type (sideDim, sideIndex));
	}

	inline index_t num_side_corners (const index_t sideDim, const index_t sideIndex) const
	{
		return side_desc (sideDim, sideIndex).num_corners();
	}

	/// returns the local indices of corners of the specified side
	inline const index_t* local_side_corners (const index_t sideDim, const index_t sideIndex) const
	{
		return impl::GROB_DESCS + side_desc_offset (sideDim, sideIndex) + 1;
	}

private:
	index_t side_offset (const index_t sideDim) const
	{
		return m_offset + 2 + sideDim + impl::GROB_DESCS[m_offset + 2 + sideDim];
	}

	index_t side_desc_offset (const index_t sideDim, const index_t sideIndex) const
	{
		const index_t t = side_offset (sideDim) + 1 + sideIndex;
		return t + impl::GROB_DESCS [t];
	}

	const index_t			m_offset;
};


namespace impl {
///	An array of 16 4-bit integers
class Array_16_4 {
public:
	Array_16_4 () : m_data (0) {}
	Array_16_4 (std::uint64_t v) : m_data (v) {}
	Array_16_4 (const Array_16_4& ar) : m_data (ar.m_data) {}

	static Array_16_4 ascending_order () {
		static const Array_16_4 ao (build_ascending_order()); 
		return Array_16_4(ao);
	}

	bool operator == (const Array_16_4& a) const	{return m_data == a.m_data;}
	
	void clear () {
		m_data = 0;
	}

	index_t get (const index_t i) const {
		return (m_data >> i*4) & 0xF;
	}
	void set (const index_t i, const index_t v) {
		m_data &= ~(0xF << i*4);		// set i-th entry to 0 (important!)
		m_data |= (v & 0xF) << i*4;	// set i-th entry to v
	}

private:
	static std::uint64_t build_ascending_order () {
		using u64 = std::uint64_t;
		u64 ao = 0;
		for(u64 i = 0; i < 16; ++i) {
			ao |= i << 4*i;
		}
		return ao;
	}
	std::uint64_t	m_data;
};

}//	end of namespace impl


/// A Grob represents a **grid object**, specified by its corner indices.
/** To create a `Grob`, pass the `grobType` and an array of corner-indices
 * (`corners`) to the constructor. `corners`has to have at least length
 * `GridDesc(grobType).num_corners()`.
 *
 * \warning	Since a `Grob` instance stores a pointer to the specified array
 *			of corners, an instance is invalidated once the pointer does no longer
 *			point to a valid memory location (e.g. because the underlying corner
 *			array has been resized). A `Grob` thus should only be used as a
 *			temporary object and should only be used if it is clear that the
 *			underlying coner-array is still valid.
 */
class Grob {
public:
	Grob (grob_t grobType, const index_t* corners) :
		m_globCornerInds (corners),
		m_cornerOffsets (impl::Array_16_4::ascending_order ()),
		m_desc (grobType)
	{}

	///	only compares corners, ignores order and orientation.
	bool operator == (const Grob& g) const
	{
		if (m_desc.type() != g.m_desc.type())
			return false;

		index_t gc[TOTAL_MAX_CORNERS];
		g.corners (gc);

		const index_t numCorners = num_corners ();
		for(index_t i = 0; i < numCorners; ++i) {
			bool gotOne = false;
			const index_t c = corner(i);
			for(index_t j = 0; j < numCorners; ++j) {
				if(c == gc[j]){
					gotOne = true;
					break;
				}
			}

			if (!gotOne)
				return false;
		}

		return true;
	}

	inline index_t dim () const							{return m_desc.dim ();}
	
	inline grob_t type () const							{return m_desc.type ();}
	
	inline GrobDesc desc () const 						{return m_desc;}

	inline void set_corners (const index_t* corners)
	{
		m_globCornerInds = corners;
	}

	inline index_t num_corners () const					{return m_desc.num_corners();}

	/// returns the global index of the i-th corner
	inline index_t corner (const index_t i) const
	{
		return m_globCornerInds [m_cornerOffsets.get(i)];
	}

	/// collects the global indices of corners
	/** \param cornersOut	Array of size `MAX_CORNERS [dim]`, where 'dim' has to
	 *						equal 'this->dim()', or simply of size TOTAL_MAX_CORNERS.
	 *
	 * \returns	The number of corners of the specified side
	 */
	inline index_t corners (index_t* cornersOut) const
	{
		const index_t numCorners = num_corners();
		for(index_t i = 0; i < numCorners; ++i)
			cornersOut[i] = static_cast<index_t> (m_globCornerInds [m_cornerOffsets.get(i)]);
		return numCorners;
	}

	inline index_t num_sides (const index_t sideDim) const
	{
		return m_desc.num_sides(sideDim);
	}
	
	inline GrobDesc side_desc (const index_t sideDim, const index_t sideIndex) const
	{
		return m_desc.side_desc (sideDim, sideIndex);
	}

	Grob side (const index_t sideDim, const index_t sideIndex)
	{
		impl::Array_16_4 cornerOffsets;
		const index_t numCorners = m_desc.side_desc(sideDim, sideIndex).num_corners();
		const index_t* locCorners = m_desc.local_side_corners (sideDim, sideIndex);
		// LOGT(side, "side " << sideIndex << "(dim: " << sideDim << ", num corners: " << numCorners << "): ");
		for(index_t i = 0; i < numCorners; ++i){
			// LOG(locCorners[i] << " ");
			cornerOffsets.set (i, m_cornerOffsets.get(locCorners[i]));
		}
		// LOG("\n");

		return Grob (m_desc.side_type (sideDim, sideIndex), m_globCornerInds, cornerOffsets);
	}

private:
	Grob (grob_t grobType, const index_t* globCornerInds, const impl::Array_16_4& cornerOffsets) :
		m_globCornerInds (globCornerInds),
		m_cornerOffsets (cornerOffsets),
		m_desc (grobType)
	{}

	const index_t*		m_globCornerInds;
	impl::Array_16_4	m_cornerOffsets;
	const GrobDesc		m_desc;
};



class GrobTypeList {
public:
	explicit GrobTypeList (grob_list_t glt) :
		m_offset (impl::GROB_LIST_DESC_OFFESTS [glt])
	{}

	explicit GrobTypeList (grob_t glt) :
		m_offset (impl::GROB_LIST_DESC_OFFESTS [glt])
	{}

	index_t type () const			{return impl::GROB_LIST_DESCS [m_offset];}
	index_t dim () const			{return impl::GROB_LIST_DESCS [m_offset + 1];}
	index_t size () const			{return impl::GROB_LIST_DESCS [m_offset + 2];}
	grob_t grob (const index_t i)	{return grob_t (impl::GROB_LIST_DESCS [m_offset + 3 + i]);}

	grob_list_t sides (const index_t sideDim)	{return grob_list_t(impl::GROB_LIST_DESCS [m_offset + 3 + size() + sideDim]);}

private:
	const index_t m_offset;
};

}//	end of namespace slimesh

#endif	//__H__slimesh__grob

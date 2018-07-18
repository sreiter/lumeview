#include "mesh.h"
#include "topology.h"
#include "log.h"
#include "vec_math_raw.h"

using namespace std;

namespace slimesh {

AssociatedElems::
AssociatedElems () :
	m_offsets (make_shared <IndexArrayAnnex> ()),
	m_assElemMap (make_shared <IndexArrayAnnex> ()),
	m_rawOffsets (nullptr),
	m_rawAssElemMap (nullptr)
{
	m_assElemMap->set_tuple_size (2);
}

AssociatedElems::
AssociatedElems (Mesh& mesh, GrobSet elemTypes, GrobSet assElemTypes) :
	m_offsets (make_shared <IndexArrayAnnex> ()),
	m_assElemMap (make_shared <IndexArrayAnnex> ())
{
	m_assElemMap->set_tuple_size (2);
	
	CreateAssociatedElemMap (m_assElemMap->vector(),
	                         m_offsets->vector(),
	                         m_grobBaseInds,
	                         mesh,
	                         elemTypes,
	                         assElemTypes);

	m_rawOffsets	= m_offsets->raw_ptr();
	m_rawAssElemMap	= m_assElemMap->raw_ptr();
}

index_t AssociatedElems::
num_associated (const index_t elemInd, const grob_t elemGT) const
{
	return m_rawOffsets [m_grobBaseInds[elemGT] + elemInd + 1] - m_rawOffsets [m_grobBaseInds[elemGT] + elemInd];
}

index_t AssociatedElems::
ass_elem_ind (const index_t elemInd, const grob_t elemGT, const index_t assElemInd) const
{
	return m_rawAssElemMap [(m_rawOffsets [m_grobBaseInds[elemGT] + elemInd] + assElemInd) * 2 + 1];
}

grob_t AssociatedElems::
ass_elem_type (const index_t elemInd, const grob_t elemGT, const index_t assElemInd) const
{
	return grob_t(m_rawAssElemMap [(m_rawOffsets [m_grobBaseInds[elemGT] + elemInd] + assElemInd) * 2]);
}



TotalToGrobIndexMap::
TotalToGrobIndexMap (Mesh& mesh, const GrobSet& gs) :
    m_grobSet (gs)
{
    COND_THROW (gs.size() > MAX_GROB_SET_SIZE, "Internal error: MAX_GROB_SET_SIZE is wrong!");

    m_baseInds[0] = 0;
    for(index_t i = 0; i < gs.size(); ++i) {
      	const index_t numTuples = gs.grob_type (i) == VERTEX ?
          								mesh.coords()->num_tuples() :
          								mesh.num_tuples (gs.grob_type (i));
          
     	m_baseInds [i+1] = m_baseInds [i] + numTuples;
    }
}

std::pair <index_t, grob_t> TotalToGrobIndexMap::
operator () (const index_t ind) const
{
    for(size_t i = 0; i < m_grobSet.size(); ++i) {
      	if (ind >= m_baseInds [i] && ind < m_baseInds [i+1])
        	return make_pair (ind - m_baseInds[i], m_grobSet.grob_type(index_t(i)));
    }

    THROW("TotalToGrobIndexMap: Couldn't map index " << ind << " to GrobSet " << m_grobSet.name());
    return make_pair <index_t, grob_t> (0, NO_GROB);
}



void FillElemIndexMap (GrobHashMap <index_t>& indexMapInOut,
                       index_t* grobBaseIndsOut,
                       const Mesh& mesh,
                       const GrobSet grobSet)
{
	VecSet (grobBaseIndsOut, 0, NUM_GROB_TYPES);

	index_t counter = 0;
	
	for (auto grobType : grobSet) {
		if (!mesh.has (grobType))
			continue;
		
		grobBaseIndsOut [grobType] = counter;

		Grob grob (grobType);

		const index_t* cornerInds = mesh.inds (grobType)->raw_ptr();
		const index_t numCornerInds = mesh.inds (grobType)->size();
		const index_t numCorners = grob.num_corners();

		for (index_t offset = 0; offset < numCornerInds; offset += numCorners, ++counter) {
			grob.set_corners(cornerInds + offset);
			indexMapInOut.insert (make_pair (grob, counter));
		}
	}
}


index_t FindUniqueSides (GrobHash& sideHashInOut,
                         const index_t* cornerInds,
                         const index_t numCornerInds,
                         const grob_t grobType,
                         const index_t sideDim)
{
	Grob grob (grobType, cornerInds);
	const index_t numGrobCorners = grob.num_corners();
	index_t numInsertions = 0;
	for (index_t igrob = 0; igrob < numCornerInds; igrob += numGrobCorners)
	{
		grob.set_corners(cornerInds + igrob);
		// LOGT(grob, grob.corner(0) << ", " << grob.corner(1) << ", " << grob.corner(2) << ", " << grob.corner(3) << "\n");
		for(index_t iside = 0; iside < grob.num_sides(sideDim); ++iside) {
			// LOGT(grob, grob.side (iside).corner(0) << ", " << grob.side (iside).corner(1) << "\n");
			const auto r = sideHashInOut.insert(grob.side (sideDim, iside));
			numInsertions += static_cast<index_t> (r.second);
		}
	}
	return numInsertions;
}

void GrobHashToIndexArray (std::vector <index_t>& indArrayInOut,
                           const GrobHash& hash)
{
	using iter_t = GrobHash::iterator;
	const iter_t iend = hash.end();
	for (iter_t igrob = hash.begin(); igrob != iend; ++igrob) {
		const Grob& grob = *igrob;
		for(index_t i = 0; i < grob.num_corners(); ++i) {
			indArrayInOut.push_back (grob.corner (i));
		}
	}
}

void GrobHashToTypeArray (std::vector <index_t>& typeArrayInOut,
                          const GrobHash& hash)
{
	using iter_t = GrobHash::iterator;
	const iter_t iend = hash.end();
	for (iter_t igrob = hash.begin(); igrob != iend; ++igrob) {
		typeArrayInOut.push_back (igrob->type());
	}
}

void GrobHashToIndexArray (std::vector <index_t>& indArrayInOut,
                           const GrobHash& hash,
                           grob_t grobType)
{
	using iter_t = GrobHash::iterator;
	const iter_t iend = hash.end();
	for (iter_t igrob = hash.begin(); igrob != iend; ++igrob) {
		const Grob& grob = *igrob;
		if (grob.type () == grobType) {
			for(index_t i = 0; i < grob.num_corners(); ++i) {
				indArrayInOut.push_back (grob.corner (i));
			}
		}
	}
}

void UniqueSidesToIndexArray (std::vector <index_t>& indArrayInOut,
                              const index_t* cornerInds,
                              const index_t numCornerInds,
                              const grob_t grobType,
                              const index_t sideDim)
{
	GrobHash hash;
	FindUniqueSides (hash, cornerInds, numCornerInds, grobType, sideDim);
	GrobHashToIndexArray (indArrayInOut, hash);
}

void CreateEdgeInds (Mesh& mesh)
{
	const std::vector<grob_t> grobs = mesh.grob_types();

	GrobHash hash;
	for(auto gt : grobs) {
		if(GrobDesc(gt).dim() > 1) {
			FindUniqueSides (hash,
							 mesh.inds(gt)->raw_ptr(),
							 mesh.inds(gt)->size(),
							 gt,
							 1);
		}
	}
	
	mesh.inds(slimesh::EDGE)->vector().clear();
	GrobHashToIndexArray (mesh.inds(slimesh::EDGE)->vector(), hash);
}


void CreateFaceInds (Mesh& mesh)
{
//todo: add support for elements with quadrilateral sides!
	const std::vector<grob_t> grobs = mesh.grob_types();

	GrobHash hash;
	for(auto gt : grobs) {
		if(GrobDesc(gt).dim() > 2) {
			FindUniqueSides (hash,
							 mesh.inds(gt)->raw_ptr(),
							 mesh.inds(gt)->size(),
							 gt,
							 2);
		}
	}
	
	mesh.inds(TRI)->vector().clear();
	GrobHashToIndexArray (mesh.inds(TRI)->vector(), hash);
}


SPMesh CreateBoundaryMesh (Mesh& mesh, GrobSet grobSet, const bool* visibilities)
{
	auto bndMesh = make_shared <Mesh> ();
	mesh.share_annexes_with (*bndMesh);
	
	if (grobSet.dim() == 0)
		return bndMesh;

	GrobSet bndGrobSet = grobSet.side_set (grobSet.dim() - 1);
	AssociatedElems assElems (mesh, bndGrobSet, grobSet);

//todo: iterate over all grobs
	for (auto bndGrobType : bndGrobSet) {
		GrobDesc bndGrobDesc (bndGrobType);
		
		const index_t numElemCorners = bndGrobDesc.num_corners();
		const index_t numElemInds = mesh.inds (bndGrobType)->size();
		const index_t numElems = numElemInds / numElemCorners;
		const index_t* elemInds = mesh.inds (bndGrobType)->raw_ptr();

		auto& newElemInds = *bndMesh->inds (bndGrobType);

		if (!visibilities) {
			for(index_t ielem = 0; ielem < numElems; ++ielem) {
				if (assElems.num_associated (ielem, bndGrobType) == 1) {
					const index_t firstInd = ielem * numElemCorners;

					for(index_t i = firstInd; i < firstInd + numElemCorners; ++i) {
						newElemInds.push_back (elemInds [i]);
					}
				}
			}
		}
	}

	return bndMesh;
}


void CreateAssociatedElemMap (std::vector <index_t>& elemMapOut,
                        	  std::vector <index_t>& offsetsOut,
                        	  index_t* grobBaseIndsOut,
                        	  Mesh& mesh,
                        	  GrobSet elemTypes,
                        	  GrobSet assElemTypes)
{
	elemMapOut.clear ();
	offsetsOut.clear ();

	
	const index_t elemDim = elemTypes.dim();
	const index_t assElemDim = assElemTypes.dim();

	const index_t numElems = mesh.num_tuples (elemTypes);
	offsetsOut.resize (numElems + 1, 0);

	// We need a hash map which tells us the index of each elem of type elemType
	GrobHashMap <index_t> elemIndHash;
	FillElemIndexMap (elemIndHash, grobBaseIndsOut, mesh, elemTypes);

	// Count how many associated elements each element has
	if (assElemDim > elemDim) {
		for (auto assElemType : assElemTypes) {
			const index_t numAssElemInds = mesh.inds (assElemType)->size ();
			const index_t numAssElemCorners = mesh.inds (assElemType)->tuple_size ();
			const index_t* assElemCorners = mesh.inds (assElemType)->raw_ptr ();
			Grob assElem (assElemType);

			for(index_t i = 0; i < numAssElemInds; i += numAssElemCorners) {
				assElem.set_corners (assElemCorners + i);
				for(index_t iside = 0; iside < assElem.num_sides(elemDim); ++iside) {
					++offsetsOut[elemIndHash[assElem.side (elemDim, iside)]];
				}
			}
		}
	}
	else {
		THROW ("CreateAssociatedElemMap: Currently, nbr dimension > elem dimension has to hold true");
	}

	// Compute an offset into elemMapOut for each element (convert count -> offset)
	index_t offset = 0;
	for(index_t i = 0; i < offsetsOut.size(); ++i) {
		const index_t curOffset = offset;
		offset += offsetsOut[i];
		offsetsOut[i] = curOffset;
	}

	elemMapOut.resize (offsetsOut.back() * 2, NO_GROB);

	for (auto assElemType : assElemTypes) {
		const index_t numAssElemInds = mesh.inds (assElemType)->size ();
		const index_t numAssElemCorners = mesh.inds (assElemType)->tuple_size ();
		const index_t* assElemCorners = mesh.inds (assElemType)->raw_ptr ();
		Grob assElem (assElemType);

		for(index_t i = 0; i < numAssElemInds; i += numAssElemCorners) {
			assElem.set_corners (assElemCorners + i);
			for(index_t iside = 0; iside < assElem.num_sides(elemDim); ++iside) {
				const index_t eind = elemIndHash[assElem.side (elemDim, iside)];
				const index_t offset = 2 * offsetsOut [eind];
				const index_t numAss = offsetsOut [eind + 1] - offsetsOut [eind];
				for(index_t j = offset; j < offset + 2 * numAss; j+=2) {
					if (elemMapOut [j] == NO_GROB){
						elemMapOut [j] = assElemType;
						elemMapOut [j+1] = i / numAssElemCorners;
						break;
					}
				}
			}
		}
	}
}

}//	end of namespace

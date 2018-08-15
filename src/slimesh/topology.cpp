#include "mesh.h"
#include "topology.h"
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
	
	CreateAssociatedElemMap (*m_assElemMap,
	                         *m_offsets,
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
    if (gs.size() > MAX_GROB_SET_SIZE)
    	SlimeshError ("Internal error: MAX_GROB_SET_SIZE is wrong!");

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

    throw SlimeshError (string("TotalToGrobIndexMap: Couldn't map index ").
    					append (to_string(ind)).
    					append (" to GrobSet ").
    					append (m_grobSet.name()));
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
	
	mesh.inds(slimesh::EDGE)->clear();
	GrobHashToIndexArray (*mesh.inds(slimesh::EDGE), hash);
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
	
	mesh.inds(TRI)->clear();
	GrobHashToIndexArray (*mesh.inds(TRI), hash);
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


}//	end of namespace

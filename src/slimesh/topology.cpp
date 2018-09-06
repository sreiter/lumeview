#include "mesh.h"
#include "topology.h"
#include "vec_math_raw.h"

//todo: remove this include
#include "neighborhoods.h"

using namespace std;

namespace slimesh {

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
          								mesh.num (gs.grob_type (i));
          
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


		for(auto grob : *mesh.inds (grobType)) {
			indexMapInOut.insert (make_pair (grob, counter++));
		}
	}
}


void FillElemIndexMap (GrobHashMap <GrobIndex>& indexMapInOut,
                       const Mesh& mesh,
                       const GrobSet grobSet)
{
	
	for (auto grobType : grobSet) {
		if (!mesh.has (grobType))
			continue;
		
		index_t counter = 0;

		for(auto grob : *mesh.inds (grobType)) {
			indexMapInOut.insert (make_pair (grob, GrobIndex (grobType, counter++)));
		}
	}
}


void ComputeGrobValences (GrobHashMap <index_t>& valencesOut,
                          Mesh& mesh,
                     	  GrobSet grobs,
                     	  GrobSet nbrGrobs)
{
	valencesOut.clear ();

	const index_t grobDim = grobs.dim();
	const index_t nbrGrobDim = nbrGrobs.dim();

	if (grobDim < nbrGrobDim) {
		for(auto nbrGT : nbrGrobs) {
			for(auto nbrGrob : *mesh.inds (nbrGT)) {
				for(index_t iside = 0; iside < nbrGrob.num_sides (grobDim); ++iside) {
					++valencesOut [nbrGrob.side (grobDim, iside)];
				}
			}
		}
	}
	else {
		throw SlimeshError ("ComputeGrobValences is currently only implemented for grobs.dim() < nbrGrobs.dim(). Sorry.");
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
		grob.set_global_corner_array(cornerInds + igrob);
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
							 mesh.inds(gt)->underlying_array().raw_ptr(),
							 mesh.inds(gt)->num_indices(),
							 gt,
							 1);
		}
	}
	
	mesh.inds(slimesh::EDGE)->clear();
	GrobHashToIndexArray (mesh.inds(slimesh::EDGE)->underlying_array(), hash);
}


void CreateFaceInds (Mesh& mesh)
{
//todo: add support for elements with quadrilateral sides!
	const std::vector<grob_t> grobs = mesh.grob_types();

	GrobHash hash;
	for(auto gt : grobs) {
		if(GrobDesc(gt).dim() > 2) {
			FindUniqueSides (hash,
							 mesh.inds(gt)->underlying_array().raw_ptr(),
							 mesh.inds(gt)->num_indices(),
							 gt,
							 2);
		}
	}
	
	mesh.inds(TRI)->clear();
	GrobHashToIndexArray (mesh.inds(TRI)->underlying_array(), hash);
}


static void CopyGrobsByValence (SPMesh target,
                                SPMesh source,
                                GrobSet grobSet,
                                Neighborhoods& srcGrobNeighborhoods,
                                const int valence)
{
	for(auto grobType : grobSet) {
		auto& elems = *source->inds (grobType);
		auto& newElems = *target->inds (grobType);

		index_t index = 0;
		for(auto& grob : elems) {
			const GrobIndex gi (grobType, index);
			if (srcGrobNeighborhoods.neighbors (gi).size() == valence) {
				newElems.push_back (grob);
			}
			++index;
		}
	}
}

SPMesh CreateBoundaryMesh (SPMesh mesh, GrobSet grobSet, const bool* visibilities)
{
	auto bndMesh = make_shared <Mesh> ();
	mesh->share_annexes_with (*bndMesh);
	
	if (grobSet.dim() == 0)
		return bndMesh;

	GrobSet bndGrobSet = grobSet.side_set (grobSet.dim() - 1);
	Neighborhoods neighborhoods (mesh, bndGrobSet, grobSet);

	CopyGrobsByValence (bndMesh, mesh, bndGrobSet, neighborhoods, 1);

	return bndMesh;
}

}//	end of namespace

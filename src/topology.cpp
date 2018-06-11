#include "mesh.h"
#include "topology.h"
#include "log.h"

using namespace std;

namespace slimesh {

AssociatedElems::
AssociatedElems () :
	m_offsets (make_shared <IndexDataBuffer> ()),
	m_assElemMap (make_shared <IndexDataBuffer> ()),
	m_rawOffsets (nullptr),
	m_rawAssElemMap (nullptr)
{
	m_assElemMap->set_tuple_size (2);
}

AssociatedElems::
AssociatedElems (Mesh& mesh, grob_t elemType, grob_t assElemType) :
	m_offsets (make_shared <IndexDataBuffer> ()),
	m_assElemMap (make_shared <IndexDataBuffer> ())
{
	m_assElemMap->set_tuple_size (2);
	
	CreateAssociatedElemMap (m_assElemMap->data(),
	                         m_offsets->data(),
	                         mesh,
	                         elemType,
	                         assElemType);

	m_rawOffsets	= m_offsets->raw_data();
	m_rawAssElemMap	= m_assElemMap->raw_data();
}

index_t AssociatedElems::
num_associated (const index_t elemInd) const
{
	return m_rawOffsets [elemInd + 1] - m_rawOffsets [elemInd];
}

index_t AssociatedElems::
ass_elem_ind (const index_t elemInd, const index_t assElemInd) const
{
	return m_rawAssElemMap [(m_rawOffsets [elemInd] + assElemInd) * 2 + 1];
}

grob_t AssociatedElems::
ass_elem_type (const index_t elemInd, const index_t assElemInd) const
{
	return grob_t(m_rawAssElemMap [(m_rawOffsets [elemInd] + assElemInd) * 2]);
}



void FillElemIndexMap (GrobHashMap <index_t>& indexMapInOut,
					   const index_t* cornerInds,
					   const index_t numCornerInds,
					   const grob_t grobType)
{
	Grob grob (grobType, cornerInds);
	const index_t numGrobCorners = grob.num_corners();

	for (index_t offset = 0; offset < numCornerInds; offset += numGrobCorners) {
		grob.set_corners(cornerInds + offset);
		indexMapInOut.insert (make_pair (grob, offset / numGrobCorners));
	}
}


void FillElemIndexMap (GrobHashMap <index_t>& indexMapInOut,
                       const Mesh& mesh,
                       const grob_t grobType)
{
	FillElemIndexMap (indexMapInOut,
	            	  mesh.inds (grobType)->raw_data(),
	            	  mesh.inds (grobType)->size(),
	            	  grobType);
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
							 mesh.inds(gt)->raw_data(),
							 mesh.inds(gt)->size(),
							 gt,
							 1);
		}
	}
	
	mesh.inds(slimesh::EDGE)->data().clear();
	GrobHashToIndexArray (mesh.inds(slimesh::EDGE)->data(), hash);
}


void CreateFaceInds (Mesh& mesh)
{
//todo: add support for elements with quadrilateral sides!
	const std::vector<grob_t> grobs = mesh.grob_types();

	GrobHash hash;
	for(auto gt : grobs) {
		if(GrobDesc(gt).dim() > 2) {
			FindUniqueSides (hash,
							 mesh.inds(gt)->raw_data(),
							 mesh.inds(gt)->size(),
							 gt,
							 2);
		}
	}
	
	mesh.inds(TRI)->data().clear();
	GrobHashToIndexArray (mesh.inds(TRI)->data(), hash);
}


SPMesh CreateBoundaryMesh (Mesh& mesh, const grob_t grobType, const bool* visibilities)
{
//todo: does not yet support multi-side-types!
	GrobDesc grobDesc (grobType);
	const grob_t bndGrobType = grobDesc.side_desc(grobDesc.dim() - 1, 0).type();
	GrobDesc bndGrobDesc (bndGrobType);


	auto bndMesh = make_shared <Mesh> ();
	bndMesh->set_data ("coords", mesh.coords());
	// bndMesh->set_coords (mesh.coords());

	if (grobDesc.dim() == 0)
		return bndMesh;

	AssociatedElems assElems (mesh, bndGrobType, grobType);

	const index_t numElemCorners = bndGrobDesc.num_corners();
	const index_t numElemInds = mesh.inds (bndGrobType)->size();
	const index_t numElems = numElemInds / numElemCorners;
	const index_t* elemInds = mesh.inds (bndGrobType)->raw_data();

	vector <index_t>& newElemInds = bndMesh->inds (bndGrobType)->data();

	if (!visibilities) {
		for(index_t ielem = 0; ielem < numElems; ++ielem) {
			if (assElems.num_associated (ielem) == 1) {
				const index_t firstInd = ielem * numElemCorners;

				for(index_t i = firstInd; i < firstInd + numElemCorners; ++i) {
					newElemInds.push_back (elemInds [i]);
				}
			}
		}
	}

	return bndMesh;
}


void CreateAssociatedElemMap (std::vector <index_t>& elemMapOut,
                        	  std::vector <index_t>& offsetsOut,
                        	  Mesh& mesh,
                        	  grob_t elemType,
                        	  grob_t assElemType)
{
	const GrobDesc elemDesc (elemType);
	const GrobDesc assElemDesc (assElemType);
	const index_t elemDim = elemDesc.dim();
	const index_t assElemDim = assElemDesc.dim();

	elemMapOut.clear ();
	offsetsOut.clear ();

	const index_t numElems = mesh.inds (elemType)->num_tuples ();
	offsetsOut.resize (numElems + 1, 0);


	const index_t numAssElemCorners = mesh.inds (assElemType)->tuple_size ();
	const index_t numAssElemInds = mesh.inds (assElemType)->size ();
	const index_t* assElemCorners = mesh.inds (assElemType)->raw_data ();
	Grob assElem (assElemType, assElemCorners);

	// We need a hash map which tells us the index of each elem of type elemType
	GrobHashMap <index_t> elemIndHash;
	FillElemIndexMap (elemIndHash, mesh, elemType);

	// Count how many associated elements each element has
	if (assElemDim > elemDim) {
		for(index_t i = 0; i < numAssElemInds; i += numAssElemCorners) {
			assElem.set_corners (assElemCorners + i);
			for(index_t iside = 0; iside < assElem.num_sides(elemDim); ++iside) {
				++offsetsOut[elemIndHash[assElem.side (elemDim, iside)]];
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

	elemMapOut.resize (offsetsOut.back() * 2, INVALID_GROB);

	for(index_t i = 0; i < numAssElemInds; i += numAssElemCorners) {
		assElem.set_corners (assElemCorners + i);
		for(index_t iside = 0; iside < assElem.num_sides(elemDim); ++iside) {
			const index_t eind = elemIndHash[assElem.side (elemDim, iside)];
			const index_t offset = 2 * offsetsOut [eind];
			const index_t numAss = offsetsOut [eind + 1] - offsetsOut [eind];
			for(index_t j = offset; j < offset + 2 * numAss; j+=2) {
				if (elemMapOut [j] == INVALID_GROB){
					elemMapOut [j] = assElemType;
					elemMapOut [j+1] = i / numAssElemCorners;
					break;
				}
			}
		}
	}
}

}//	end of namespace

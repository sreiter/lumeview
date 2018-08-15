// This file is part of slimesh, a C++ mesh library
//
// Copyright (C) 2018 Sebastian Reiter <s.b.reiter@gmail.com>

#ifndef __H__slimesh_topology_impl
#define __H__slimesh_topology_impl

#include "topology.h"

namespace slimesh {

template <class TIndexVector>
void GrobHashToIndexArray (TIndexVector& indArrayInOut,
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

template <class TIndexVector>
void GrobHashToTypeArray (TIndexVector& typeArrayInOut,
                          const GrobHash& hash)
{
	using iter_t = GrobHash::iterator;
	const iter_t iend = hash.end();
	for (iter_t igrob = hash.begin(); igrob != iend; ++igrob) {
		typeArrayInOut.push_back (igrob->type());
	}
}

template <class TIndexVector>
void GrobHashToIndexArray (TIndexVector& indArrayInOut,
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

template <class TIndexVector>
void UniqueSidesToIndexArray (TIndexVector& indArrayInOut,
                              const index_t* cornerInds,
                              const index_t numCornerInds,
                              const grob_t grobType,
                              const index_t sideDim)
{
	GrobHash hash;
	FindUniqueSides (hash, cornerInds, numCornerInds, grobType, sideDim);
	GrobHashToIndexArray (indArrayInOut, hash);
}

template <class TIndexVector>
void CreateAssociatedElemMap (TIndexVector& elemMapOut,
                        	  TIndexVector& offsetsOut,
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
		throw SlimeshError ("CreateAssociatedElemMap: Currently, nbr dimension > elem dimension has to hold true");
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

}//	end of namespace slimesh

#endif	//__H__slimesh_topology_impl

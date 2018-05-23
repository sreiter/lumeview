#ifndef __H__msh__topology
#define __H__msh__topology

#include <vector>
#include "grob.h"

namespace msh {

///	Collects all sides of the *grobs* specified by `cornerInds` and `grobType`.
/** \note	It is assumed, that `cornerInds` holds the corner indices of one or
*			more *grobs* of the same `grobType` in sequential order.
* \note `sideHashInOut is not cleared during this function. It is thus possible
*		call this method repeatedly on different `cornerInds` and `grobType` to
*		find all sides of a hybrid grid.*/
index_t FindUniqueSides (GrobHash& sideHashInOut,
                        const index_t* cornerInds,
                        const index_t numCornerInds,
                        const grob_t grobType,
                        const index_t sideDim);


void GrobHashToIndexArray (std::vector <index_t>& indArrayInOut,
                           const GrobHash& hash);

void GrobHashToTypeArray (std::vector <index_t>& typeArrayInOut,
                          const GrobHash& hash);

void GrobHashToIndexArray (std::vector <index_t>& indArrayInOut,
                           const GrobHash& hash,
                           grob_t grobType);

void UniqueSidesToIndexArray (std::vector <index_t>& indArrayInOut,
                              const index_t* cornerInds,
                              const index_t numCornerInds,
                              const grob_t grobType,
                              const index_t sideDim);
}//	end of namespace msh

#endif	//__H__msh__topology

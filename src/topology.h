#ifndef __H__slimesh__topology
#define __H__slimesh__topology

#include <vector>
#include "grob.h"

namespace slimesh {

class Mesh;

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

void CreateEdgeInds (Mesh& mesh);

}//	end of namespace slimesh

#endif	//__H__slimesh__topology

#ifndef __H__slimesh__topology
#define __H__slimesh__topology

#include <vector>
#include "grob.h"
#include "grob_hash.h"

namespace slimesh {

class Mesh;


class AssociatedElems {
public:
    AssociatedElems ();
    AssociatedElems (Mesh& mesh, grob_t elemType, grob_t assElemType);

    index_t num_associated (const index_t elemInd) const;
    index_t ass_elem_ind (const index_t elemInd, const index_t assElemInd) const;
    grob_t ass_elem_type (const index_t elemInd, const index_t assElemInd) const;

private:
    SPIndexDataBuffer m_offsets;
    SPIndexDataBuffer m_assElemMap;
    index_t*          m_rawOffsets;
    index_t*          m_rawAssElemMap;
};


/// Fills a map which associates grobs with consecutive indices
/** \note   It is assumed, that `cornerInds` holds the corner indices of one or
*           more *grobs* of the same `grobType` in sequential order.
* \note `indexMapInOut is not cleared during this function. It is thus possible
*        call this method repeatedly on different `cornerInds` and `grobType` to
*        find fill all element indices of a hybrid grid.
* \{ */
index_t FillIndexMap (GrobHashMap <index_t>& indexMapInOut,
                      const index_t* cornerInds,
                      const index_t numCornerInds,
                      const grob_t grobType);

index_t FillIndexMap (GrobHashMap <index_t>& indexMapInOut,
                      const Mesh& mesh,
                      const grob_t grobType);
/** \} */


/// Collects all sides of the *grobs* specified by `cornerInds` and `grobType`.
/** \note It is assumed, that `cornerInds` holds the corner indices of one or
*			    more *grobs* of the same `grobType` in sequential order.
* \note  `sideHashInOut is not cleared during this function. It is thus possible
*		     call this method repeatedly on different `cornerInds` and `grobType` to
*		     find all sides of a hybrid grid.*/
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

/// Creates edge indices for a mesh with face or volume elements
void CreateEdgeInds (Mesh& mesh);

/// Creates faces indices for a mesh with volume elements
/** \todo:  add support for elements with quadrilateral sides!*/
void CreateFaceInds (Mesh& mesh);

SPMesh CreateBoundaryMesh (Mesh& mesh, const grob_t grobType, const bool* visibilities);

void CreateAssociatedElemMap (std::vector <index_t>& elemMapOut,
                              std::vector <index_t>& offsetsOut,
                              Mesh& mesh,
                              grob_t elemType,
                              grob_t assElemType);



}//	end of namespace slimesh

#endif	//__H__slimesh__topology

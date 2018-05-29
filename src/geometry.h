#ifndef __H__msh__geometry
#define __H__msh__geometry

#include <memory>
#include "types.h"
#include "grob.h"
#include "mesh.h"

namespace msh {


///	computes the normal of a single triangle
/**
 * \param normalOut	array of length 3. The resulting normal will be written to this array.
 * \param c0		array of length 3. coordinate of first corner.
 * \param c1		array of length 3. coordinate of second corner.
 * \param c2		array of length 3. coordinate of third corner.
 */
real_t* TriangleNormal3 (real_t* normalOut,
                         const real_t* c0,
                         const real_t* c1,
                         const real_t* c2);

/**
 * \param normalsOut	array of the same size as 'coords'
 * \param coords		coordinates of the corners
 * \param numCoords		total number of coordinates
 *						(at least: (highest referenced corner in cornerInds) * 3)
 * \param cornerInds	indices of triangle corners. Each triplet describes on triangle.
 * \param numCornerInds	total number of corner indices.
 */
void ComputeVertexNormals3 (real_t* normalsOut,
                    		const real_t* coords,
                    		const index_t numCoords,
                    		const index_t* cornerInds,
                    		const index_t numCornerInds,
                    		const grob_t grobType);


///	computes the vertex normals of a mesh and stores them in the specified data array
std::shared_ptr <DataArray <real_t>>
ComputeTriVertexNormals3 (Mesh& meshInOut,
                		  std::string normalId = "vrtNormals");

}// end of namespace msh

#endif	//__H__msh__geometry

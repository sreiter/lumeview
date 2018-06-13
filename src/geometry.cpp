#include "cond_throw.h"
#include "geometry.h"
#include "mesh.h"
#include "vec_math_raw.h"

namespace slimesh {

real_t* TriangleNormal3 (real_t* normalOut,
                         const real_t* c0,
                         const real_t* c1,
                         const real_t* c2)
{
	real_t d0[3];
	real_t d1[3];

	VecSub (d0, 3, c1, c0);
	VecSub (d1, 3, c2, c0);

	return VecNormalize (VecCross3 (normalOut, d0, d1), 3);
}


std::shared_ptr <DataBuffer <real_t>>
ComputeFaceVertexNormals3 (Mesh& mesh,
						  const std::string& normalId)
{
	COND_THROW (mesh.coords()->tuple_size() != 3,
	            "ComputeFaceVertexNormals3: Coordinates have to be 3-tuples!");

	auto normalData = mesh.data<real_t> (normalId, VERTEX);
	normalData->set_tuple_size (3);
	normalData->data().resize (mesh.num_coords());

	const real_t*	coords		= mesh.coords()->raw_data();
	const index_t	numCoords	= mesh.num_coords();
	real_t* 		normals		= normalData->raw_data();

	VecSet (normals, 0, numCoords);
	
	for(auto gt : GrobSet (FACES)) {
		const index_t*	inds		= mesh.inds (gt)->raw_data();
		const index_t	numInds		= mesh.inds (gt)->size();
		const index_t	numCorners	= mesh.inds (gt)->tuple_size();
		const index_t	offset = numCorners / 2;

		for (index_t i = 0; i < numInds; i += numCorners) {
			const index_t* elem = inds + i;

			real_t d0[3];
			real_t d1[3];

			VecSub (d0, 3, coords + elem[offset] * 3, coords + elem[0] * 3);
			VecSub (d1, 3, coords + elem[1 + offset] * 3, coords + elem[1] * 3);

			real_t n[3];
			VecNormalize (VecCross3 (n, d0, d1), 3);

			for(index_t j = 0; j < numCorners; ++j)
				VecAppend (normals + elem [j] * 3, 3, n);
		}
	}

	return normalData;
}

}// end of namespace slimesh

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


void ComputeVertexNormals3 (real_t* normalsOut,
							const real_t* coords,
							const index_t numCoords,
							const index_t* cornerInds,
							const index_t numCornerInds,
							const grob_t grobType)
{
	VecSet (normalsOut, 0, numCoords);

	if (grobType == TRI) {
		for(index_t itri = 0; itri < numCornerInds; itri+=3) {
			const index_t* tri = &cornerInds [itri];
			real_t n[3];
			TriangleNormal3 (n, &coords [tri [0]*3], &coords [tri [1]*3], &coords [tri [2]*3]);

			for(index_t i = 0; i < 3; ++i) 
				VecAppend (&normalsOut [tri [i]*3], 3, n);
		}

		VecTupNormalize (normalsOut, numCoords, 3);
	}
	else {
		THROW("VertexNormals3 is currently only implemented for triangle meshes");
	}
}


std::shared_ptr <DataBuffer <real_t>>
ComputeTriVertexNormals3 (Mesh& mesh,
						  std::string normalId)
{
	auto normals = mesh.data<real_t> (std::move(normalId));
	normals->set_tuple_size (3);
	normals->data().resize (mesh.num_coords());

	ComputeVertexNormals3 (normals->raw_data(),
	            		   mesh.coords()->raw_data(),
	            		   mesh.num_coords(),
	            		   mesh.inds(TRI)->raw_data(),
	            		   mesh.inds(TRI)->size(),
	            		   TRI);
	return normals;
}

}// end of namespace slimesh

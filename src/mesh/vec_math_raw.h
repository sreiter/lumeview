#ifndef __H__msh__vec_math_raw
#define __H__msh__vec_math_raw

#include <cmath>
#include "../types.h"

namespace msh {

real_t* VecCopy (real_t* vOut, const real_t* v, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v)
		*vOut = *v;
	return vOut;
}

real_t* VecSet (real_t* vOut, const real_t v, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut)
		*vOut = v;
	return vOut;
}

real_t* VecAppend (real_t* vOut, const real_t* v, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v)
		*vOut += *v;
	return vOut;
}

real_t* VecAdd (real_t* vOut, const real_t* v0, const real_t* v1, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v0, ++v1)
		*vOut = *v0 + *v1;
	return vOut;
}

real_t* VecSub (real_t* vOut, const real_t* v0, const real_t* v1, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v0, ++v1)
		*vOut = *v0 - *v1;
	return vOut;
}

real_t VecDot (const real_t* v0, const real_t* v1, const index_t n)
{
	real_t d = 0;
	for (index_t i = 0; i < n; ++i, ++v0, ++v1)
		d += *v0 * *v1;
	return d;
}

real_t VecLenSq (const real_t* v, const index_t n)
{
	return VecDot (v, v, n);
}

real_t VecLen (const real_t* v, const index_t n)
{
	return static_cast<real_t> (sqrt(VecDot (v, v, n)));
}

real_t VecDistSq (const real_t* v0, const real_t* v1, const index_t n)
{
	real_t d = 0;
	for (index_t i = 0; i < n; ++i, ++v0, ++v1){
		const real_t dif = *v0 - *v1;
		d += dif*dif;
	}
	return d;
}

real_t VecDist (const real_t* v0, const real_t* v1, const index_t n)
{
	return static_cast<real_t> (VecDistSq (v0, v1, n));
}


real_t* VecNormalize (real_t* vOut, const real_t* v, const index_t n)
{
	const real_t l = VecLen (v, n);
	if (l != 0) {
		for (index_t i = 0; i < n; ++i, ++vOut, ++v)
			*vOut = *v / l;
	}
	else
		VecCopy (vOut, v, n);
	return vOut;
}

real_t * VecNormalize (real_t* v, const index_t n)
{
	const real_t l = VecLen (v, n);
	if (l != 0) {
		for (index_t i = 0; i < n; ++i, ++v)
			*v /= l;
	}
	return v;
}


real_t* VecCross3 (real_t* vOut, const real_t* v0, const real_t* v1)
{
	real_t c[3];
	c[0] = v0[1] * v1[2] - v1[1] * v0[2];
	c[1] = v0[2] * v1[0] - v1[2] * v0[0];
	c[2] = v0[0] * v1[1] - v1[0] * v0[1];
	vOut[0] = c[0];
	vOut[1] = c[1];
	vOut[2] = c[2];
	return vOut;
}

}// end of namespace msh

#endif	//__H__msh__vec_math_raw

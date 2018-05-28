#ifndef __H__msh__vec_math_raw
#define __H__msh__vec_math_raw

#include <cmath>
#include "../types.h"

namespace msh {


template <class real_t>
real_t* VecCopy (real_t* vOut, const real_t* v, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v)
		*vOut = *v;
	return vOut;
}

template <class real_t, class value_t>
real_t* VecSet (real_t* vOut, const value_t v, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut)
		*vOut = v;
	return vOut;
}

template <class real_t>
real_t* VecAppend (real_t* vOut, const real_t* v, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v)
		*vOut += *v;
	return vOut;
}

template <class real_t>
real_t* VecAdd (real_t* vOut, const real_t* v0, const real_t* v1, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v0, ++v1)
		*vOut = *v0 + *v1;
	return vOut;
}

template <class real_t>
real_t* VecSub (real_t* vOut, const real_t* v0, const real_t* v1, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v0, ++v1)
		*vOut = *v0 - *v1;
	return vOut;
}

template <class real_t, class value_t>
real_t* VecScale (real_t* vOut, const real_t* v, const value_t s, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vOut, ++v)
		*vOut = *v * s;
	return vOut;
}

template <class real_t, class value_t>
real_t* VecScale (real_t* vInOut, const value_t s, const index_t n)
{
	for (index_t i = 0; i < n; ++i, ++vInOut)
		*vInOut *= s;
	return vInOut;
}

template <class real_t>
real_t VecDot (const real_t* v0, const real_t* v1, const index_t n)
{
	real_t d = 0;
	for (index_t i = 0; i < n; ++i, ++v0, ++v1)
		d += *v0 * *v1;
	return d;
}

template <class real_t>
real_t VecLenSq (const real_t* v, const index_t n)
{
	return VecDot (v, v, n);
}

template <class real_t>
real_t VecLen (const real_t* v, const index_t n)
{
	return static_cast<real_t> (sqrt(VecDot (v, v, n)));
}

template <class real_t>
real_t VecDistSq (const real_t* v0, const real_t* v1, const index_t n)
{
	real_t d = 0;
	for (index_t i = 0; i < n; ++i, ++v0, ++v1){
		const real_t dif = *v0 - *v1;
		d += dif*dif;
	}
	return d;
}

template <class real_t>
real_t VecDist (const real_t* v0, const real_t* v1, const index_t n)
{
	return static_cast<real_t> (VecDistSq (v0, v1, n));
}


template <class real_t>
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

template <class real_t>
real_t* VecNormalize (real_t* v, const index_t n)
{
	const real_t l = VecLen (v, n);
	if (l != 0) {
		for (index_t i = 0; i < n; ++i, ++v)
			*v /= l;
	}
	return v;
}

///	Adds the tuple `vtup` to each tuple in `vInOut`
/** Considers the contents of `v` to be a sequence of tuples of size `ntup`.
 * \param vInOut	resulting vector of size `n`
 * \param vtup		vector of size `ntup`
 * \param n			number of entries in `v`
 * \param ntup		number of entries of one tuple
 */
template <class real_t>
real_t* VecTupAppend (real_t* vInOut, const real_t* vtup, const index_t n, const index_t ntup)
{
	for(index_t i = 0; i < n; i += ntup)
		VecAppend (vInOut + i, vtup, ntup);
	return vInOut;
}

///	Adds the tuple `vtup` to each tuple in `v`
/** Considers the contents of `v` to be a sequence of tuples of size `ntup`.
 * \param vOut		resulting vector of size `n`
 * \param v			vector of size `n`
 * \param vtup		vector of size `ntup`
 * \param n			number of entries in `v`
 * \param ntup		number of entries of one tuple
 */
template <class real_t>
real_t* VecTupAdd (real_t* vOut, const real_t* v, const real_t* vtup, const index_t n, const index_t ntup)
{
	for(index_t i = 0; i < n; i += ntup)
		VecAdd (vOut + i, v + i, vtup, ntup);
	return vOut;
}

///	Subtracts the tuple `vtup` from each tuple in `v`
/** Considers the contents of `v` to be a sequence of tuples of size `ntup`.
 * \param vOut		resulting vector of size `n`
 * \param v			vector of size `n`
 * \param vtup		vector of size `ntup`
 * \param n			number of entries in `v`
 * \param ntup		number of entries of one tuple
 */
template <class real_t>
real_t* VecTupSub (real_t* vOut, const real_t* v, const real_t* vtup, const index_t n, const index_t ntup)
{
	for(index_t i = 0; i < n; i += ntup)
		VecSub (vOut + i, v + i, vtup, ntup);
	return vOut;
}

///	Normalizes each tuple in `v`
/** Considers the contents of `v` to be a sequence of tuples of size `ntup`.
 * \param vOut		resulting vector of size `n`
 * \param v			vector of size `n`
 * \param n			number of entries in `v`
 * \param ntup		number of entries of one tuple
 */
template <class real_t>
real_t* VecTupNormalize (real_t* vOut, const real_t* v, const index_t n, const index_t ntup)
{
	for(index_t i = 0; i < n; i += ntup)
		VecNormalize (vOut + i, v + i, ntup);
	return vOut;
}

///	Normalizes each tuple in `vInOut`
/** Considers the contents of `vInOut` to be a sequence of tuples of size `ntup`.
 * \param vInOut	resulting vector of size `n`
 * \param n			number of entries in `v`
 * \param ntup		number of entries of one tuple
 */
template <class real_t>
real_t* VecTupNormalize (real_t* vInOut, const index_t n, const index_t ntup)
{
	for(index_t i = 0; i < n; i += ntup)
		VecNormalize (vInOut + i, ntup);
	return vInOut;
}

///	Computes the sum of all tuples in `v`.
/** Considers the contents of `v` to be a sequence of tuples of size `ntup`.
 * \param tupOut	the resulting tuple. Array of size `ntup`
 * \param v			vector of size `n`
 * \param n			number of entries in `v`
 * \param ntup		number of entries of one tuple
 */
template <class real_t>
real_t* VecTupSum (real_t* tupOut, const real_t* v, const index_t n, const index_t ntup)
{
	VecSet (tupOut, 0, ntup);
	for(index_t i = 0; i < n; i += ntup)
		VecAppend (tupOut, v + i, ntup);
	return tupOut;
}

///	Computes the average of all tuples in `v`.
/** Considers the contents of `v` to be a sequence of tuples of size `ntup`.
 * \param tupOut	the resulting tuple. Array of size `ntup`
 * \param v			vector of size `n`
 * \param n			number of entries in `v`
 * \param ntup		number of entries of one tuple
 */
template <class real_t>
real_t* VecTupAverage (real_t* tupOut, const real_t* v, const index_t n, const index_t ntup)
{
	VecTupSum (tupOut, v, n, ntup);
	if (ntup > 0 && n >= ntup)
		VecScale (tupOut, real_t(1) / (n / ntup), ntup);
	return tupOut;
}


template <class real_t>
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

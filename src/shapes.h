#ifndef __H__shapes
#define __H__shapes

#include <glm/detail/type_vec3.hpp>

template <class real_t>
struct TBox {
	TBox ();
	TBox (real_t min, real_t max);
	TBox (const glm::tvec3<real_t>& min, const glm::tvec3<real_t>& max);
	TBox (glm::tvec3<real_t>&& min, glm::tvec3<real_t>&& max);
	TBox (const TBox& box);
	TBox (TBox&& box);

	glm::tvec3<real_t>	minCorner;
	glm::tvec3<real_t>	maxCorner;
};

using Box = TBox <float>;
using DBox = TBox <double>;


template <class real_t>
struct TSphere {
	TSphere ();
	TSphere (const glm::tvec3<real_t>& center, const real_t radius);
	TSphere (glm::tvec3<real_t>&& center, const real_t radius);
	TSphere (const TSphere& sphere);
	TSphere (TSphere&& sphere);

	glm::tvec3<real_t>	center;
	real_t				radius;
};

using Sphere = TSphere <float>;
using DSphere = TSphere <double>;



///	Calcualtes the bounding box to an array of coordinates
/**
 * \param coords	array of all coordinates of the form:
 *					`x0y0x1y1x2y2x3y3` (num 8, stride 2) or
 *					`x0y0z0x1y1z1x2y2z2` (num 9, stride 3).
 *
 * \param num		total number of entries in `coords`. num should be a
 *					multiple of `stride`.
 *
 * \param stride	number of floating point values  that make up a
 *					coordinate-tuple.
 */
template <class real_t>
TBox <real_t> BoxFromCoordinates (const real_t* coords, size_t num, size_t stride);


///	Calcualtes a bounding sphere to an array of coordinates
/**
 * \param coords	array of all coordinates of the form:
 *					`x0y0x1y1x2y2x3y3` (num 8, stride 2) or
 *					`x0y0z0x1y1z1x2y2z2` (num 9, stride 3).
 *
 * \param num		total number of entries in `coords`. num should be a
 *					multiple of `stride`.
 *
 * \param stride	number of floating point values  that make up a
 *					coordinate-tuple.
 *
 * \note	The computed sphere is not necessarily the minimal sphere containing all points.
 *			However, it is guaranteed that the sphere contains all points and that at least one
 *			point lies on the rim of the sphere.*/
template <class real_t>
TSphere <real_t> SphereFromCoordinates (const real_t* coords, size_t num, size_t stride);

#endif	//__H__shapes

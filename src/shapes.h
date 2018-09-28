// This file is part of lumeview, a lightweight viewer for unstructured meshes
//
// Copyright (C) 2018 Sebastian Reiter
// Copyright (C) 2018 G-CSC, Goethe University Frankfurt
// Author: Sebastian Reiter <s.b.reiter@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef __H__shapes
#define __H__shapes

#include <glm/detail/type_vec3.hpp>
#include "config.h"

namespace lumeview {

template <class real_t>
struct TBox {
	TBox ();
	TBox (real_t min, real_t max);
	TBox (const glm::tvec3<real_t>& min, const glm::tvec3<real_t>& max);
	TBox (glm::tvec3<real_t>&& min, glm::tvec3<real_t>&& max);
	TBox (const TBox& box);
	TBox (TBox&& box);

	TBox& operator = (const TBox& box);

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

	TSphere& operator = (const TSphere& sphere);

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
TBox <real_t> BoxFromCoords (const real_t* coords, index_t num, index_t stride);


///	Calcualtes the bounding box to coordinates in a `ArrayAnnex`
/**
 * \param coords	`ArrayAnnex` of all coordinates of the form:
 *					`x0y0x1y1x2y2x3y3` (num 8, tuple_size 2) or
 *					`x0y0z0x1y1z1x2y2z2` (num 9, tuple_size 3).
 */
// template <class real_t>
// TBox <real_t> BoxFromCoords (ArrayAnnex <real_t>& coords)
// {
// 	return BoxFromCoords (coords.raw_data(), coords.size(), coords.tuple_size());
// }


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
TSphere <real_t> SphereFromCoords (const real_t* coords, index_t num, index_t stride);

}// end of namespace lumeview

#endif	//__H__shapes

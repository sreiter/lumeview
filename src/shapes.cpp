#include <algorithm>
#include <limits>
#include <glm/gtc/type_ptr.hpp>
#include "cond_throw.h"
#include "shapes.h"
#include "vec_math_raw.h"
using namespace std;

namespace slimesh {

template <class real_t>
TBox <real_t>::TBox ()
{
}

template <class real_t>
TBox <real_t>::TBox (real_t min, real_t max) :
	minCorner (min),
	maxCorner (max)
{
}

template <class real_t>
TBox <real_t>::TBox (const glm::tvec3<real_t>& min, const glm::tvec3<real_t>& max) :
	minCorner (min),
	maxCorner (max)
{
}

template <class real_t>
TBox <real_t>::TBox (glm::tvec3<real_t>&& min, glm::tvec3<real_t>&& max) :
	minCorner (std::move (min)),
	maxCorner (std::move (max))
{
}

template <class real_t>
TBox <real_t>::TBox (const TBox& box) :
	minCorner (box.minCorner),
	maxCorner (box.maxCorner)
{}

template <class real_t>
TBox <real_t>::TBox (TBox&& box) :
	minCorner (std::move(box.minCorner)),
	maxCorner (std::move(box.maxCorner))
{}

template <class real_t>
TBox <real_t>& TBox <real_t>::
operator = (const TBox& box)
{
	minCorner = box.minCorner;
	maxCorner = box.maxCorner;

	return *this;
}


template struct TBox <float>;
template struct TBox <double>;


template <class real_t>
TBox <real_t> BoxFromCoords (const real_t* coords, index_t num, index_t stride)
{
	const index_t cmps = std::min<index_t> (stride, 3);

	TBox <real_t> b (numeric_limits<real_t>::max(), numeric_limits<real_t>::lowest());
	for(index_t i = 0; i < num; i+=stride){
		for(index_t j = 0; j < cmps; ++j){
			if (coords [i+j] < b.minCorner[j])
				b.minCorner[j] = coords [i+j];
			if (coords [i+j] > b.maxCorner[j])
				b.maxCorner[j] = coords [i+j];
		}
	}

	for(index_t i = stride; i < 3; ++i){
		b.minCorner[i] = 0;
		b.maxCorner[i] = 0;
	}

	return b;
}

template TBox <float> BoxFromCoords <float> (const float*, index_t, index_t);
template TBox <double> BoxFromCoords <double> (const double*, index_t, index_t);



template <class real_t>
TSphere <real_t>::TSphere ()
{}


template <class real_t>
TSphere <real_t>::TSphere (const glm::tvec3<real_t>& _center, const real_t _radius) :
	center (_center),
	radius (_radius)
{}


template <class real_t>
TSphere <real_t>::TSphere (glm::tvec3<real_t>&& _center, const real_t _radius) :
	center (std::move (_center)),
	radius (_radius)
{}


template <class real_t>
TSphere <real_t>::TSphere (const TSphere& sphere) :
	center (sphere.center),
	radius (sphere.radius)
{}


template <class real_t>
TSphere <real_t>::TSphere (TSphere&& sphere) :
	center (std::move (sphere.center)),
	radius (sphere.radius)
{}

template <class real_t>
TSphere <real_t>& TSphere <real_t>::
operator = (const TSphere& sphere)
{
	center = sphere.center;
	radius = sphere.radius;
	return *this;
}

template struct TSphere <float>;
template struct TSphere <double>;


template <class real_t>
TSphere <real_t> SphereFromCoords (const real_t* coords, index_t num, index_t stride)
{
	COND_THROW (stride > 3, "SphereFromCoords: Max stride of 3 supported. "
	            "Given: " << stride);
	COND_THROW (stride == 0 || num < stride,
	            "SphereFromCoords: At least one coordinate is required");

//	compute center
	glm::tvec3<real_t> center (0);
	real_t* pcenter = glm::value_ptr(center);
	VecTupAverage (pcenter, coords, num, stride);

//	find the coordinate with the largest index
	const index_t cmps = std::min<index_t> (stride, 3);

	real_t maxRadSq = 0;
	for(index_t i = 0; i < num; i+=stride) {
		const real_t d = VecDistSq (pcenter, stride, coords + i);
		if (d > maxRadSq)
			maxRadSq = d;
	}

	return TSphere <real_t> (center, sqrt(maxRadSq));
}

template TSphere <float> SphereFromCoords <float> (const float*, index_t, index_t);
template TSphere <double> SphereFromCoords <double> (const double*, index_t, index_t);

}// end of namespace slimesh

#include <algorithm>
#include <limits>
#include <glm/gtc/type_ptr.hpp>
#include "cond_throw.h"
#include "shapes.h"
#include "vec_math_raw.h"
using namespace std;

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

template class TBox <float>;
template class TBox <double>;



template <class real_t>
TBox <real_t> BoxFromCoordinates (const real_t* coords, size_t num, size_t stride)
{
	const size_t cmps = std::min<size_t> (stride, 3);

	TBox <real_t> b (numeric_limits<real_t>::max(), numeric_limits<real_t>::lowest());
	for(size_t i = 0; i < num; i+=stride){
		for(size_t j = 0; j < cmps; ++j){
			if (coords [i+j] < b.minCorner[j])
				b.minCorner[j] = coords [i+j];
			if (coords [i+j] > b.maxCorner[j])
				b.maxCorner[j] = coords [i+j];
		}
	}

	for(size_t i = stride; i < 3; ++i){
		b.minCorner[i] = 0;
		b.maxCorner[i] = 0;
	}

	return b;
}

template TBox <float> BoxFromCoordinates <float> (const float*, size_t, size_t);
template TBox <double> BoxFromCoordinates <double> (const double*, size_t, size_t);



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

template class TSphere <float>;
template class TSphere <double>;


template <class real_t>
TSphere <real_t> SphereFromCoordinates (const real_t* coords, size_t num, size_t stride)
{
	COND_THROW (stride > 3, "SphereFromCoordinates: Max stride of 3 supported. "
	            "Given: " << stride);
	COND_THROW (stride == 0 || num < stride,
	            "SphereFromCoordinates: At least one coordinate is required");

//	compute center
	glm::tvec3<real_t> center (0);
	real_t* pcenter = glm::value_ptr(center);
	msh::VecTupAverage (pcenter, coords, num, stride);

//	find the coordinate with the largest index
	const size_t cmps = std::min<size_t> (stride, 3);

	real_t maxRadSq = 0;
	for(int i = 0; i < num; i+=stride) {
		const real_t d = msh::VecDistSq (pcenter, coords + i, stride);
		if (d > maxRadSq)
			maxRadSq = d;
	}

	return TSphere <real_t> (center, sqrt(maxRadSq));
}

template TSphere <float> SphereFromCoordinates <float> (const float*, size_t, size_t);
template TSphere <double> SphereFromCoordinates <double> (const double*, size_t, size_t);
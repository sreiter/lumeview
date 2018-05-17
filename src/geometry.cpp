#include <algorithm>
#include <limits>
#include "geometry.h"

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
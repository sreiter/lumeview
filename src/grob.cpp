#include "grob.h"

using namespace std;

namespace slimesh {
namespace impl {

void PrintGrobDescs ()
{
	LOG ("Overview over all available grid objects:\n\n");
	for(index_t igrob = 0; igrob < INVALID_GROB; ++igrob) {
		GrobDesc desc (static_cast<grob_t> (igrob));

		LOG(igrob + 1 << ": " << desc.name() << endl);
		LOG("dim: " << desc.dim() << endl);

		for(index_t sideDim = 0; sideDim < desc.dim(); ++sideDim) {
			const index_t numSides = desc.num_sides (sideDim);
			LOG("sides of dim " << sideDim << ": " << numSides << endl);
			for(index_t iside = 0; iside < numSides; ++iside) {
				LOG("  side " << iside << ": type = " <<
				    desc.side_desc (sideDim, iside).name() << ", on corners:");

				const index_t* corners = desc.local_side_corners (sideDim, iside);
				for(index_t i = 0; i < desc.num_side_corners (sideDim, iside); ++i)
				{
					LOG(" " << corners[i]);
				}
				LOG(endl);
			}
		}
		LOG(endl);
	}
}

}// end of namespace impl

const std::string& GrobName (grob_t grob)
{
	static const std::string names [] = {
		std::string ("vertex"),
		std::string ("edge"),
		std::string ("tri"),
		std::string ("quad"),
		std::string ("tet"),
		std::string ("invalid")
	};

	return names[grob];
}

}// end of namespace slimesh
#include "grob.h"
#include <algorithm>

using namespace std;

namespace slimesh {
namespace impl {

void PrintGrobDescs ()
{
	LOG ("Overview over all available grid objects:\n\n");
	for(index_t igrob = 0; igrob < NO_GROB; ++igrob) {
		GrobDesc desc (static_cast<grob_t> (igrob));

		LOG(igrob + 1 << ": " << desc.name() << endl);
		LOG("dim: " << desc.dim() << endl);

		for(index_t sideDim = 0; sideDim < desc.dim(); ++sideDim) {
			const index_t numSides = desc.num_sides (sideDim);
			LOG("side set of dim " << sideDim << ": " << GrobSetName (desc.side_set_type (sideDim)) << endl);
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

void PrintGrobSetDescs ()
{
	LOG ("Overview over all available grid objects sets:\n\n");
	for(index_t igrobSet = 0; igrobSet <= CELLS; ++igrobSet) {
		GrobSet gs (static_cast<grob_set_t> (igrobSet));

		LOG(igrobSet + 1 << ": " << gs.name() << endl);
		LOG("dim:  " << gs.dim() << endl);
		LOG("size: " << gs.size() << endl);

		LOG("grobs:")
		
		for (auto gt : gs)
			LOG(" " << GrobName (gt));

		LOG(endl);

		for(index_t sideDim = 0; sideDim < gs.dim(); ++sideDim) 
			LOG("side set " << sideDim << "D: " << GrobSetName (gs.side_set(sideDim)) << endl);

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
		std::string ("hex"),
		std::string ("pyra"),
		std::string ("prism"),
		std::string ("invalid")
	};

	return names[grob];
}

const std::string& GrobSetName (grob_set_t grobSet)
{
	static const std::string names [] = {
		std::string ("vertices"),
		std::string ("edges"),
		std::string ("tris"),
		std::string ("quads"),
		std::string ("tets"),
		std::string ("hexs"),
		std::string ("pyras"),
		std::string ("prisms"),
		std::string ("invalid"),
		std::string ("faces"),
		std::string ("cells"),
	};

	return names[grobSet];
}

}// end of namespace slimesh
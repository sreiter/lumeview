#include "mesh.h"
#include "topology.h"
#include "log.h"

namespace slimesh {

index_t FindUniqueSides (GrobHash& sideHashInOut,
                         const index_t* cornerInds,
                         const index_t numCornerInds,
                         const grob_t grobType,
                         const index_t sideDim)
{
	Grob grob (grobType, cornerInds);
	const index_t numGrobCorners = grob.num_corners();
	index_t numInsertions = 0;
	for (index_t igrob = 0; igrob < numCornerInds; igrob += numGrobCorners)
	{
		grob.set_corners(cornerInds + igrob);
		// LOGT(grob, grob.corner(0) << ", " << grob.corner(1) << ", " << grob.corner(2) << "\n");
		for(index_t iside = 0; iside < grob.num_sides(); ++iside) {
			// LOGT(grob, grob.side (iside).corner(0) << ", " << grob.side (iside).corner(1) << "\n");
			auto r = sideHashInOut.insert(grob.side (iside));
			numInsertions += static_cast<index_t> (r.second);
		}
	}
	return numInsertions;
}

void GrobHashToIndexArray (std::vector <index_t>& indArrayInOut,
                           const GrobHash& hash)
{
	using iter_t = GrobHash::iterator;
	const iter_t iend = hash.end();
	for (iter_t igrob = hash.begin(); igrob != iend; ++igrob) {
		const Grob& grob = *igrob;
		for(index_t i = 0; i < grob.num_corners(); ++i) {
			indArrayInOut.push_back (grob.corner (i));
		}
	}
}

void GrobHashToTypeArray (std::vector <index_t>& typeArrayInOut,
                          const GrobHash& hash)
{
	using iter_t = GrobHash::iterator;
	const iter_t iend = hash.end();
	for (iter_t igrob = hash.begin(); igrob != iend; ++igrob) {
		typeArrayInOut.push_back (igrob->type());
	}
}

void GrobHashToIndexArray (std::vector <index_t>& indArrayInOut,
                           const GrobHash& hash,
                           grob_t grobType)
{
	using iter_t = GrobHash::iterator;
	const iter_t iend = hash.end();
	for (iter_t igrob = hash.begin(); igrob != iend; ++igrob) {
		const Grob& grob = *igrob;
		if (grob.type () == grobType) {
			for(index_t i = 0; i < grob.num_corners(); ++i) {
				indArrayInOut.push_back (grob.corner (i));
			}
		}
	}
}

void UniqueSidesToIndexArray (std::vector <index_t>& indArrayInOut,
                              const index_t* cornerInds,
                              const index_t numCornerInds,
                              const grob_t grobType,
                              const index_t sideDim)
{
	GrobHash hash;
	FindUniqueSides (hash, cornerInds, numCornerInds, grobType, sideDim);
	GrobHashToIndexArray (indArrayInOut, hash);
}

void CreateEdgeInds (Mesh& mesh)
{
	const std::vector<grob_t> grobs = mesh.grob_types();

	GrobHash hash;
	for(auto gt : grobs) {
		if(GrobDesc(gt).dim() > 1) {
			FindUniqueSides (hash,
							 mesh.inds(gt)->raw_data(),
							 mesh.inds(gt)->size(),
							 gt,
							 1);
		}
	}
	
	mesh.inds(slimesh::EDGE)->data().clear();
	GrobHashToIndexArray (mesh.inds(slimesh::EDGE)->data(), hash);
}

}//	end of namespace

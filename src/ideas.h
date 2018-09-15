auto vis = make_shared <VisualizationAnnex> ();

vis->



ExtractSides (Mesh& meshOut, const Mesh& mesh, grob_t grobType, GrobSet sideSet,
			  std::function<bool (const GrobIndex&),
			  SPIndexArrayAnnex srcIndicesOut = SPIndexArrayAnnex ());

ExtractGrobs (Mesh& meshOut, const Mesh& mesh, grob_t grobType,
			  std::function<bool (const GrobIndex&)>,
			  SPIndexArrayAnnex srcIndicesOut = SPIndexArrayAnnex ());

ExtractGrobs (std::vector<SPMesh>& meshesInOut, const Mesh& mesh, grob_t grobType,
			  const std::string& indexAnnex,
			  SPIndexArrayAnnex srcIndicesOut = SPIndexArrayAnnex ());

// SubsetVisualization: Create one mesh for each subset
// Forward SubsetInfoAnnex to SubMeshes
// Create a SubMeshInfoAnnex which stores the original mesh and an index

class SubsetVisualization {
public:
	SubsetVisualization ();

	void set_mesh (SPMesh mesh, GrobSet grobSet)
	{
		m_mesh = mesh;
		m_subsetMeshes.clear();
		
		for(auto gt : grobSet) {
			COND_THROW (!m_mesh->has_annex <IndexArrayAnnex> ("subset", gt),
			            "Provided mesh does not provide an IndexArrayAnnex with "
			            "name 'subset' for the requested grob type " << GrobName (gt));

			COND_THROW (mesh->annex <IndexArrayAnnex> ("subset", gt)->size() != m_mesh->num (gt),
			            "subset annex has wrong size ("
			            << mesh->annex <IndexArrayAnnex> ("subset", gt)->size()
			            << ") for grob type " << GrobName (gt) << ". Expected was "
			            << m_mesh->num (gt));
		}

		if (m_grobSet == CELLS) {
			m_neighborhood.refresh (mesh, FACES, CELLS);
			subsetMeshesFromCellRim ();
		}
		else
			subsetMeshesFromGrobs (mesh, m_grobSet);

	}

	void subsetMeshesFromCellRim () {
		m_rimMesh->clear_geometry();
		m_rimMesh->clear_annex_contents();
		m_rimMesh->set_coords (m_mesh->coords());

	//	extract the rim of the visible elements
		for(auto grobType : GrobSet (CELLS)) {
			auto isVis = m_mesh->existing_annex <BoolArrayAnnex> ("visible", grobType);
			// here we reuse the 'subset' annex to temporarily store the srcCellIndices.
			// We will later replace those by the actual subset index.
			auto srcCellIndexAnnex = m_rimMesh->annex <IndexArrayAnnex> ("subset", grobType);
			srcCellIndexAnnex->resize (m_mesh->num (grobType));

			if (isVis) {
				CreateRimMesh (m_rimMesh, m_mesh,
				               [&isVis] (const GrobIndex& gi) {return isVis [gi.index];},
				               srcCellIndexAnnex);
			}
			else {
				CreateRimMesh (m_rimMesh, m_mesh, srcCellIndexAnnex);
			}

		//	replace cellIndex with cellSubsetIndex
			auto subset = srcCellIndexAnnex;
			auto srcCellSubset = m_mesh->annex <IndexArrayAnnex> ("subset", grobType);
			for(index_t i = 0; i < subset.size(); ++i)
				subset [i] = srcCellSubset [subset[i]];
		}
	}
};
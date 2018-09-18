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

class IVisualization;
class MeshVisualization;
class FaceColorVisualization;
class WireFrameVisualization;

class SubsetVisualization {
public:
	SubsetVisualization ();

	void set_mesh (SPMesh mesh, GrobSet grobSet)
	{
		m_mesh = mesh;
		m_grobSet = grobSet;
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
	//	AnnexTable: Allows lookup into annexes of different grobs of the same GrobSet based on a GrobIndex
		isVis = ArrayAnnexTable <BoolArrayAnnex> (m_mesh, "visible", CELLS, false); // last param: createIfMissing==false
		srcSubset = ArrayAnnexTable <IndexArrayAnnex> (m_mesh, "subset", CELLS, false); // last param: createIfMissing==false
		rimSubset = ArrayAnnexTable <IndexArrayAnnex> (m_rimMesh, "subset", FACES, true); // last param: createIfMissing==true

		auto gotRimElem = [&srcSubset, &rimSubset] (const GrobIndex& srcGrob, const GrobIndex& rimGrob)
						  {rimSubset.annex(rimGrob.grobType).push_back (srcSubset[srcGrob]);}

		if (isVis) {
			CreateRimMesh (m_rimMesh, m_mesh,
			               [&isVis] (const GrobIndex& gi) {return isVis [gi];},
			               gotRimElem);
		}
		else {
			CreateRimMesh (m_rimMesh, m_mesh, srcCellIndexAnnex, gotRimElem);
		}

		subsetMeshesFromGrobs (m_rimMesh, FACES);
	}
};
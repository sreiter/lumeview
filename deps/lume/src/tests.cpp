// This file is part of lume, a C++ library for lightweight unstructured meshes
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


#include "lume/custom_exception.h"
#include "lume/grob.h"
#include "lume/file_io.h"
#include "lume/parallel_for.h"
#include "lume/tests.h"
#include "lume/topology.h"
#include "lume/neighborhoods.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

namespace lume {
namespace tests {

DECLARE_CUSTOM_EXCEPTION (TestError, LumeError);

#define FAIL(msg)	{std::stringstream ss; ss << msg; throw(TestError(ss.str()));}
#define COND_FAIL(cond, msg)	if(cond){FAIL(msg);}


class TestStats {
public:
	TestStats () : m_numTests (0), m_numFailed (0) {}

	void success()	{++m_numTests;}
	void fail()		{++m_numTests; ++m_numFailed;}

	index_t num_tests () const		{return m_numTests;}
	index_t num_failed () const		{return m_numFailed;}
	index_t num_succeeded () const	{return num_tests() - num_failed();}

private:
	index_t m_numTests;
	index_t m_numFailed;
};



namespace impl {

	/// tests whether the grob_type() method of a GrobDesc of a given grob_t returns the correct grob_type.
	static void TestGrobDescType (grob_t gt)
	{
		COND_FAIL (GrobDesc (gt).grob_type () != gt,
		           "Bad GrobDesc::grob_type() for grob type " << gt);
	}

	static void TestGrobDescTypes (grob_set_t gs)
	{
		for(auto gt : GrobSet(gs)) {
			TestGrobDescType (gt);
		}
	}
}

static void TestGrobDescs ()
{
	impl::TestGrobDescTypes (VERTICES);
	impl::TestGrobDescTypes (EDGES);
	impl::TestGrobDescTypes (FACES);
	impl::TestGrobDescTypes (CELLS);

//todo: lots more to be tested
}

static void TestCreateMeshFromFile (const string& meshName)
{
	CreateMeshFromFile (meshName);
}


namespace impl {
	void TestGrobArrayLength (Mesh& mesh, grob_t grobType)
	{
		if (!mesh.has (grobType))
			return;

		GrobArray& ga = mesh.grobs (grobType);
		IndexArrayAnnex& ia = ga.underlying_array ();

		COND_FAIL (ia.tuple_size() != ga.grob_desc().num_corners(),
		           "Tuple size of underlying index array does not match the number of corners of the given GrobArray");
		COND_FAIL (ga.size() != ia.num_tuples(),
		           "Size of GrobArray does not match the number of tuples of the underlying index array");

	}

	void TestGrobArrayLength (Mesh& mesh, GrobSet grobSet)
	{
		for(auto gt : grobSet) {
			TestGrobArrayLength (mesh, gt);			
		}
	}
}// end of namespace


static void TestGrobArrays (SPMesh mesh)
{
	impl::TestGrobArrayLength (*mesh, GrobSet(VERTICES));
	impl::TestGrobArrayLength (*mesh, GrobSet(EDGES));
	impl::TestGrobArrayLength (*mesh, GrobSet(FACES));
	impl::TestGrobArrayLength (*mesh, GrobSet(CELLS));
}


namespace impl {
	void TestGrobIterator (Mesh& mesh, grob_t grobType)
	{
		if (!mesh.has (grobType))
			return;

		GrobArray& grobArray = mesh.grobs (grobType);
		IndexArrayAnnex& indArray = grobArray.underlying_array ();

		index_t indCounter = 0;
		for(auto grob : grobArray) {
			for(index_t i = 0; i < grob.num_corners(); ++i) {
				COND_FAIL (grob.corner (i) != indArray [indCounter + i],
				           "Corner index mismatch between GrobIterator and raw array offsets into underlying index array.");
			}
			indCounter += grob.num_corners();
		}
	}

	void TestGrobIterator (Mesh& mesh, GrobSet grobSet)
	{
		for(auto gt : grobSet) {
			TestGrobIterator (mesh, gt);			
		}
	}
}// end of namespace

static void TestGrobIterator (SPMesh mesh)
{
	impl::TestGrobIterator (*mesh, GrobSet(VERTICES));
	impl::TestGrobIterator (*mesh, GrobSet(EDGES));
	impl::TestGrobIterator (*mesh, GrobSet(FACES));
	impl::TestGrobIterator (*mesh, GrobSet(CELLS));
}


namespace impl {
	static void TestGrobIndexAccess (SPMesh mesh, GrobSet grobTypes)
	{
		for(auto grobType : grobTypes) {
			index_t counter = 0;
			for(auto grob : mesh->grobs (grobType)) {
				COND_FAIL (grob != mesh->grob (GrobIndex (grobType, counter)),
				           "mesh->grob(" << counter << ") doesn't correspond to "
				           << counter << "'th grob as provided by the GrobIterator");
				++counter;
			}
		}
	}

}// end of namespace impl
static void TestGrobIndexAccess (SPMesh mesh)
{
	impl::TestGrobIndexAccess (mesh, VERTICES);
	impl::TestGrobIndexAccess (mesh, EDGES);
	impl::TestGrobIndexAccess (mesh, FACES);
	impl::TestGrobIndexAccess (mesh, CELLS);
}


namespace impl {
	static void TestSidesCorrespondToElements (Mesh& mesh, GrobSet grobTypes, GrobSet sideTypes)
	{
		const index_t sideDim = sideTypes.dim();
		COND_FAIL (grobTypes.dim() <= sideDim, "BAD TEST SETUP");

	//	todo: This implementation is slow for large meshes
		for(auto gt : grobTypes) {
			for(auto grob : mesh.grobs(gt)) {
				for(index_t iside = 0; iside < grob.num_sides (sideDim); ++iside) {
					Grob sideGrob = grob.side (sideDim, iside);

				// check whether sideGrob is contained in the set of sides
					bool gotOne = false;
					for(auto sideGT : sideTypes) {
						for(auto testGrob : mesh.grobs(sideGT)) {
							if (testGrob == sideGrob){
								gotOne = true;
								break;
							}
						}
					}

					COND_FAIL (!gotOne, "Couldn't find side " << iside << " (" << sideGrob.desc().name()
					           << ") of grob " << grob.desc().name() << " in the array of grobs of type '"
					           << sideTypes.name() << "'");
				}
			}
		}
	}
}// end of namespace impl

static void TestConsistentTopology (SPMesh mesh)
{
	impl::TestSidesCorrespondToElements (*mesh, EDGES, VERTICES);
	impl::TestSidesCorrespondToElements (*mesh, FACES, VERTICES);
	impl::TestSidesCorrespondToElements (*mesh, FACES, EDGES);
	impl::TestSidesCorrespondToElements (*mesh, CELLS, VERTICES);
	impl::TestSidesCorrespondToElements (*mesh, CELLS, EDGES);
	impl::TestSidesCorrespondToElements (*mesh, CELLS, FACES);
}


namespace impl {
	static void TestFillGrobToIndexMap (SPMesh mesh, GrobSet grobSet)
	{
		GrobHashMap <index_t> indexMap;
		index_t baseInds[NUM_GROB_TYPES];

		if (!mesh->has (grobSet))
			return;

		FillGrobToIndexMap (indexMap, baseInds, *mesh, grobSet);

		for(auto gt : grobSet) {
			const index_t baseInd = baseInds [gt];
			index_t index = 0;

			for(auto grob : mesh->grobs (gt)) {
				const index_t providedIndex = indexMap[grob] - baseInd;
				COND_FAIL(providedIndex != index,
				          "Index lookup mismatch for grob type '" << GrobDesc(gt).name()
				          << "': Current index is " << index <<
				          " but GrobHashMap provides " << indexMap[grob]);
				++index;
			}
		}
	}
}// end of namespace impl

static void TestFillGrobToIndexMap (SPMesh mesh)
{
	impl::TestFillGrobToIndexMap (mesh, VERTICES);
	impl::TestFillGrobToIndexMap (mesh, EDGES);
	impl::TestFillGrobToIndexMap (mesh, FACES);
	impl::TestFillGrobToIndexMap (mesh, CELLS);
}


namespace impl {
	/** creates a grobToIndexMap for `sideGrobSet`, then iterates over the
	 * elements of next higher dimension and makes sure, that all of their sides
	 * are contained in grobToIndexMap.*/
	static void TestGrobToIndexMapSideLookup (SPMesh mesh, GrobSet sideGrobSet)
	{
		if (!mesh->has (sideGrobSet))
			return;

		GrobHashMap <index_t> indexMap;
		index_t baseInds[NUM_GROB_TYPES];

		FillGrobToIndexMap (indexMap, baseInds, *mesh, sideGrobSet);

		const index_t sideDim = sideGrobSet.dim();
		GrobSet grobSet (GrobSetTypeByDim (sideDim + 1));

		try {
			for(auto gt : grobSet) {
				for(auto grob : mesh->grobs (gt)) {
					for(index_t iside = 0; iside < grob.num_sides(sideDim); ++iside) {
						indexMap.at (grob.side (sideDim, iside));
					}
				}
			}
		}
		catch (std::out_of_range&) {
			FAIL ("side of grob could not be found in grobToIndexMap of all side grobs");
		}
	}
}// end of namespace impl

static void TestGrobToIndexMapSideLookup (SPMesh mesh)
{
	for(int dim = 0; dim < MAX_GROB_DIM; ++dim)
		impl::TestGrobToIndexMapSideLookup (mesh, GrobSetTypeByDim (dim));
}


namespace impl {
	static void TestGrobValences (SPMesh mesh,
	                              const index_t numGrobsWithValence1,
	                              const index_t numGrobsWithValence2)
	{
		GrobHashMap <index_t> valences;
		GrobSet nbrGrobSet = mesh->grob_set_type_of_highest_dim ();
		if (nbrGrobSet.dim() == 0)
			return;

		GrobSet grobSet = nbrGrobSet.side_set (nbrGrobSet.dim() - 1);

		ComputeGrobValences (valences, *mesh, grobSet, nbrGrobSet);

		vector <index_t> numSidesWithValenceN;
		for(auto gt : grobSet) {
			for(auto grob : mesh->grobs (gt)) {
				const index_t valence = valences.at (grob);
				if (valence >= numSidesWithValenceN.size())
					numSidesWithValenceN.resize (valence + 1, 0);
				++numSidesWithValenceN[valence];
			}
		}

		COND_FAIL (numSidesWithValenceN[0] > 0,
		           "There should be no faces with valence 0 in this grid");

		COND_FAIL (numSidesWithValenceN[1] != numGrobsWithValence1,
		           "There should be exactly " << numGrobsWithValence1
		           << " faces with valence 1 in this grid");

		COND_FAIL (numSidesWithValenceN[2] != numGrobsWithValence2,
		           "There should be exactly " << numGrobsWithValence2
		           << " faces with valence 2 in this grid");
	}
}// end of namespace impl

static void TestGrobValences ()
{
	vector<string> testMeshNames {"test_meshes/tris_and_quads.ugx",
								  "test_meshes/tet_refined.ugx"};

    vector<index_t> numGrobsWithValence1 {10, 16};
    vector<index_t> numGrobsWithValence2 {9, 8};

    int counter = 0;
	for(auto meshName : testMeshNames) {
		try {
			SPMesh mesh = CreateMeshFromFile (meshName);
			impl::TestGrobValences (mesh, numGrobsWithValence1[counter],
			                        numGrobsWithValence2[counter]);
			++counter;
			cout << "    ok: '" << meshName << "'" << endl;
		}
		catch (std::exception& e) {
			cout << "    fail: '" << meshName << "'" << endl;
			FAIL ("Failed for '" << meshName << "' with message: " << e.what());
		}
	}
}


namespace impl {
	static void TestFillHigherDimNeighborOffsetMap (SPMesh mesh,
	                                       GrobSet grobTypes,
	                                       GrobSet nbrGrobTypes)
	{
		GrobHashMap <index_t> valences;
		ComputeGrobValences (valences, *mesh, grobTypes, nbrGrobTypes);

		GrobHashMap <index_t> grobToIndexMap;
		index_t baseInds[NUM_GROB_TYPES];
		FillGrobToIndexMap (grobToIndexMap, baseInds, *mesh, grobTypes);

		vector <index_t> offsets;
		lume::impl::FillHigherDimNeighborOffsetMap (offsets, *mesh, grobTypes, nbrGrobTypes, grobToIndexMap);

		for(auto gt : grobTypes) {
			for(auto grob : mesh->grobs (gt)) {
				const index_t i = grobToIndexMap.at (grob);
				const index_t v = offsets.at(i+1) - offsets.at(i);

				COND_FAIL(v != valences.at (grob),
				          "Valence deduced from offset-map (" << v << ") does not correspond to "
				          " computed valence map (" << valences.at (grob) << ")");
			}
		}
	}
}// end of namespace impl

static void TestFillHigherDimNeighborOffsetMap (SPMesh mesh)
{
	impl::TestFillHigherDimNeighborOffsetMap (mesh, VERTICES, EDGES);
	impl::TestFillHigherDimNeighborOffsetMap (mesh, VERTICES, FACES);
	impl::TestFillHigherDimNeighborOffsetMap (mesh, VERTICES, CELLS);
	impl::TestFillHigherDimNeighborOffsetMap (mesh, EDGES, FACES);
	impl::TestFillHigherDimNeighborOffsetMap (mesh, EDGES, CELLS);
	impl::TestFillHigherDimNeighborOffsetMap (mesh, FACES, CELLS);
}


namespace impl {
	static void TestFillLowerDimNeighborOffsetMap (SPMesh mesh,
	                                       GrobSet grobTypes,
	                                       GrobSet nbrGrobTypes)
	{
		GrobHashMap <index_t> valences;
		ComputeGrobValences (valences, *mesh, grobTypes, nbrGrobTypes);

		vector <index_t> offsets;
		lume::impl::FillLowerDimNeighborOffsetMap (offsets, *mesh, grobTypes, nbrGrobTypes);

		index_t i = 0;
		for(auto gt : grobTypes) {
			for(auto grob : mesh->grobs (gt)) {
				const index_t v = offsets.at(i+1) - offsets.at(i);

				COND_FAIL(v != valences.at (grob),
				          grobTypes.name() << " to " << nbrGrobTypes.name()
				          << " valence deduced from offset-map (" << v
				          << ") does not correspond to computed valence map ("
				          << valences.at (grob) << ")");
				++i;
			}
		}
	}
}// end of namespace impl

static void TestFillLowerDimNeighborOffsetMap (SPMesh mesh)
{
	impl::TestFillLowerDimNeighborOffsetMap (mesh, EDGES, VERTICES);
	impl::TestFillLowerDimNeighborOffsetMap (mesh, FACES, VERTICES);
	impl::TestFillLowerDimNeighborOffsetMap (mesh, CELLS, VERTICES);
	impl::TestFillLowerDimNeighborOffsetMap (mesh, FACES, EDGES);
	impl::TestFillLowerDimNeighborOffsetMap (mesh, CELLS, EDGES);
	impl::TestFillLowerDimNeighborOffsetMap (mesh, CELLS, FACES);
}


namespace impl {
	static void TestNeighborsAreSides (SPMesh mesh, const Grob& grob, const NeighborGrobs& nbrs)
	{
		index_t counter = 0;
		for(auto nbrGrob : nbrs) {
			COND_FAIL (grob.find_side (nbrGrob) == NO_INDEX,
			           "Couldn't find " << counter << "'th " << " neighbor of type "
			           << nbrGrob.desc().name()
			           << " in the set of sides of a " << grob.desc().name());
			++counter;
		}
	}

	static void TestGrobIsSideOfNeighbors (SPMesh mesh, const Grob& grob, const NeighborGrobs& nbrs)
	{
		index_t counter = 0;
		for(auto nbrGrob : nbrs) {
			COND_FAIL (nbrGrob.find_side (grob) == NO_INDEX,
			           "Provided grob (" << grob.desc().name()
			           << ") is not a side of the " << counter << "'th " << " neighbor of type "
			           << nbrGrob.desc().name());
			++counter;
		}
	}

	static void TestNeighborhoods (SPMesh mesh, GrobSet grobTypes, GrobSet nbrGrobs)
	{
	//	valences are used to check whether the individual neighborhoods have
	//	correct size. Note that 'ComputeGrobValences' is tested in a different test.
		GrobHashMap <index_t> valences;
		ComputeGrobValences (valences, *mesh, grobTypes, nbrGrobs);

		Neighborhoods nbrhds (mesh, grobTypes, nbrGrobs);

		const index_t grobDim = grobTypes.dim();
		const index_t nbrGrobDim = nbrGrobs.dim();

		for(auto gt : grobTypes) {
			index_t counter = 0;
			for(auto grob : mesh->grobs (gt)) {
				const GrobIndex gi (gt, counter++);
				const NeighborGrobs nbrs = nbrhds.neighbor_grobs (gi);
				const index_t numNbrs = nbrs.size();

				COND_FAIL (numNbrs != valences [grob],
				           "Mismatch between the number of neighbors ("
				           << numNbrs << ") in a neighborhood of '"
				           << GrobName (gt) << "' and the valence of that '"
				           << GrobName (gt) << "' (" << valences [grob] << ")");

				if (nbrGrobDim < grobDim) {
					TestNeighborsAreSides (mesh, grob, nbrs);
				}
				else if (nbrGrobDim > grobDim) {
					TestGrobIsSideOfNeighbors (mesh, grob, nbrs);
				}

			}
		}
	}
}// end of namespace impl


static void TestNeighborhoods (SPMesh mesh)
{
	impl::TestNeighborhoods (mesh, VERTICES, EDGES);
	impl::TestNeighborhoods (mesh, VERTICES, FACES);
	impl::TestNeighborhoods (mesh, VERTICES, CELLS);
	impl::TestNeighborhoods (mesh, EDGES, VERTICES);
	impl::TestNeighborhoods (mesh, EDGES, FACES);
	impl::TestNeighborhoods (mesh, EDGES, CELLS);
	impl::TestNeighborhoods (mesh, FACES, VERTICES);
	impl::TestNeighborhoods (mesh, FACES, EDGES);
	impl::TestNeighborhoods (mesh, FACES, CELLS);
	impl::TestNeighborhoods (mesh, CELLS, VERTICES);
	impl::TestNeighborhoods (mesh, CELLS, EDGES);
	impl::TestNeighborhoods (mesh, CELLS, FACES);
}


namespace impl {
	static void TestNeighborValence (NeighborIndices nbrs,
	                                  const index_t expectedValence,
	                                  const std::string& linkGrobSetName)
	{
		const index_t numNbrs = nbrs.size();
		COND_FAIL (numNbrs != expectedValence,
		           "Wrong number of " << nbrs.neighborhoods()->neighbor_grob_set().name()
		           << " neighbors of a "
		           << GrobName (nbrs.center_grob_index().grobType)
		           << " linked by " << linkGrobSetName
		           << ". Expected " << expectedValence << " but got " << numNbrs);
	}

	static void TestFaceNeighbors (GrobSet linkGrobSet)
	{

	//	in this mesh all faces have the same valence.
	//	11 for connections via vertices, 2 for connections via edges
		auto mesh = CreateMeshFromFile ("test_meshes/circle_12.ugx");

		index_t expectedValence;
		if 		(linkGrobSet.dim() == 0)	expectedValence = 11;
		else if (linkGrobSet.dim() == 1)	expectedValence = 2;
		else								FAIL ("This test only supports linkGrobSets of dimension 0 or 1");

		Neighborhoods faceNbrs (mesh, FACES, Neighborhoods (mesh, linkGrobSet, FACES));

		for(auto grobType : GrobSet(FACES)) {
			const index_t numGrobs = mesh->num (grobType);
			for(index_t i = 0; i < numGrobs; ++i) {
				const GrobIndex gi (grobType, i);

			//	make sure that the valence of the current grob is fine
				const NeighborIndices nbrInds = faceNbrs.neighbor_indices (gi);
				impl::TestNeighborValence (nbrInds, expectedValence, linkGrobSet.name());

			//	also make sure that all neighbors of that grob have the same valence again
			//	(this is a special property of the considered mesh)
				for(auto nbrGrobIndex : nbrInds) {
					const NeighborIndices faceNbrsOfNbr = faceNbrs.neighbor_indices (nbrGrobIndex);
					impl::TestNeighborValence (faceNbrsOfNbr, expectedValence, linkGrobSet.name());
				}
			}
		}
	}
}// end of namespace impl

static void TestFaceNeighbors ()
{
	impl::TestFaceNeighbors (VERTICES);
	impl::TestFaceNeighbors (EDGES);
}


namespace impl {
	static void TestCreateBoundaryMesh (const string& meshName, const string& rimMeshName)
	{
		try {
			SPMesh mesh = CreateMeshFromFile (meshName);
			SPMesh bndMesh = CreateBoundaryMesh (mesh, CELLS);

			SPMesh rimMesh = CreateMeshFromFile (rimMeshName);
			COND_FAIL (bndMesh->num (TRI) != rimMesh->num (TRI),
			           "Mismatch between number of triangles in created boundary mesh ("
			           << bndMesh->num (TRI) << ") and loaded rim mesh (" << rimMesh->num (TRI) << ")");

			COND_FAIL (bndMesh->num (QUAD) != rimMesh->num (QUAD),
			           "Mismatch between number of quadrilaterals in created boundary mesh ("
			           << bndMesh->num (QUAD) << ") and loaded rim mesh (" << rimMesh->num (QUAD) << ")");

			cout << "    ok: '" << meshName << "' and '" << rimMeshName << "'" << endl;
		}
		catch (LumeError& e) {
			FAIL ("Failed for '" << meshName << "' and '" << rimMeshName << "' with message: " << e.what());
		}
	}
}// end of namespace impl


static void TestCreateBoundaryMesh ()
{
	impl::TestCreateBoundaryMesh ("test_meshes/tet_refined.ugx",
	                        	  "test_meshes/tet_refined_rim.ugx");

	impl::TestCreateBoundaryMesh ("test_meshes/elems_refined.ugx",
	                        	  "test_meshes/elems_refined_rim.ugx");

}


namespace impl {
	template <int minSyncLoopSize>
	void TestParallelFor (const size_t size)
	{
		vector <int> v;
		v.resize(size, 0);

		parallel_for <minSyncLoopSize> (0, v.size(), [](size_t i){v[i] = i;})

		for(size_t i = 0; i < v.size(); ++i) {
			COND_FAIL (i != v[i], "Vector entry doesn't expected value " << i
			           << ". Instead it contains: " << v[i]);
		}
	}

}// end of namespace impl
static void TestParallelFor ()
{
	TestParallelFor <1> (100);
	TestParallelFor <2> (100);
	TestParallelFor <10> (100);
	TestParallelFor <99> (100);
	TestParallelFor <100> (100);
	TestParallelFor <101> (100);
	TestParallelFor <200> (100);
}


}


namespace impl {
	template <class ... TArgs1, class ... TArgs2>
	static void RunTest (TestStats& testStats,
	                     void (*testFunction)(TArgs1...),
	                     const char* testFunctionName,
	                     TArgs2&&... args)
	{
		cout.flush ();

		cout << "->  " << testFunctionName << endl;
		try {
			testFunction (forward<TArgs2>(args)...);

			cout << "SUCCESS" << endl;
			testStats.success();
		}
		catch (std::exception& err) {
			testStats.fail();
			cout << "FAIL ('" << testFunctionName << "'). Message below:" << endl;
			cout << "     " << err.what () << endl << endl;
		}
		catch (...) {
			testStats.fail();
			cout << "FAIL ('" << testFunctionName << "'). UNKNOWN INTERNAL ERROR!" << endl << endl;
		}
	}

	static void RunTestOnFiles (void (*testFct)(const string& fileName),
	                            const vector<string>& fileNames)
	{
		for(auto fileName : fileNames) {
			try {
				testFct (fileName);
				cout << "    ok: '" << fileName << "'" << endl;
			}
			catch (std::exception& e) {
				cout << "    fail: '" << fileName << "'" << endl;
				FAIL ("Failed for '" << fileName << "' with message: " << e.what());
			}
		}
	}

	
	struct TestMesh {
		TestMesh (const std::string& _fileName) :
			fileName (_fileName),
			mesh (CreateMeshFromFile (fileName))
		{}

		std::string fileName;
		SPMesh		mesh;
	};

	static void RunTestOnMeshes (void (*testFct)(SPMesh mesh),
	                             const vector<TestMesh>& testMeshes)
	{
		for(auto testMesh : testMeshes) {
			try {
				testFct (testMesh.mesh);
				cout << "    ok: '" << testMesh.fileName << "'" << endl;
			}
			catch (std::exception& e) {
				cout << "    fail: '" << testMesh.fileName << "'" << endl;
				FAIL ("Failed for '" << testMesh.fileName << "' with message: " << e.what());
			}
		}
	}
}

#define RUN_TEST(testStats, testFunctionName) impl::RunTest (testStats, &testFunctionName, #testFunctionName);
#define RUN_TEST_ON_FILES(testStats, testFunctionName, files) impl::RunTest (testStats, &impl::RunTestOnFiles, #testFunctionName, &testFunctionName, files);
#define RUN_TEST_ON_MESHES(testStats, testFunctionName, meshes) impl::RunTest (testStats, &impl::RunTestOnMeshes, #testFunctionName, &testFunctionName, meshes);



bool RunTests ()
{
	using impl::TestMesh;
	
	int numTests = 0;
	int numFailed = 0;

	cout << "RUNNING TESTS" << endl;

	TestStats testStats;

	RUN_TEST(testStats, TestGrobDescs);

	vector<string> generalTestFiles {"test_meshes/quad.stl",
									 "test_meshes/box_with_spheres.ele",
								  	 "test_meshes/tris_and_quads.ugx",
								  	 "test_meshes/elems_refined_rim.ugx",
								  	 "test_meshes/tet_refined.ugx",
								  	 "test_meshes/elems_refined.ugx",
								  	 "test_meshes/circle_12.ugx"};

	vector<TestMesh> topologyTestMeshes {TestMesh ("test_meshes/tris_and_quads.ugx"),
								  		 TestMesh ("test_meshes/elems_refined_rim.ugx"),
								  		 TestMesh ("test_meshes/tet_refined.ugx"),
								  		 TestMesh ("test_meshes/elems_refined.ugx")};

	RUN_TEST_ON_FILES (testStats, TestCreateMeshFromFile, generalTestFiles);

	RUN_TEST_ON_MESHES(testStats, TestGrobArrays, topologyTestMeshes);
	RUN_TEST_ON_MESHES(testStats, TestGrobIterator, topologyTestMeshes);
	RUN_TEST_ON_MESHES(testStats, TestGrobIndexAccess, topologyTestMeshes);
	RUN_TEST_ON_MESHES(testStats, TestConsistentTopology, topologyTestMeshes);
	RUN_TEST_ON_MESHES(testStats, TestFillGrobToIndexMap, topologyTestMeshes);
	RUN_TEST_ON_MESHES(testStats, TestGrobToIndexMapSideLookup, topologyTestMeshes);
	RUN_TEST(testStats, TestGrobValences);
	RUN_TEST_ON_MESHES(testStats, TestFillLowerDimNeighborOffsetMap, topologyTestMeshes);
	RUN_TEST_ON_MESHES(testStats, TestFillHigherDimNeighborOffsetMap, topologyTestMeshes);
	RUN_TEST_ON_MESHES(testStats, TestNeighborhoods, topologyTestMeshes);
	RUN_TEST(testStats, TestFaceNeighbors);
	RUN_TEST(testStats, TestCreateBoundaryMesh);

	cout << endl << "TESTS DONE: " << testStats.num_tests() << " tests were run, "
		 << testStats.num_failed() << " failed." << endl << endl;

	return numFailed == 0;
}


}//	end of namespace tests
}//	end of namespace lume

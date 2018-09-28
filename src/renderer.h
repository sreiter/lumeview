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

#ifndef __H__Renderer
#define __H__Renderer

#include <map>
#include <memory>
#include <vector>

#include "gl_buffer.h"
#include "lume/mesh.h"
#include "shader.h"
#include "vec_math.h"
#include "shapes.h"
#include "view.h"

namespace lumeview {

enum ShadingPreset {
	NONE		= 0,
	FLAT		= 1,
	SMOOTH		= 2,
};

static const index_t NUM_SHADING_PRESETS = 3;


class Renderer {
public:
	Renderer ();
	Renderer (std::string shaderPath);
	Renderer (const Renderer&) = delete;

	void clear ();
	
	void add_stage (std::string name,
	                lume::SPMesh mesh,
	                const lume::GrobSet grobSet,
	                ShadingPreset shading);

	size_t num_stages () const	{return m_stages.size();}
	size_t last_stage () const	{return num_stages() ? num_stages() - 1 : 0;}
	
	/// sets the color of the specified stage. If no stage is specified, the last one is used.
	/** stageInd may be negative. In that case, -1 is the last, -2 the second to last, etc.*/
	void stage_set_color (const glm::vec4& color, int stageInd = -1);

	///	returns min (x) and max (y) z clip distances required to show all polygons.
	glm::vec2 estimate_z_clip_dists (const View& view) const;

	void render (const View& view);

	void do_imgui (bool* pOpened = NULL);

private:
	struct Stage {
		Stage ()	{glGenVertexArrays (1, &vao);}
		Stage (const Stage&) = delete;
		Stage (Stage&& s) :
			name (std::move (s.name)),
			mesh (std::move (s.mesh)),
			shadingPreset (std::move (s.shadingPreset)),
			color (std::move (s.color)),
			zfacNear (std::move (s.zfacNear)),
			zfacFar (std::move (s.zfacFar)),
			vao (std::exchange (s.vao, 0)),
			coordBuf (std::move (s.coordBuf)),
			normBuf (std::move (s.normBuf)),
			indBuf (std::move (s.indBuf)),
			primType (std::move (s.primType)),
			numInds (std::move (s.numInds)),
			grobSet (std::move (s.grobSet)),
			bndSphere (std::move (s.bndSphere))
		{}

		~Stage ()	{if (vao) glDeleteVertexArrays (1, &vao);}

		std::string 				name;
		lume::SPMesh				mesh;
		ShadingPreset				shadingPreset;
		glm::vec4					color;
		float						zfacNear;
		float						zfacFar;

		uint 						vao;
		std::shared_ptr <GLBuffer>	coordBuf;
		std::shared_ptr <GLBuffer>	normBuf;
		std::shared_ptr <GLBuffer>	indBuf;
		GLenum						primType;
		GLsizei						numInds;
		lume::GrobSet				grobSet;
		Sphere						bndSphere;
	};

	
	Stage& stage (int stageInd);
	const Stage& stage (int stageInd) const;
	Shader get_shader (const lume::GrobSet grobSet, ShadingPreset shading);
	void prepare_buffers ();


	std::vector <Stage> m_stages;
	Shader				m_shaders[lume::NUM_GROB_TYPES + 1][NUM_SHADING_PRESETS];
	std::string			m_shaderPath;
};

}// end of namespace lumeview

#endif	//__H__Renderer

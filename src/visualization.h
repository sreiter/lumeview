#ifndef __H__visualization
#define __H__visualization

#include <map>
#include <memory>
#include <vector>

#include "gl_buffer.h"
#include "mesh.h"
#include "shader.h"
#include "vec_math.h"
#include "shapes.h"
#include "view.h"

namespace slimesh {

enum ShadingPreset {
	NONE		= 0,
	FLAT		= 1,
	SMOOTH		= 2,
};

static const index_t NUM_SHADING_PRESETS = 3;


class Visualization {
public:
	Visualization();
	Visualization(std::string shaderPath);
	Visualization (const Visualization&) = delete;

	void add_stage (std::string name,
	                std::shared_ptr <Mesh> mesh,
	                grob_t grobType,
	                ShadingPreset shading);

	///	returns min (x) and max (y) z clip distances required to show all polygons.
	glm::vec2 estimate_z_clip_dists (const View& view) const;

	void render (const View& view);

	void do_imgui (bool* pOpened = NULL);

private:
	Shader get_shader (grob_t grobType, ShadingPreset shading);

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
			grobType (std::move (s.grobType)),
			bndSphere (std::move (s.bndSphere))
		{}

		~Stage ()	{if (vao) glDeleteVertexArrays (1, &vao);}

		std::string 				name;
		std::shared_ptr <Mesh>		mesh;
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
		grob_t						grobType;
		Sphere						bndSphere;
	};

	std::vector <Stage> m_stages;
	Shader				m_shaders[NUM_GROB_TYPES + 1][NUM_SHADING_PRESETS];
	std::string			m_shaderPath;
};

}// end of namespace slimesh

#endif	//__H__visualization

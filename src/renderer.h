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
	/** stageInd may be negative. In that case, -1 is the last, -2 the second to last, etc.
	 * \note	This value is ignored if a color pointer has been set through
	 * 'stage_set_color_ptr'*/
	void stage_set_color (const glm::vec4& color, int stageInd = -1);

	/// sets the color of the specified stage. If no stage is specified, the last one is used.
	/** stageInd may be negative. In that case, -1 is the last, -2 the second to last, etc.
	 *
	 * \param colorPtr	pointer to an array of 4 floats, representing a color with
	 *					values between 0 and 1 in the format RGBA.
	 *
	 * \note	the specified pointer is stored by the renderer and color values
	 *			are read from that memory address during rendering. The memory location
	 *			thus has to hold valid color values until the color-pointer has been
	 *			unset. Changes to the color outside of the renderer are thus reflected by
	 *			the renderer.
	 *
	 * \note	If a color will not change, or an R-Value shall be passed as color,
	 *			please use the method 'stage_set_color' instead.
	 *
	 * \warning	Make sure to unset the color pointer when the underlying memory
	 *			address no longer points to a valid color.
	 */
	// void stage_set_color_ptr (const float* colorPtr, int stageInd = -1);

	///	unsets the previously set color_ptr
	/** The normal stage color (set through 'Stage_set_color') will be used
	 * for rendering afterwards.*/
	// void stage_unset_color_ptr (int stageInd = -1);

	///	returns min (x) and max (y) z clip distances required to show all polygons.
	glm::vec2 estimate_z_clip_dists (const View& view) const;

	void render (const View& view);

	void do_imgui (bool* pOpened = NULL);

private:
	struct Stage {
		Stage ()	{glGenVertexArrays (1, &vao);}
		// Stage () : colorPtr (nullptr)	{glGenVertexArrays (1, &vao);}
		Stage (const Stage&) = delete;
		Stage (Stage&& s) :
			name (std::move (s.name)),
			mesh (std::move (s.mesh)),
			shadingPreset (std::move (s.shadingPreset)),
			color (std::move (s.color)),
			// colorPtr (std::move (s.colorPtr)),
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
		// const float*				colorPtr;
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

#ifndef __H__view
#define __H__view

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "fwds.h"

namespace slimesh {

class View {
public:
    View ();
    
    void apply (const Shader& shader) const;

    void set_viewport (const glm::ivec4& vp);
    const glm::ivec4& viewport () const;

    glm::vec2 aspect_ratio () const;

    void set_view_matrix (const glm::mat4& view);
    const glm::mat4& view_matrix () const;

    glm::mat4 projection () const;

    glm::vec3 unproject (const glm::vec3& c) const;

    float depth_at_screen_coord (const glm::vec2& c) const;

private:
    glm::mat4   m_viewMat;
    glm::ivec4  m_viewport;
};

}// end of namespace slimesh

#endif  //__H__view

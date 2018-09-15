#ifndef __H__view
#define __H__view

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "fwds.h"
#include "camera.h"

namespace lumeview {

class View {
public:
    View ();
    
    void apply (const Shader& shader) const;

    void                set_viewport (const glm::ivec4& vp);
    const glm::ivec4&   viewport () const;

    void            set_camera (const Camera& camera);
    void            set_camera (Camera&& camera);

    Camera&         camera ();
    const Camera&   camera () const;

    glm::vec2 aspect_ratio () const;

    glm::mat4 view_matrix () const;

    glm::mat4 projection_matrix () const;

    glm::vec3 unproject (const glm::vec3& c) const;

    glm::vec3 project (const glm::vec3& c) const;

    float depth_at_screen_coord (const glm::vec2& c) const;

    void set_z_clip_dists (const glm::vec2& c);
    glm::vec2 z_clip_dists () const;

private:
    Camera      m_camera;
    glm::ivec4  m_viewport;
    glm::vec2   m_zClipDists;
};

}// end of namespace lumeview

#endif  //__H__view

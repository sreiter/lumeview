
#include <glad/glad.h>  // include before other OpenGL related includes
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "cond_throw.h"
#include "view.h"
#include "shader.h"

using namespace std;

namespace slimesh {

View::View () :
    m_viewport (0, 0, 1, 1),
    m_zClipDists (1.e-3, 1.e2)
{}

void View::apply (const Shader& shader) const
{
    const glm::ivec4& vp = m_viewport;
    glViewport (vp.x, vp.y, vp.z, vp.w);
    shader.set_uniform ("view", view_matrix());
    shader.set_uniform ("projection", projection_matrix());
}

void View::set_viewport (const glm::ivec4& vp)
{
    COND_THROW((vp.x == vp.z) || (vp.y == vp.w),
               "Bad vp spefified: " << vp);
    m_viewport = vp;
}

const glm::ivec4& View::viewport () const
{
    return m_viewport;
}

void View::set_camera (const Camera& camera)
{
    m_camera = camera;
}

void View::set_camera (Camera&& camera)
{
    m_camera = std::move (camera);
}

Camera& View::camera ()
{
    return m_camera;
}

const Camera& View::camera () const
{
    return m_camera;
}


glm::vec2 View::aspect_ratio () const
{
    const glm::vec2 size (m_viewport.z - m_viewport.x,
                          m_viewport.w - m_viewport.y);

    if (size.x == 0 || size.y == 0)
        return glm::vec2 (1.f, 1.f);

    if (size.x > size.y) {
        return glm::vec2 (size.x / size.y, 1.f);
    }
    else {
        return glm::vec2 (1.f, size.y / size.x);
    }
}

glm::mat4 View::view_matrix () const
{
   return m_camera.view_matrix();
}

glm::mat4 View::projection_matrix () const
{
    const auto ar = aspect_ratio ();
    if (ar.x == 0 || ar.y == 0) {
        return glm::mat4 (1.f);
    }
    else {
        return glm::perspective(glm::radians(45.0f), ar.x / ar.y, m_zClipDists.x, m_zClipDists.y);
    }
}

glm::vec3 View::unproject (const glm::vec3& c) const
{
    const glm::ivec4& vp = m_viewport;
    return glm::unProject (glm::vec3(c.x, (vp.w - vp.y - c.y), c.z),
                             view_matrix(),
                             projection_matrix(),
                             m_viewport);
}

float View::depth_at_screen_coord (const glm::vec2& c) const
{
    const glm::ivec4& vp = m_viewport;
    GLfloat depthVal;
    glReadPixels ((int)c.x, (int)(vp.w - vp.y - c.y), 1, 1,
                  GL_DEPTH_COMPONENT, GL_FLOAT, &depthVal);
    return depthVal;
}

void View::set_z_clip_dists (const glm::vec2& c)
{
    m_zClipDists = c;
}

glm::vec2 View::z_clip_dists () const
{
    return m_zClipDists;
}


}// end of namespace slimesh

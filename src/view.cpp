
#include <glad/glad.h>  // include before other OpenGL related includes
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "cond_throw.h"
#include "view.h"

View::View () :
    m_viewMat (1.f),
    m_viewport (0, 0, 1, 1)
{}

void View::apply (unsigned int viewLoc) const
{
    const glm::ivec4& vp = m_viewport;
    glViewport (vp.x, vp.y, vp.z, vp.w);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMat));
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

glm::vec2 View::aspect_ratio () const
{
    const glm::vec2 size (m_viewport.z - m_viewport.x,
                          m_viewport.w - m_viewport.y);

    if (size.x > size.y) {
        float s = size.x / size.y;
        return glm::vec2 (s, 1.f);
    }
    else {
        float s = size.y / size.x;
        return glm::vec2 (1.f, s);
    }
}

void View::set_view_matrix (const glm::mat4& view)
{
   m_viewMat = view;
}

const glm::mat4& View::view_matrix () const
{
   return m_viewMat;
}

glm::mat4 View::projection () const
{
   return glm::mat4 (1.f);
}

glm::vec3 View::unproject (const glm::vec3& c) const
{
    const glm::ivec4& vp = m_viewport;
    return glm::unProject (glm::vec3(c.x, (vp.w - vp.y - c.y), c.z),
                             m_viewMat,
                             projection(),
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
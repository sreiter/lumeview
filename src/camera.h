#ifndef __H__CAMERA_
#define __H__CAMERA_

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>


namespace lumeview {

class Camera {
  public:
    Camera ();

    void set_scale (const glm::vec3& scale);
    void set_rotation (const glm::quat& rot);
    void set_translation (const glm::vec3& trans);

    const glm::vec3& scale () const;
    const glm::quat& rotation () const;
    const glm::vec3& translation () const;

    void scale (const glm::vec3& scaleFac);
    void rotate (const glm::quat& rot);
    void translate (const glm::vec3& offset);

    glm::vec3 right () const;
    glm::vec3 up () const;
    glm::vec3 forward () const;

    glm::vec3 from () const;
    glm::vec3 to () const;

    glm::mat4 view_matrix () const;

  private:
    glm::vec3 m_scale;
    glm::quat m_rot;
    glm::vec3 m_trans;
};

}// end of namespace lumeview

#endif  //__H__CAMERA_

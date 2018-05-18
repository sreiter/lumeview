#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera () :
	m_scale (1.f, 1.f, 1.f),
	m_rot (1.f, 0.f, 0.f, 0.f),
	m_trans (0.f, 0.f, 0.f)
{
}

void Camera::
set_scale (const glm::vec3& scale)
{
	m_scale = scale;
}


void Camera::
set_rotation (const glm::quat& rot)
{
	m_rot = rot;
}


void Camera::
set_translation (const glm::vec3& trans)
{
	m_trans = trans;
}

const glm::vec3& Camera::
scale () const
{
	return m_scale;
}


const glm::quat& Camera::
rotation () const
{
	return m_rot;
}


const glm::vec3& Camera::
translation () const
{
	return m_trans;
}



void Camera::
scale (const glm::vec3& scaleFac)
{
	m_scale *= scaleFac;
}


void Camera::
rotate (const glm::quat& rot)
{
	m_rot = rot * m_rot;
}


void Camera::
translate (const glm::vec3& offset)
{
	m_trans += offset;
}



glm::vec3 Camera::
right () const
{
	return axis (m_rot * glm::quat (0, 1, 0, 0) * conjugate(m_rot));
}


glm::vec3 Camera::
up () const
{
	return axis (m_rot * glm::quat (0, 0, 1, 0) * conjugate(m_rot));
}


glm::vec3 Camera::
forward () const
{
	return axis (m_rot * glm::quat (0, 0, 0, -1) * conjugate(m_rot));
}



glm::mat4 Camera::
view_matrix () const
{
	glm::vec3 r = right(), u = up(), f = forward();

	// glm::mat4 A (r.x,  r.y,  r.z,  0,
	// 			 u.x,  u.y,  u.z,  0,
	// 			 f.x,  f.y,  f.z,  0,
	// 			 0,    0,    0,    1.f);

	glm::mat4 A (r.x,  u.x,  f.x,  0,
				 r.y,  u.y,  f.y,  0,
				 r.z,  u.z,  f.z,  0,
				 0,    0,    0,    1.f);

	A = glm::scale (A, m_scale);

	return glm::translate (A, -m_trans);
}

#include "arc_ball.h"
#include <glm/gtc/quaternion.hpp>

namespace slimesh {

ArcBall::ArcBall() :
	m_rotQuat (1.f, 0.f, 0.f, 0.f), // w = 1, rest 0
	m_frame (1.f, 1.f),
	m_offset (0.f, 0.f),
	m_radius (1.f),
	m_dragging (false)
{
}

void ArcBall::
set_frame(const glm::vec2& frame)
{
	m_frame = frame;
}

void ArcBall::
set_radius (const float radius)
{
	m_radius = radius;
}

void ArcBall::
set_offset (const glm::vec2& offset)
{
	m_offset = offset;
}


const glm::vec2& ArcBall::
frame () const
{
	return m_frame;
}

float ArcBall::
radius () const
{
	return m_radius;
}

const glm::vec2& ArcBall::
offset () const
{
	return m_offset;
}

const glm::quat& ArcBall::rotation_quaternion() const
{
	return m_rotQuat;
}

void ArcBall::set_rotation_quaternion(const glm::quat& q)
{
	end_drag();
	m_rotQuat = q;
}

void ArcBall::begin_drag(const glm::vec2& c)
{
	if(m_dragging)
		end_drag();

	m_beginDragQuat = m_rotQuat;
	m_beginDragPos = get_ball_point_from_frame_coords(glm::vec2(c.x, m_frame.y - c.y));

	m_dragging = true;
}

void ArcBall::drag_to(const glm::vec2& c)
{
	if(m_dragging)
	{
		glm::vec3 v = get_ball_point_from_frame_coords(glm::vec2(c.x, m_frame.y - c.y));
		m_rotQuat = m_beginDragQuat * get_quat_from_ball_points(m_beginDragPos, v);
	}
}

void ArcBall::end_drag()
{
	m_dragging = false;
}

glm::vec3 ArcBall::get_ball_point_from_frame_coords(const glm::vec2& c) const
{
    vec3 p;
    p.x   = (c.x - m_offset.x - m_frame.x / 2.f) / (m_radius * m_frame.x / 2.f);
    p.y   = (c.y - m_offset.y - m_frame.y / 2.f) / (m_radius * m_frame.y / 2.f);
    p.z   = 0.0f;

    const float mag = p.x * p.x + p.y * p.y;

    if (mag > 1.0f) {
        float scale = 1.0f / sqrtf (mag);
        p.x *= scale;
        p.y *= scale;
    }
    else
        p.z = sqrtf (1.0f - mag);

    return p;
}

glm::quat ArcBall::get_quat_from_ball_points(const glm::vec3& from, const glm::vec3& to) const
{
    float d = glm::dot (from, to);
    glm::vec3 p = glm::cross (to, from);

	glm::quat quat(d, p.x, p.y, p.z);
	return glm::normalize(quat);
}

}// end of namespace slimesh
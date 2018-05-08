
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

class ArcBall
{
  public:
	ArcBall();
	void set_frame (float width, float height, float radius = 0.9f, float offsetX = 0, float offsetY = 0);
	void set_frame(const glm::vec2& size, float radius = 0.9f, const glm::vec2& offset = glm::vec2(0, 0));

	inline const glm::mat4 rotation_matrix () const	{return glm::mat4_cast(m_rotQuat);}
	const glm::quat& rotation_quaternion () const;
	void set_rotation_quaternion (const glm::quat& q);

	void begin_drag (const glm::vec2& c);
	void drag_to (const glm::vec2& c);
	void end_drag ();

	bool dragging () const {return m_dragging;}
	
  private:
  	using vec2 = glm::vec2;
  	using vec3 = glm::vec3;
  	using mat4 = glm::mat4;
  	using quat = glm::quat;

	vec3 get_ball_point_from_frame_coords (const vec2& c) const;
	quat get_quat_from_ball_points (const vec3& from, const vec3& to) const;

	quat	m_rotQuat;

	quat	m_beginDragQuat;
	vec3	m_beginDragPos;

	vec2	m_frame;
	vec2	m_offset;

	vec2	m_lastMousePos;

	float	m_radius;

	bool	m_dragging;
};
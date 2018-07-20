#ifndef __H__slimesh__gl_buffer
#define __H__slimesh__gl_buffer

#include <glad/glad.h>	// include before other OpenGL related includes
#include "config.h"
#include "cond_throw.h"

namespace slimesh {

class GLBuffer {
public:
	/**	generates an OpenGL buffer object.
	 *
	 * \param type	Either 'GL_ARRAY_BUFFER' or 'GL_ELEMENT_ARRAY_BUFFER'
	 *
	 * \param memHint Either 'GL_STATIC_DRAW' (default) or 'GL_DYNAMIC_DRAW'
	 */
	GLBuffer (GLenum type, GLenum memHint = GL_STATIC_DRAW) :
		m_id (0),
		m_size (0),
		m_capacity (0),
		m_type (type),
		m_memHint (memHint)
	{
		glGenBuffers (1, &m_id);
	}

	~GLBuffer ()
	{
		glDeleteBuffers (1, &m_id);
	}

	void bind () {
		glBindBuffer (m_type, m_id);
	}

	index_t size () const		{return m_size;}
	index_t capacity () const	{return m_capacity;}

	/// makes sure that a buffer of the specified size is allocated.
	/** \note this also binds the buffer 
	 * \warning this may clear the buffer contents!
	 */
	void set_size (const uint size) {
		m_size = size;
		if (size > m_capacity){
			bind ();
			glBufferData (m_type, size, NULL, m_memHint);
			m_capacity = size;
		}
	}

	/// allocates a new buffer storage if necessary and transfers the specified data
	/** \note this also binds the buffer */
	void set_data (const void* data, const uint size) {
		m_size = size;
		if (size > m_capacity) {
			bind ();
			glBufferData (m_type, size, data, m_memHint);
			m_capacity = size;
		}
		else
			set_sub_data (0, data, size);
	}

	/// transfers the specified data to an existing buffer region
	/** \note this also binds the buffer */
	void set_sub_data (const uint offset, const void* data, const uint size) {
		COND_THROW (offset + size > m_size,
		            "GLBuffer::set_sub_data: Specified buffer region expands over buffer boundary");
		bind ();
		glBufferSubData (m_type, offset, size, data);
	}

private:
	uint	m_id;
	uint	m_size;
	uint	m_capacity;
	GLenum	m_type;
	GLenum	m_memHint;
};

}// end of namespace slimesh

#endif	//__H__slimesh__gl_buffer

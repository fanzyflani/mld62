#include "common.h"
#include "GL/intern.h"

GLvoid glBegin(GLenum mode) // p24 2.6.1
{
	// Make sure we aren't already in a block
	if(gl_begin_mode != 0)
	{
		gl_internal_set_error(GL_INVALID_OPERATION);
		return;
	}

	// Eliminate any bad enums
	switch(mode)
	{
		case GL_POINTS:
		case GL_LINES:
		case GL_LINE_STRIP:
		case GL_LINE_LOOP:
		case GL_TRIANGLES:
		case GL_TRIANGLE_STRIP:
		case GL_TRIANGLE_FAN:
		case GL_QUADS:
		case GL_QUAD_STRIP:
		case GL_POLYGON:
			break;
		default:
			gl_internal_set_error(GL_INVALID_ENUM);
			return;
	}

	// TODO: make GL_POLYGON a special case once we get proper split logic in place
	if(mode == GL_POLYGON)
		mode = GL_TRIANGLE_FAN;

	// Prepare structure
	gl_begin_mode = mode;
	gl_begin_idx = 0;

	// TODO: actually append stuff to DMA
}

GLvoid glEnd(GLvoid) // p24 2.6.1
{
	// Make sure we are in a block
	if(gl_begin_mode == 0)
	{
		gl_internal_set_error(GL_INVALID_OPERATION);
		return;
	}

	// STAHP
	gl_begin_mode = 0;
	gl_begin_idx = 0;
}

GLvoid glColor3ub(GLubyte r, GLubyte g, GLubyte b)
{
	gl_begin_gourcount = -1;
	((uint8_t *)&gl_begin_colcur)[0] = r;
	((uint8_t *)&gl_begin_colcur)[1] = g;
	((uint8_t *)&gl_begin_colcur)[2] = b;
}

GLvoid glTexCoord2i(GLint s, GLint t)
{
	((uint8_t *)&gl_begin_texcur)[0] = (uint8_t)s;
	((uint8_t *)&gl_begin_texcur)[1] = (uint8_t)t;
}

GLvoid glVertex3x(GLfixed x, GLfixed y, GLfixed z)
{
	// Make sure we are in a block
	if(gl_begin_mode == 0)
	{
		gl_internal_set_error(GL_INVALID_OPERATION);
		return;
	}

	uint32_t sxy = (((GLuint)(GLushort)y)<<16)|((GLuint)(GLushort)x);
	uint32_t sz = (uint32_t)(GLint)(GLshort)z;

	// Push new vertex
	gl_begin_vtxbuf[gl_begin_idx][0] = sxy;
	gl_begin_vtxbuf[gl_begin_idx][1] = sz;
	gl_begin_colbuf[gl_begin_idx] = gl_begin_colcur;
	gl_begin_texbuf[gl_begin_idx] = gl_begin_texcur;

	// Advance
	gl_begin_idx++;

	// Separate the men from the b- erm, the different render modes
	switch (gl_begin_mode)
	{
		case GL_TRIANGLES:
			// Don't gouraud-shade if we changed colour just before the first point
			if(gl_begin_gourcount == -1 && gl_begin_idx == 1)
				gl_begin_gourcount = 0;

			if(gl_begin_idx == 3)
			{
				gl_begin_idx = 0;
				gl_internal_push_triangle(0, 1, 2);
				gl_begin_gourcount = 0;
			}

			break;

		default:
			// TODO: implement some other things
			// in the mean time, clamp for safety
			gl_begin_idx &= 3;
			break;
	}
}


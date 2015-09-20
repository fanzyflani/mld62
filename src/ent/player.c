#include "common.h"

GLuint player_dl_base = (GLuint)-1;

static void ent_player_draw_imm_base(void)
{
	// Build twisted tri prism body
	glColor3ub(0x7F, 0x7F, 0x7F);

	int x0 = (fixsin((0x10000*2)/3)*90)>>16;
	int x1 = (fixsin((0x10000*1)/3)*90)>>16;
	int x2 = (fixsin((0x10000*0)/3)*90)>>16;
	int z0 = (fixcos((0x10000*2)/3)*90)>>16;
	int z1 = (fixcos((0x10000*1)/3)*90)>>16;
	int z2 = (fixcos((0x10000*0)/3)*90)>>16;

	glBegin(GL_TRIANGLES);
		// Top
		glTexCoord2i( 0,  0); glVertex3x( x0, -90,  z0);
		glTexCoord2i(21,  0); glVertex3x( x1, -90,  z1);
		glTexCoord2i( 0, 63); glVertex3x(-x2,  90, -z2);

		glTexCoord2i(21,  0); glVertex3x( x1, -90,  z1);
		glTexCoord2i(42,  0); glVertex3x( x2, -90,  z2);
		glTexCoord2i(21, 63); glVertex3x(-x0,  90, -z0);

		glTexCoord2i(42,  0); glVertex3x( x2, -90,  z2);
		glTexCoord2i(64,  0); glVertex3x( x0, -90,  z0);
		glTexCoord2i(42, 63); glVertex3x(-x1,  90, -z1);

		glTexCoord2i( 0,  0); glVertex3x( x0, -90,  z0);
		glTexCoord2i(42,  0); glVertex3x( x2, -90,  z2);
		glTexCoord2i(21,  0); glVertex3x( x1, -90,  z1);

		// Bottom
		glTexCoord2i(42, 63); glVertex3x(-x1,  90, -z1);
		glTexCoord2i(21, 63); glVertex3x(-x0,  90, -z0);
		glTexCoord2i(42,  0); glVertex3x( x2, -90,  z2);

		glTexCoord2i(64, 63); glVertex3x(-x2,  90, -z2);
		glTexCoord2i(42, 63); glVertex3x(-x1,  90, -z1);
		glTexCoord2i(64,  0); glVertex3x( x0, -90,  z0);

		glTexCoord2i(21, 63); glVertex3x(-x0,  90, -z0);
		glTexCoord2i( 0, 63); glVertex3x(-x2,  90, -z2);
		glTexCoord2i(21,  0); glVertex3x( x1, -90,  z1);

		glTexCoord2i(21, 63); glVertex3x(-x0,  90, -z0);
		glTexCoord2i(42, 63); glVertex3x(-x1,  90, -z1);
		glTexCoord2i(64, 63); glVertex3x(-x2,  90, -z2);
	glEnd();
}

void ent_player_draw(void)
{
	if(player_dl_base == (GLuint)-1)
	{
		player_dl_base = glGenLists(1);
		glNewList(player_dl_base, GL_COMPILE);
		ent_player_draw_imm_base();
		glEndList();
	}

	glCallList(player_dl_base);
}

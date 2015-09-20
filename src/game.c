#include "common.h"

extern volatile int got_vblank;
extern int waiting_for_vblank;

fixed tri_ang = 0;
GLuint tri_tex0 = (GLuint)-1;
GLuint pal_tex0 = (GLuint)-1;
int tmr_dmaend = 0;

void update_frame(void)
{
	static volatile int lag;
	int x, y, i;

	int tmr_frame = TMR_n_COUNT(1);

	// Finish drawing
	glFinish();

	int tmr_dmafin = TMR_n_COUNT(1);

	// Flip pages
	gpu_display_start(0, screen_buffer);
	screen_buffer = (screen_buffer == 0 ? 240 : 0);
	gpu_draw_range(0, screen_buffer, 319, 239 + screen_buffer);
	gpu_draw_offset(0 + 160, screen_buffer + 120);

	// Enable things
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// Clear screen
	glClearColorx(0x0000, 0x1D00, 0x1D00, 0x0000);
	glClear(1);

	// Set up camera matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatex(0, 0, 0x100);
	glRotatex(-tri_ang/6, 0x1000, 0, 0);
	glRotatex(tri_ang, 0, 0x1000, 0);
	//glRotatex(0, 0, 0x10000, 0);

	// Draw test meshes
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tri_tex0);
	ent_player_draw();
	glDisable(GL_TEXTURE_2D);

	tri_ang += FM_PI*2/180/2;

	// Draw string
	//gpu_send_control_gp1(0x01000000);
	static uint8_t update_str_buf[64]; // don't want this using up scratch space
	//sprintf(update_str_buf, "mtest=%p", malloc(5000));
	//screen_print(16, 16+8*2, 0x007F7F7F, update_str_buf);

	// Read joypad
	pad_id   =  pad_id_now  ;
	pad_data = ~pad_data_now;
	joy_poll();

	/*
	if(((pad_data&~pad_old_data) & PAD_RIGHT) != 0)
	{
		if(s3mplayer.cord > s3mplayer.mod->ord_num-2)
			s3mplayer.cord = s3mplayer.mod->ord_num-2;
		s3mplayer.crow=64;
		s3mplayer.ctick = s3mplayer.speed;
	}

	if(((pad_data&~pad_old_data) & PAD_LEFT) != 0)
	{
		s3mplayer.cord -= 2;
		if(s3mplayer.cord < -1)
			s3mplayer.cord = -1;
		s3mplayer.crow=64;
		s3mplayer.ctick = s3mplayer.speed;
	}
	*/

	pad_old_data = pad_data;

	const char *pad_id_str = "Unknown";
	switch(pad_id)
	{
		case 0x5A12:
			// I've heard this one's rather buggy
			pad_id_str = "Mouse - FUCK YOU";
			break;
		case 0x5A41:
			pad_id_str = "Digital Pad";
			break;
		case 0x5A53:
			pad_id_str = "Analogue Stick";
			break;
		case 0x5A73:
			pad_id_str = "Analogue Pad";
			break;
		case 0xFFFF:
			pad_id_str = "NOTHING";
			break;
	}

	sprintf(update_str_buf, "joypad=%04X (%04X: %s)", (unsigned)pad_data, (unsigned)pad_id, pad_id_str);
	screen_print(16, 16+8*3, 0x007F7F7F, update_str_buf);
	sprintf(update_str_buf, "glGetError() = %X", (unsigned)glGetError());
	screen_print(16, 16+8*5, 0x007F7F7F, update_str_buf);
	sprintf(update_str_buf, "sbrk(0) = %p", (void *)sbrk(0));
	screen_print(16, 16+8*6, 0x007F7F7F, update_str_buf);
	sprintf(update_str_buf, "dma end = %i", (int)tmr_dmaend);
	screen_print(16, 16+8*7, 0x007F7F7F, update_str_buf);
	sprintf(update_str_buf, "fra beg = %i", (int)tmr_frame);
	screen_print(16, 16+8*8, 0x007F7F7F, update_str_buf);
	sprintf(update_str_buf, "dma fin = %i", (int)tmr_dmafin);
	screen_print(16, 16+8*9, 0x007F7F7F, update_str_buf);
	sprintf(update_str_buf, "dma beg = %i", (int)TMR_n_COUNT(1));
	screen_print(16, 16+8*10, 0x007F7F7F, update_str_buf);
	sprintf(update_str_buf, "fra lag = %i", (int)(got_vblank - waiting_for_vblank));
	screen_print(16, 16+8*11, 0x007F7F7F, update_str_buf);

	// Flush DMA
	glFlush();
	tmr_dmaend = TMR_n_COUNT(1);

	/*
	// Get actual render time
	//sprintf(update_str_buf, "malloc test = %p", NULL);//malloc(5));
	//screen_print(16, 16+8*11, 0x007F7F7F, update_str_buf);
	//sprintf(update_str_buf, "malloc test = %p", malloc(102));
	//screen_print(16, 16+8*12, 0x007F7F7F, update_str_buf);
	glFinish();
	*/

	//while((GP1 & 0x10000000) == 0) {}
}

void game_init_assets(void)
{
	int x, y;

	// Allocate palette
	{
		glGenTextures(1, &pal_tex0);
		glBindTexture(GL_TEXTURE_2D, pal_tex0);

		static uint16_t palette[16*8];
		for(x = 0; x < 16; x++)
			palette[x] = (2*x+1)*0x421;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1,
			16, 8, 0,
			GL_RGBA, GL_UNSIGNED_SHORT, palette);

	}

	// Allocate texture
	{
		glGenTextures(1, &tri_tex0);
		glBindTexture(GL_TEXTURE_2D, tri_tex0);

		// Generate XOR pattern
		// (declare static - we don't have 32*32*2 bytes of scratchpad)
		static uint8_t xor_pattern[64*32];

		memset(xor_pattern, 0, 64*32);

		for(y = 0; y < 64; y++)
		for(x = 0; x < 64; x++)
		{
			int v = x^y;
			v = (v>>1) + (v&1);
			if((v&16)!=0) v = (~v)&15;
			xor_pattern[y*32 + (x>>1)] |= v<<((x&1)*4);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX,
			64, 64, 0,
			GL_COLOR_INDEX4_EXT, GL_UNSIGNED_NYBBLE_PSX, xor_pattern);
		glBindClutPSX(pal_tex0, 0, 0);
	}


}


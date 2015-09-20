#include "common.h"

#define TARGET_PSX
#define F3M_FREQ 44100
#define F3M_BUFLEN 882
#define F3M_CHNS 2
#include "f3m.c"
player_s s3mplayer;

//extern mod_s fsys_s3m_test[];

int main(void);
void yield(void);

extern volatile uint8_t _BSS_START[];
extern volatile uint8_t _BSS_END[];
extern volatile uint32_t int_handler_stub[];
extern volatile uint32_t int_handler_stub_end[];

volatile int got_vblank = 0;
int waiting_for_vblank = 0;
volatile int playing_music = 0;
void isr_handler_c(uint32_t cop0_sr, uint32_t cop0_cause, uint32_t cop0_epc)
{
	(void)cop0_sr;
	(void)cop0_cause;
	(void)cop0_epc;

	//*(uint32_t *)0x801FFFFC = cop0_cause;
	//*(uint32_t *)0x801FFFF8 = cop0_sr;

	if((I_STAT & 0x0080) != 0)
	{
		while((JOY_STAT & 0x0080) != 0) {}
		joy_update();

		I_STAT &= ~0x0080;
		I_MASK |= 0x0080;
		JOY_CTRL |= 0x0010;
		// TODO handle other joypad interrupts
	}

	if((I_STAT & 0x0001) != 0)
	{
		TMR_n_MODE(1) = 0;
		TMR_n_COUNT(1) = 0;
		TMR_n_MODE(1) = 0x0300;
		got_vblank++;
		if(playing_music) f3m_player_play(&s3mplayer, NULL, NULL);
		I_STAT &= ~0x0001;
		I_MASK |= 0x0001;
	}
}

extern void aaa_nop_sled_cache_clearer(void);

void aaa_start(void)
{
	static volatile int fencer = 0;
	(void)fencer;
	int i;

	I_MASK = 0;

	// Clear cache
	aaa_nop_sled_cache_clearer();

	// Patch interrupt handler
	((volatile uint32_t *)0x80000080)[0] = int_handler_stub[0];
	((volatile uint32_t *)0x80000080)[1] = int_handler_stub[1];
	((volatile uint32_t *)0x80000080)[2] = int_handler_stub[2];

	fencer = 2;

	//memset((void *)(_BSS_START), 0, _BSS_END - _BSS_START);

	fencer = 1;

	I_STAT = 0xFFFF;
	I_MASK = 0x0001;
	asm volatile (
		"\tmfc0 $t0, $12\n"
		"\tori $t0, 0x7F01\n"
		"\tmtc0 $t0, $12\n"
		/*
		"\tmfc0 $t0, $13\n"
		"\tori $t0, 0x0300\n"
		"\tmtc0 $t0, $13\n"
		*/
		/*
		"\tsyscall\n"
		"\tnop\n"
		*/

		/*
		"\tsyscall\n"
		"\tnop\n"
		*/

		:::"t0"
	);

	//InitHeap(0x100000, 0x0F0000);
	/*
	// this code does work, no guarantees that it won't clobber things though
	asm volatile (
		"\tlui $4, 0x0010\n"
		"\tlui $5, 0x000F\n"
		"\tori $9, $zero, 0x39\n"
		"\taddiu $sp, $sp, 0x10\n"
		"\tori $2, $zero, 0x00A0\n"
		"\tjalr $2\n"
		"\tnop\n"
		"\taddiu $sp, $sp, -0x10\n"
	);
	*/

	main();

	for(;;)
		yield();
}

void yield(void)
{
	// TODO: halt 
}

void get_mem_info (void *mem);
void update_frame(void);
void game_init_assets(void);
int main(void)
{
	int i;
	static volatile int k = 0;
	int x, y, xc, yc;

	(void)k;

	// KEEP get_mem_info
	// this is so the optimiser doesn't destroy our function
	uint32_t mem_tmp[3];
	get_mem_info((void *)&mem_tmp);
	k = mem_tmp[0];

	// Disable DMA
	DMA_DPCR &= ~0x08888888;

	// Reset GPU 
	gpu_init();
	dma_init();

	// Steal GPU ranges for screen
	// FIXME: confirm these
	glTexStealRangePSX(0, 0, 320*4, 240*2);
	glTexStealRangePSX(512*4, 0, 512*4, 8);
	glTexStealRangePSX(448*4, 0, 8, 8);

	// Set up joypad
	joy_init();

	// Prep module
	f3m_player_init(&s3mplayer, fsys_s3m_test);

	SPU_CNT = 0xC000;
	SPU_MVOL_L = 0x2000;
	SPU_MVOL_R = 0x2000;

	game_init_assets();

	waiting_for_vblank = got_vblank;
	playing_music = 1;
	for(;;)
	{
		while(waiting_for_vblank >= got_vblank) {}
		int tmp_wait = got_vblank;
		waiting_for_vblank++;
		//f3m_player_play(&s3mplayer, NULL, NULL);
		update_frame();
		waiting_for_vblank = tmp_wait;
	}

	for(;;)
		yield();

	return 0;
}


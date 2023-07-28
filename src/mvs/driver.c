/******************************************************************************

	driver.c

	MVS ドライバ

******************************************************************************/

#include "mvs.h"


#define IRQ2CTRL_ENABLE				0x10
#define IRQ2CTRL_LOAD_RELATIVE		0x20
#define IRQ2CTRL_AUTOLOAD_VBLANK	0x40
#define IRQ2CTRL_AUTOLOAD_REPEAT	0x80


/******************************************************************************
	グローバル変数
******************************************************************************/

int neogeo_driver_type;
int neogeo_raster_enable;
uint16_t neogeo_ngh;

uint8_t auto_animation_speed;
uint8_t auto_animation_disabled;
uint8_t auto_animation_counter;

struct cacheinfo_t MVS_cacheinfo[] =
{
//     name,  parent,  crom,  srom,  vrom
	{ "aof2a",    "aof2",     0, 0, 0 },
	{ "fatfursa", "fatfursp", 0, 0, 0 },
	{ "kof95h",   "kof95",    0, 0, 0 },
	{ "samsho3h", "samsho3",  0, 0, 0 },
	{ "fswords",  "samsho3",  0, 0, 0 },
	{ "aof3k",    "aof3",     0, 0, 0 },
	{ "kof96ae",  "kof96",    1, 1, 1 },
	{ "kof96cn",  "kof96",    1, 1, 0 },
	{ "kof96h",   "kof96",    0, 0, 0 },
	{ "kof96ep",  "kof96",    0, 0, 0 },
	{ "kof96pm",  "kof96",    0, 0, 0 },
	{ "kof96sp",  "kof96",    1, 1, 0 },
	{ "kizuna",   "savagere", 1, 1, 1 },
	{ "kof97h",   "kof97",    0, 0, 0 },
	{ "kof97c",   "kof97",    1, 1, 0 },
	{ "kof97cn",  "kof97",    1, 1, 0 },
	{ "kof97d",   "kof97",    0, 0, 0 },
	{ "kof97k",   "kof97",    0, 0, 0 },
	{ "kof97pls", "kof97",    0, 0, 0 },
	{ "kof97pla", "kof97",    0, 1, 0 },
	{ "kof97prc", "kof97",    0, 0, 0 },
	{ "kof97ps",  "kof97",    1, 0, 0 },
	{ "kof97yk",  "kof97",    1, 1, 1 },
	{ "kof97xt",  "kof97",    1, 1, 0 },
	{ "kog",      "kof97",    1, 1, 0 },
	{ "kogd",     "kof97",    1, 1, 0 },
	{ "kof97oro", "kof97",    1, 1, 0 },
	{ "lastbladh","lastblad", 0, 0, 0 },
	{ "lastsold", "lastblad", 0, 0, 0 },
	{ "shocktroa","shocktro", 0, 0, 0 },
	{ "rbff2h",   "rbff2",    0, 0, 0 },
	{ "rbff2k",   "rbff2",    0, 0, 0 },
	{ "kof98a",   "kof98",    0, 0, 0 },
	{ "kof98c",   "kof98",    1, 1, 0 },
	{ "kof98cn",  "kof98",    1, 1, 0 },
	{ "kof98evo", "kof98",    1, 0, 0 },
	{ "kof98k",   "kof98",    0, 0, 0 },
	{ "kof98ka",  "kof98",    0, 0, 0 },
	{ "kof98h",   "kof98",    0, 0, 0 },
	{ "kof98ae",  "kof98",    1, 1, 1 },
	{ "breakrev", "breakers", 1, 1, 1 },
	{ "lans2004", "shocktr2", 1, 1, 1 },
	{ "kof99h",   "kof99",    0, 0, 0 },
	{ "kof99e",   "kof99",    0, 0, 0 },
	{ "kof99k",   "kof99",    0, 0, 0 },
	{ "kof99p",   "kof99",    1, 1, 0 },
	{ "kof99ae",  "kof99",    1, 1, 1 },
	{ "garouh",   "garou",    0, 0, 0 },
	{ "garoubl",  "garoup",   0, 1, 1 },
	{ "mslugxc1", "mslugx",   0, 0, 0 },
	{ "mslug3h",  "mslug3",   0, 0, 0 },
	{ "mslug3b6", "mslug3",   0, 1, 0 },
	{ "kof2000n", "kof2000",  0, 0, 0 },
	{ "kof2kcn",  "kof2000",  1, 1, 0 },
	{ "kof2kps2", "kof2000",  1, 1, 0 },
	{ "kof2001h", "kof2001",  0, 0, 0 },
	{ "kf2k1pls", "kof2001",  0, 0, 0 },
	{ "kf2k1pa",  "kof2001",  0, 0, 0 },
	{ "kof2k1bs", "kof2001",  1, 1, 0 },
	{ "cthd2003", "kof2001",  1, 1, 0 },
	{ "cthd2k3a", "kof2001",  1, 1, 1 },
	{ "ct2k3sp",  "kof2001",  1, 1, 0 },
	{ "ct2k3sa",  "kof2001",  1, 1, 0 },
	{ "mslug4h",  "mslug4",   0, 0, 0 },
	{ "ms4plus",  "mslug4",   0, 0, 0 },
	{ "kof2002b", "kof2002",  1, 0, 0 },
	{ "kof2k2cn", "kof2002",  1, 1, 0 },
	{ "kf2k2pls", "kof2002",  0, 0, 0 },
	{ "kf2k2pla", "kof2002",  0, 0, 0 },
	{ "kf2k2plb", "kof2002",  0, 0, 0 },
	{ "kf2k2plc", "kof2002",  0, 1, 0 },
	{ "kf2k2mp",  "kof2002",  0, 1, 0 },
	{ "kf2k2mp2", "kof2002",  0, 1, 0 },
	{ "kf2k2ps2", "kof2002",  1, 1, 1 },
	{ "kf2k2ur",  "kof2002",  1, 1, 1 },
	{ "kof2k2tg3","kof2002",  1, 1, 1 },
	{ "matrimbl", "matrim",   0, 0, 1 },
	{ "mslug5b",  "mslug5",   0, 0, 0 },
	{ "mslug5h",  "mslug5",   0, 0, 0 },
	{ "ms5plus",  "mslug5",   0, 1, 0 },
	{ "svcpcba",  "svcpcb",   0, 0, 0 },
	{ "samsho5h", "samsho5",  0, 0, 0 },
	{ "samsho5b", "samsho5",  1, 1, 1 },
	{ "samsh5sph","samsh5sp", 0, 0, 0 },
	{ "samsh5spho","samsh5sp", 0, 0, 0 },
	{ "kof2k4se", "kof2002",  1, 1, 1 },
	{ "kf2k4pls", "kof2k4se", 0, 1, 0 },
	{ "kf10thep", "kof10th",  1, 1, 0 },
	{ "kf2k5uni", "kof10th",  0, 0, 0 },
	{ "svcplus",  "svcboot",  0, 1, 0 },
	{ "svcplusa", "svcboot",  0, 0, 0 },
	{ "svcsplus", "svcboot",  0, 1, 0 },
	{ "svcps2",   "svc",      1, 1, 1 },
	{ "kf2k3bla", "kf2k3bl",  0, 0, 0 },
	{ "kf2k3pl",  "kf2k3bl",  0, 1, 0 },
	{ "kf2k3upl", "kf2k3bl",  0, 1, 0 },
	{ "kf2k3ps2", "kof2003",  1, 1, 1 },
	{ "ironclado","ironclad", 0, 0, 0 },
	{ "jockeygpa","jockeygp", 0, 0, 0 },
	{ "rbff1a",   "rbff1",    0, 0, 0 },
	{ "rbffspeck","rbffspec", 0, 0, 0 },
	{ "samsho2k", "samsho2",  0, 0, 0 },
	{ "samsho2k2","samsho2",  0, 0, 0 },
	{ "samsho4k", "samsho4",  0, 0, 0 },
	{ "shocktroa","shocktro", 0, 0, 0 },
	{ NULL }
};


/******************************************************************************
	ローカル変数
******************************************************************************/

static int raster_enable;

static uint16_t raster_counter;
static int scanline_read;
static int busy;

static int display_position_interrupt_counter;
static int display_position_interrupt_control;
static uint32_t display_counter;

static int display_position_interrupt_pending;
static int vblank_interrupt_pending;

static uint8_t auto_animation_frame_counter;

static int sound_code;
static int result_code;
static int pending_command;

static uint8_t main_cpu_vector_table_source;
static uint8_t controller_select;
static uint8_t save_ram_unlocked;

static uint32_t m68k_second_bank;
static uint32_t z80_bank[4];

static uint16_t *neogeo_cpu1_second_bank;

static uint8_t ALIGN_DATA neogeo_game_vectors[0x80];
static uint8_t *neogeo_vectors[2];

static uint16_t neogeo_rng = 0x2345;


/******************************************************************************
	プロトタイプ
******************************************************************************/

static void update_interrupts(void);


/*------------------------------------------------------
	Set CPU1 second ROM bank
------------------------------------------------------*/

static void neogeo_set_cpu1_second_bank(uint32_t offset)
{
	if (m68k_second_bank != offset)
	{
		m68k_second_bank = offset;
		neogeo_cpu1_second_bank = (uint16_t *)((uint32_t)memory_region_cpu1 + offset - 0x200000);
		C68k_Set_Fetch(&C68K, 0x200000, 0x2fffff, (uint32_t)&memory_region_cpu1[offset]);
	}
}


/*------------------------------------------------------
	Set CPU2 ROM bank
------------------------------------------------------*/

static void neogeo_set_cpu2_bank(int bank, uint32_t offset)
{
	if (z80_bank[bank] != offset)
	{
		z80_bank[bank] = offset;

		switch (bank)
		{
		case 0: memcpy(&memory_region_cpu2[0x8000], &memory_region_cpu2[offset + 0x10000], 0x4000); break;
		case 1: memcpy(&memory_region_cpu2[0xc000], &memory_region_cpu2[offset + 0x10000], 0x2000); break;
		case 2: memcpy(&memory_region_cpu2[0xe000], &memory_region_cpu2[offset + 0x10000], 0x1000); break;
		case 3: memcpy(&memory_region_cpu2[0xf000], &memory_region_cpu2[offset + 0x10000], 0x0800); break;
		}
	}
}


/*------------------------------------------------------
	Inisialize driver
------------------------------------------------------*/

void neogeo_driver_init(void)
{
	memcpy(neogeo_game_vectors, memory_region_cpu1, 0x80);

	neogeo_vectors[0] = memory_region_user1;
	neogeo_vectors[1] = neogeo_game_vectors;

	m68k_second_bank = 0xffffffff;
	z80_bank[0] = 0xffffffff;
	z80_bank[1] = 0xffffffff;
	z80_bank[2] = 0xffffffff;
	z80_bank[3] = 0xffffffff;

	if (memory_length_cpu1 > 0x100000)
		neogeo_set_cpu1_second_bank(0x100000);
	else
		neogeo_set_cpu1_second_bank(0x000000);

	neogeo_set_cpu2_bank(0, 0x8000);
	neogeo_set_cpu2_bank(1, 0xc000);
	neogeo_set_cpu2_bank(2, 0xe000);
	neogeo_set_cpu2_bank(3, 0xf000);

	m68000_init();
	z80_init();
}


/*------------------------------------------------------
	Reset driver
------------------------------------------------------*/

void neogeo_driver_reset(void)
{
#ifdef ADHOC
	if (adhoc_enable)
	{
		pd4990a.seconds = 0;
		pd4990a.minutes = 0;
		pd4990a.hours   = 0;
		pd4990a.days    = 0;
		pd4990a.month   = 0;
		pd4990a.year    = 0;
		pd4990a.weekday = 0;
	}
	else
#endif
	{
		int y, m, d;

		time_t now = time(NULL);
   		struct tm *today = localtime(&now);


		pd4990a.seconds = today->tm_sec;
		pd4990a.minutes = today->tm_min;
		pd4990a.hours   = today->tm_hour;
		pd4990a.days    = today->tm_mday;
		pd4990a.month   = today->tm_mon;
		pd4990a.year    = today->year % 100;

		y = today.year;
		m = today.month;
		d = today.day;
		if (m == 1 || m == 2) { y--; m += 12; }
		pd4990a.weekday = (y + (y / 4) - (y / 100) + (y / 400) + ((13 * m + 8) / 5) + d) % 7;
	}

	memset(neogeo_ram, 0, 0x10000);

	memcpy(memory_region_cpu1, neogeo_vectors[0], 0x80);	// bios vector
	main_cpu_vector_table_source = 0;

	watchdog_reset_w(0, 0, 0);

	raster_counter = RASTER_COUNTER_START;
	scanline_read = 0;

	display_position_interrupt_counter = 0;
	display_position_interrupt_control = 0;
	display_counter = 0;

	vblank_interrupt_pending = 0;
	display_position_interrupt_pending = 0;

	sound_code = 0;
	result_code = 0;
	pending_command = 0;

	auto_animation_frame_counter = 0;
	auto_animation_speed = 0;
	auto_animation_disabled = 0;
	auto_animation_counter = 0;

	neogeo_rng = 0x2345;
	save_ram_unlocked = 0;

	controller_select = 0;

	neogeo_reset_driver_type();

	if (machine_init_type == INIT_ms5pcb
	||	machine_init_type == INIT_svcpcb)
	{
		memcpy(memory_region_user1, memory_region_user1 + 0x20000 + neogeo_hard_dipsw * 0x20000, 0x20000);
	}
#if !RELEASE
	if (machine_init_type == INIT_kog)
	{
		memory_region_cpu1[0x1ffffc/2] = neogeo_hard_dipsw;
	}
#endif

	m68000_reset();
	z80_reset();
}


/*------------------------------------------------------
	Reset driver type
------------------------------------------------------*/

void neogeo_reset_driver_type(void)
{
	busy = 0;

	if (neogeo_ngh == NGH_tpgolf
	||	neogeo_ngh == NGH_trally
	||	neogeo_ngh == NGH_spinmast
	||	neogeo_ngh == NGH_neodrift)
		neogeo_raster_enable = 1;

	raster_enable = neogeo_raster_enable;

	if (neogeo_bios > JAPAN_AES && neogeo_bios < DEBUG_BIOS)
		raster_enable = 1;

	if (raster_enable)
	{
		if (neogeo_ngh == NGH_mosyougi) busy = 1;

		neogeo_driver_type = RASTER;
	}
	else
	{
		neogeo_driver_type = NORMAL;
	}

	timer_set_update_handler();
}


/*------------------------------------------------------
	Adjust display position interrupt
------------------------------------------------------*/

INLINE void adjust_display_position_interrupt(void)
{
	if ((display_counter + 1) != 0)
	{
		display_position_interrupt_counter = (display_counter + 1) / 0x180;
	}
}


/*------------------------------------------------------
	Update interrupt
------------------------------------------------------*/

static void update_interrupts(void)
{
	int level = 0;

	/* determine which interrupt is active */
	if (vblank_interrupt_pending) level = 1;
	if (display_position_interrupt_pending) level = 2;

	/* either set or clear the appropriate lines */
	if (level)
		m68000_set_irq_line(level, ASSERT_LINE);
	else
		m68000_set_irq_line(7, CLEAR_LINE);
}


/*------------------------------------------------------
	MC68000 VBLANK interrupt (IRQ1)
------------------------------------------------------*/

void neogeo_vblank_interrupt(void)
{
	raster_counter = RASTER_COUNTER_START;

	pd4990a_addretrace();

	if (!auto_animation_disabled)
	{
		if (auto_animation_frame_counter == 0)
		{
			auto_animation_frame_counter = auto_animation_speed;
			auto_animation_counter++;
		}
		else auto_animation_frame_counter--;
	}

	vblank_interrupt_pending = 1;

	update_interrupts();
}


/*------------------------------------------------------
	MC68000 scanline interrupt (IRQ2)
------------------------------------------------------*/

void neogeo_raster_interrupt(int line)
{
	int do_refresh = 0;
	uint16_t raster_line = line;

	if (raster_line == RASTER_LINES) raster_line = 0;

	if (raster_line < RASTER_LINE_RELOAD)
		raster_counter = RASTER_COUNTER_START + raster_line;
	else
		raster_counter = RASTER_COUNTER_RELOAD + raster_line - RASTER_LINE_RELOAD;

	if (busy)
	{
		if (scanline_read)
		{
			do_refresh = neogeo_raster_enable;
			scanline_read = 0;
		}
	}

	if (display_position_interrupt_counter > 0)
	{
		display_position_interrupt_counter--;

		if (display_position_interrupt_counter == 0)
		{
			if (display_position_interrupt_control & IRQ2CTRL_ENABLE)
			{
				if (!busy) do_refresh = neogeo_raster_enable;

				display_position_interrupt_pending = 1;
			}

			if (display_position_interrupt_control & IRQ2CTRL_AUTOLOAD_REPEAT)
				adjust_display_position_interrupt();
		}
	}

	if (line == RASTER_LINES)
	{
		if (display_position_interrupt_control & IRQ2CTRL_AUTOLOAD_VBLANK)
			adjust_display_position_interrupt();

		pd4990a_addretrace();

		if (!auto_animation_disabled)
		{
			if (auto_animation_frame_counter == 0)
			{
				auto_animation_frame_counter = auto_animation_speed;
				auto_animation_counter++;
			}
			else auto_animation_frame_counter--;
		}

		vblank_interrupt_pending = 1;
	}

	if (do_refresh && !skip_this_frame())
		neogeo_partial_screenrefresh((raster_counter - 0x100) - 1);

	update_interrupts();
}


/****************************************************************************
	MC68K memory access
****************************************************************************/

/*------------------------------------------------------
	Select BIOS vector (0x3a0003 / 0x3a0013)
------------------------------------------------------*/

INLINE void set_main_cpu_vector_table_source(uint8_t data)
{
	memcpy(memory_region_cpu1, neogeo_vectors[data], 0x80);
	main_cpu_vector_table_source = data;
	display_position_interrupt_counter = 0;
	blit_set_fix_clear_flag();
	blit_set_spr_clear_flag();
	autoframeskip_reset();

	// hack for PSP
	if (data)
	{
		max_sprite_number = MAX_SPRITES_PER_SCREEN;
	}
	else
	{
		if (neogeo_bios > JAPAN_AES && neogeo_bios < DEBUG_BIOS)
			max_sprite_number = 32;
		else if (neogeo_bios == ASIA_AES
		|| neogeo_bios == JAPAN_AES
		|| neogeo_bios == DEBUG_BIOS)
			max_sprite_number = MAX_SPRITES_PER_SCREEN;
		else
			max_sprite_number = 0;
	}
}


/*------------------------------------------------------
	Select palette RAM bank  (0x3a000f / 0x3a001f)
------------------------------------------------------*/

INLINE void neogeo_set_palette_bank(uint8_t data)
{
	if (palette_bank != data)
	{
		palette_bank = data;

		video_palette = video_palettebank[data];
	}
}


/*------------------------------------------------------
	Write VRAM offset ($3c0001)
------------------------------------------------------*/

INLINE void set_videoram_offset(uint16_t data)
{
	videoram_offset = data;

	/* the read happens right away */
	videoram_read_buffer = neogeo_videoram[videoram_offset];
}


/*------------------------------------------------------
	Read data from VRAM ($3c0001 / $3c0003)
------------------------------------------------------*/

INLINE uint16_t get_videoram_data(void)
{
	return videoram_read_buffer;
}


/*------------------------------------------------------
	Write data to VRAM ($3c0003)
------------------------------------------------------*/

INLINE void set_videoram_data(uint16_t data)
{
	neogeo_videoram[videoram_offset] = data;

	/* auto increment/decrement the current offset - A15 is NOT effected */
	videoram_offset = (videoram_offset & 0x8000) | ((videoram_offset + videoram_modulo) & 0x7fff);

	/* read next value right away */
	videoram_read_buffer = neogeo_videoram[videoram_offset];
}


/*------------------------------------------------------
	Read VRAM modulo ($3c0005)
------------------------------------------------------*/

INLINE uint16_t get_videoram_modulo(void)
{
	return videoram_modulo;
}


/*------------------------------------------------------
	Write VRAM modulo ($3c0005)
------------------------------------------------------*/

INLINE void set_videoram_modulo(uint16_t data)
{
	videoram_modulo = data;
}


/*---------------------------------------------------------
	Read video control data ($3c0007)
---------------------------------------------------------*/

INLINE uint16_t get_video_control(void)
{
	scanline_read = 1;

	if (neogeo_driver_type == NORMAL)
	{
		uint16_t raster_line = timer_getscanline();

		if (raster_line == RASTER_LINES) raster_line = 0;

		if (raster_line < RASTER_LINE_RELOAD)
			raster_counter = RASTER_COUNTER_START + raster_line;
		else
			raster_counter = RASTER_COUNTER_RELOAD + raster_line - RASTER_LINE_RELOAD;
	}

	return (raster_counter << 7) | (auto_animation_counter & 0x0007);
}


/*---------------------------------------------------------
	Write video control data ($3c0007)
---------------------------------------------------------*/

INLINE void set_video_control(uint16_t data)
{
	auto_animation_speed = data >> 8;
	auto_animation_disabled = data & 0x0008;

	display_position_interrupt_control = data & 0x00f0;
}


/*------------------------------------------------------
	Set display counter (MSB) (0x3c0008)
------------------------------------------------------*/

INLINE void neogeo_set_display_counter_msb(uint16_t data)
{
	if (neogeo_driver_type == NORMAL) return;

	display_counter = (display_counter & 0x0000ffff) | ((uint32_t)data << 16);
}


/*------------------------------------------------------
	Set display counter (LSB) (0x3c000a)
------------------------------------------------------*/

INLINE void neogeo_set_display_counter_lsb(uint16_t data)
{
	if (neogeo_driver_type == NORMAL) return;

	display_counter = (display_counter & 0xffff0000) | data;

	if (display_position_interrupt_control & IRQ2CTRL_LOAD_RELATIVE)
		adjust_display_position_interrupt();
}


/*------------------------------------------------------
	Write IRQ acknowledge ($3c000c)
------------------------------------------------------*/

INLINE void neogeo_acknowledge_interrupt(uint16_t data)
{
	if (data & 0x02) display_position_interrupt_pending = 0;
	if (data & 0x04) vblank_interrupt_pending = 0;

	update_interrupts();
}


/****************************************************************************
	M68000 memory handlers
****************************************************************************/

/*------------------------------------------------------
	Set second ROM bank ($2ffff0 - $2fffff)
------------------------------------------------------*/

WRITE16_HANDLER( neogeo_bankswitch_w )
{
	if ((offset & (0x2ffff0/2)) == (0x2ffff0/2))
	{
		uint32_t bankaddress;

		if (memory_length_cpu1 <= 0x100000)
			return;

		data = data & 0x7;
		bankaddress = (data + 1) << 20;
		if (bankaddress >= memory_length_cpu1)
			bankaddress = 0x100000;

		neogeo_set_cpu1_second_bank(bankaddress);
	}
}


/*------------------------------------------------------
	Reset watchdog counter ($300000 - $300001)
------------------------------------------------------*/

TIMER_CALLBACK( watchdog_callback )
{
	if (Loop == LOOP_EXEC) Loop = LOOP_RESET;
}


WRITE16_HANDLER( watchdog_reset_w )
{
	timer_set(WATCHDOG_TIMER, 128762, 0, watchdog_callback);
}


/*------------------------------------------------------
	Read controller 2 ($340000 - $340001)
------------------------------------------------------*/

READ16_HANDLER( neogeo_controller2_r )
{
	if (neogeo_ngh == NGH_popbounc)
	{
		if (!controller_select)
			return input_analog_value[1] << 8;
	}

	return neogeo_port_value[1] << 8;
}


/*------------------------------------------------------
	Read controller 3 ($380000 - $380001)
------------------------------------------------------*/

READ16_HANDLER( neogeo_controller3_r )
{
#ifdef ADHOC
	if (adhoc_enable)
		return neogeo_port_value[2] << 8;
	else
#endif
		return (neogeo_port_value[2] << 8) & 0x8fff;
}


/*------------------------------------------------------
	Read controller 1 and 4 ($380000 - $380001)
	                        ($380008 - $380009)
------------------------------------------------------*/

READ16_HANDLER( neogeo_controller1and4_r )
{
	if (offset & 0x40)
	{
		return neogeo_port_value[5];
	}
	else
	{
		switch (neogeo_ngh)
		{
		case NGH_irrmaze:
			if (controller_select)
				return (input_analog_value[1] << 8) + neogeo_port_value[3];
			return (input_analog_value[0] << 8) + neogeo_port_value[3];

		case NGH_popbounc:
			if (!controller_select)
				return (input_analog_value[0] << 8) + neogeo_port_value[3];
			break;
		}

		return (neogeo_port_value[0] << 8) + neogeo_port_value[3];
	}
}


/*------------------------------------------------------
	Read timer ($320001)
------------------------------------------------------*/

READ16_HANDLER( neogeo_timer_r )
{
	uint16_t res;
	int coinflip = pd4990a_testbit_r(0);
	int databit = pd4990a_databit_r(0);

	res = (neogeo_ngh == NGH_vliner) ? 0x3f : neogeo_port_value[4];
	res ^= (coinflip << 6) ^ (databit << 7);

	res |= result_code << 8;
	if (pending_command) res &= 0x7fff;

	return res;
}


/*------------------------------------------------------
	Write Z80 communication data ($320001)
------------------------------------------------------*/

TIMER_CALLBACK( neogeo_sound_write )
{
	sound_code = param;
	z80_set_irq_line(IRQ_LINE_NMI, PULSE_LINE);
}


WRITE16_HANDLER( neogeo_z80_w )
{
	pending_command = 1;
	timer_set(SOUNDLATCH_TIMER, TIME_NOW, (data >> 8) & 0xff, neogeo_sound_write);
}


/*------------------------------------------------------
	Write I/O control switch ($380000 - $3800ff)
------------------------------------------------------*/

WRITE16_HANDLER( io_control_w )
{
	switch (offset & 0xff/2)
	{
	case 0x00: controller_select = data & 1; break;
//	case 0x18: set_output_latch(data & 0x00ff); break;
//	case 0x20: set_output_data(data & 0x00ff); break;
	case 0x28: pd4990a_control_w(0, data, mem_mask); break;
	}
}


/*------------------------------------------------------
	Write system control switch ($3a0000 - $3a001f)
------------------------------------------------------*/

WRITE16_HANDLER( system_control_w )
{
	if ((mem_mask & 0x00ff) != 0x00ff)
	{
		uint8_t bit = (offset >> 3) & 1;

		switch (offset & 7)
		{
//		case 0x00: neogeo_set_screen_dark(bit); break;
		case 0x01: set_main_cpu_vector_table_source(bit); break;
		case 0x05: neogeo_set_fixed_layer_source(bit); break;
		case 0x06: save_ram_unlocked = bit; break;
		case 0x07: neogeo_set_palette_bank(bit); break;
		}
	}
}


/*------------------------------------------------------
	Read VRAM register (0x3c0000 - 0x3c000f)
------------------------------------------------------*/

READ16_HANDLER( neogeo_video_register_r )
{
	if (mem_mask == 0xff00)
		return 0xff;

	switch (offset & 3)
	{
	case 0x00:
	case 0x01: return get_videoram_data();
	case 0x02: return get_videoram_modulo();
	case 0x03: return get_video_control();
	}
	return 0xffff;
}


/*------------------------------------------------------
	Write VRAM register (0x3c0000 - 0x3c000f)
------------------------------------------------------*/

WRITE16_HANDLER( neogeo_video_register_w )
{
	/* accessing the LSB only is not mapped */
	if (mem_mask != 0xff00)
	{
		/* accessing the MSB only stores same data in MSB and LSB */
		if (mem_mask == 0x00ff)
			data = (data & 0xff00) | (data >> 8);

		switch (offset & 7)
		{
		case 0x00: set_videoram_offset(data); break;
		case 0x01: set_videoram_data(data); break;
		case 0x02: set_videoram_modulo(data); break;
		case 0x03: set_video_control(data); break;
		case 0x04: neogeo_set_display_counter_msb(data); break;
		case 0x05: neogeo_set_display_counter_lsb(data); break;
		case 0x06: neogeo_acknowledge_interrupt(data); break;
		case 0x07: break; /* unknown, see get_video_control */
		}
	}
}


/*------------------------------------------------------
	Read from palette RAM (0x400000 - 0x40ffff)
------------------------------------------------------*/

READ16_HANDLER( neogeo_paletteram_r )
{
	offset &= 0xfff;
	return palettes[palette_bank][offset];
}


/*------------------------------------------------------
	Write to palette RAM (0x400000 - 0x40ffff)
------------------------------------------------------*/

WRITE16_HANDLER( neogeo_paletteram_w )
{
	uint16_t color;

	offset &= 0xfff;
	color = COMBINE_DATA(&palettes[palette_bank][offset]);

	if (offset & 0x0f)
		video_palette[offset] = video_clut16[color & 0x7fff];
}


/*------------------------------------------------------
	Read from memory card ($800000 - $8007ff)
------------------------------------------------------*/

READ16_HANDLER( neogeo_memcard16_r )
{
#ifdef ADHOC
	if (adhoc_enable)
		return ~0;
	else
#endif
		return neogeo_memcard[offset & 0x7ff] | 0xff00;
}


/*------------------------------------------------------
	Write to memory card ($800000 - $8007ff)
------------------------------------------------------*/

WRITE16_HANDLER( neogeo_memcard16_w )
{
	if (ACCESSING_LSB)
	{
#ifdef ADHOC
		if (!adhoc_enable)
#endif
			neogeo_memcard[offset & 0x7ff] = data & 0xff;
	}
}


/*------------------------------------------------------
	Write to 64K SRAM ($d00000 - $d0ffff)
------------------------------------------------------*/

WRITE16_HANDLER( neogeo_sram16_w )
{
	if (save_ram_unlocked)
	{
		COMBINE_DATA(&neogeo_sram16[offset & 0x7fff]);
	}
}


/****************************************************************************
	Z80 port access
****************************************************************************/

/*------------------------------------------------------
	Read Z80 port
------------------------------------------------------*/

uint8_t neogeo_z80_port_r(uint16_t port)
{
	switch (port & 0xff)
	{
	case 0x00:
		pending_command = 0;
		return sound_code;

	case 0x04:
		return YM2610_status_port_A_r(0);

	case 0x05:
		return YM2610_read_port_r(0);

	case 0x06:
		return YM2610_status_port_B_r(0);

	case 0x08:
		neogeo_set_cpu2_bank(3, (port & 0x7f00) << 3);
		break;

	case 0x09:
		neogeo_set_cpu2_bank(2, (port & 0x3f00) << 4);
		break;

	case 0x0a:
		neogeo_set_cpu2_bank(1, (port & 0x1f00) << 5);
		break;

	case 0x0b:
		neogeo_set_cpu2_bank(0, (port & 0x0f00) << 6);
		break;
	};

	return 0;
}


/*------------------------------------------------------
	Write Z80 port
------------------------------------------------------*/

void neogeo_z80_port_w(uint16_t port, uint8_t data)
{
	switch (port & 0xff)
	{
	case 0x04:
		YM2610_control_port_A_w(0, data);
		break;

	case 0x05:
		YM2610_data_port_A_w(0, data);
		break;

	case 0x06:
		YM2610_control_port_B_w(0, data);
		break;

	case 0x07:
		YM2610_data_port_B_w(0, data);
		break;

	case 0x0c:
		result_code = data;
		break;
	}
}


/******************************************************************************
	サウンドIRQハンドラ
******************************************************************************/

void neogeo_sound_irq(int irq)
{
	z80_set_irq_line(0, irq ? ASSERT_LINE : CLEAR_LINE);
}


/******************************************************************************
	プロテクション
******************************************************************************/

/*------------------------------------------------------
  Metal Slug X

  todo: emulate, not patch!
------------------------------------------------------*/

void mslugx_install_protection(void)
{
	int i;
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;

	for (i = 0;i < (0x100000/2) - 4;i++)
	{
		if (mem16[i + 0] == 0x0243 &&
			mem16[i + 1] == 0x0001 && 	/* andi.w  #$1, D3 */
			mem16[i + 2] == 0x6600)		/* bne xxxx */
		{
			mem16[i + 2] = 0x4e71;
			mem16[i + 3] = 0x4e71;
		}
	}
	mem16[0x3bdc/2] = 0x4e71;
	mem16[0x3bde/2] = 0x4e71;
	mem16[0x3be0/2] = 0x4e71;
	mem16[0x3c0c/2] = 0x4e71;
	mem16[0x3c0e/2] = 0x4e71;
	mem16[0x3c10/2] = 0x4e71;

	mem16[0x3c36/2] = 0x4e71;
	mem16[0x3c38/2] = 0x4e71;
}


/*------------------------------------------------------
	$200000メモリハンドラ (プロテクションなし)
------------------------------------------------------*/

READ16_HANDLER( neogeo_secondbank_r )
{
	return neogeo_cpu1_second_bank[offset];
}


WRITE16_HANDLER( neogeo_secondbank_w )
{
	neogeo_bankswitch_w(offset, data, mem_mask);
}


/*------------------------------------------------------
	Fatal Fury 2
------------------------------------------------------*/

static uint32_t neogeo_prot_data;

READ16_HANDLER( fatfury2_protection_r )
{
	uint16_t res = (neogeo_prot_data >> 24);

	switch (offset & (0xfffff/2))
	{
	case 0x55550/2:
	case 0xffff0/2:
	case 0x00000/2:
	case 0xff000/2:
	case 0x36000/2:
	case 0x36008/2:
		return res;

	case 0x36004/2:
	case 0x3600c/2:
		return ((res & 0xf0) >> 4) | ((res & 0x0f) << 4);
	}
	return 0;
}

WRITE16_HANDLER( fatfury2_protection_w )
{
	switch (offset & (0xfffff/2))
	{
	case 0x11112/2: /* data == 0x1111; expects 0xff000000 back */
		neogeo_prot_data = 0xff000000;
		break;

	case 0x33332/2: /* data == 0x3333; expects 0x0000ffff back */
		neogeo_prot_data = 0x0000ffff;
		break;

	case 0x44442/2: /* data == 0x4444; expects 0x00ff0000 back */
		neogeo_prot_data = 0x00ff0000;
		break;

	case 0x55552/2: /* data == 0x5555; read back from 55550, ffff0, 00000, ff000 */
		neogeo_prot_data = 0xff00ff00;
		break;

	case 0x56782/2: /* data == 0x1234; read back from 36000 *or* 36004 */
		neogeo_prot_data = 0xf05a3601;
		break;

	case 0x42812/2: /* data == 0x1824; read back from 36008 *or* 3600c */
		neogeo_prot_data = 0x81422418;
		break;

	case 0x55550/2:
	case 0xffff0/2:
	case 0xff000/2:
	case 0x36000/2:
	case 0x36004/2:
	case 0x36008/2:
	case 0x3600c/2:
		neogeo_prot_data <<= 8;
		break;
	}
}


/*------------------------------------------------------
  King of Fighters 98

  The encrypted set has a rom overlay feature, checked at
  various points in the game
------------------------------------------------------*/

WRITE16_HANDLER( kof98_protection_w )
{
	if (offset == 0x20aaaa/2)
	{
		/* info from razoola */
		uint16_t *mem16 = (uint16_t *)memory_region_cpu1;

		switch (data)
		{
		case 0x0090:
			mem16[0x100/2] = 0x00c2;
			mem16[0x102/2] = 0x00fd;
			break;

		case 0x00f0:
			mem16[0x100/2] = 0x4e45;
			mem16[0x102/2] = 0x4f2d;
			break;
		}
	}
	else
	{
		neogeo_bankswitch_w(offset, data, mem_mask);
	}
}


/*------------------------------------------------------
  SMA Protection

  thanks to Razoola
------------------------------------------------------*/

/* information about the sma random number generator provided by razoola */
/* this RNG is correct for KOF99, other games might be different */

static uint16_t sma_random_r(void)
{
	uint16_t old = neogeo_rng;

	uint16_t newbit = (
			(neogeo_rng >>  2) ^
			(neogeo_rng >>  3) ^
			(neogeo_rng >>  5) ^
			(neogeo_rng >>  6) ^
			(neogeo_rng >>  7) ^
			(neogeo_rng >> 11) ^
			(neogeo_rng >> 12) ^
			(neogeo_rng >> 15)) & 1;

	neogeo_rng = (neogeo_rng << 1) | newbit;

	return old;
}


READ16_HANDLER( kof99_protection_r )
{
	switch (offset)
	{
	case 0x2fe446/2: return 0x9a37;
	case 0x2ffff8/2: return sma_random_r();
	case 0x2ffffa/2: return sma_random_r();
	}
	return neogeo_cpu1_second_bank[offset];
}

WRITE16_HANDLER( kof99_protection_w )
{
	if (offset == 0x2ffff0/2)
	{
		int bankaddress;
		static int bankoffset[64] =
		{
			0x000000, 0x100000, 0x200000, 0x300000,
			0x3cc000, 0x4cc000, 0x3f2000, 0x4f2000,
			0x407800, 0x507800, 0x40d000, 0x50d000,
			0x417800, 0x517800, 0x420800, 0x520800,
			0x424800, 0x524800, 0x429000, 0x529000,
			0x42e800, 0x52e800, 0x431800, 0x531800,
			0x54d000, 0x551000, 0x567000, 0x592800,
			0x588800, 0x581800, 0x599800, 0x594800,
			0x598000,	/* rest not used? */
		};

		/* unscramble bank number */
		data =
			(((data >> 14) & 1) << 0) +
			(((data >>  6) & 1) << 1) +
			(((data >>  8) & 1) << 2) +
			(((data >> 10) & 1) << 3) +
			(((data >> 12) & 1) << 4) +
			(((data >>  5) & 1) << 5);

		bankaddress = 0x100000 + bankoffset[data];

		neogeo_set_cpu1_second_bank(bankaddress);
	}
}


READ16_HANDLER( garou_protection_r )
{
	switch (offset)
	{
	case 0x2fe446/2: return 0x9a37;
	case 0x2fffcc/2:
	case 0x2ffff0/2: return sma_random_r();
	}
	return neogeo_cpu1_second_bank[offset];
}

WRITE16_HANDLER( garou_protection_w )
{
	if (offset == 0x2fffc0/2)
	{
		/* thanks to Razoola and Mr K for the info */
		int bankaddress;
		static int bankoffset[64] =
		{
			0x000000, 0x100000, 0x200000, 0x300000, // 00
			0x280000, 0x380000, 0x2d0000, 0x3d0000, // 04
			0x2f0000, 0x3f0000, 0x400000, 0x500000, // 08
			0x420000, 0x520000, 0x440000, 0x540000, // 12
			0x498000, 0x598000, 0x4a0000, 0x5a0000, // 16
			0x4a8000, 0x5a8000, 0x4b0000, 0x5b0000, // 20
			0x4b8000, 0x5b8000, 0x4c0000, 0x5c0000, // 24
			0x4c8000, 0x5c8000, 0x4d0000, 0x5d0000, // 28
			0x458000, 0x558000, 0x460000, 0x560000, // 32
			0x468000, 0x568000, 0x470000, 0x570000, // 36
			0x478000, 0x578000, 0x480000, 0x580000, // 40
			0x488000, 0x588000, 0x490000, 0x590000, // 44
			0x5d0000, 0x5d8000, 0x5e0000, 0x5e8000, // 48
			0x5f0000, 0x5f8000, 0x600000, /* rest not used? */
		};

		/* unscramble bank number */
		data =
			(((data >>  5) & 1) << 0) +
			(((data >>  9) & 1) << 1) +
			(((data >>  7) & 1) << 2) +
			(((data >>  6) & 1) << 3) +
			(((data >> 14) & 1) << 4) +
			(((data >> 12) & 1) << 5);

		bankaddress = 0x100000 + bankoffset[data];

		neogeo_set_cpu1_second_bank(bankaddress);
	}
}

WRITE16_HANDLER( garouh_protection_w )
{
	if (offset == 0x2fffc0/2)
	{
		/* thanks to Razoola and Mr K for the info */
		int bankaddress;
		static int bankoffset[64] =
		{
			0x000000, 0x100000, 0x200000, 0x300000, // 00
			0x280000, 0x380000, 0x2d0000, 0x3d0000, // 04
			0x2c8000, 0x3c8000, 0x400000, 0x500000, // 08
			0x420000, 0x520000, 0x440000, 0x540000, // 12
			0x598000, 0x698000, 0x5a0000, 0x6a0000, // 16
			0x5a8000, 0x6a8000, 0x5b0000, 0x6b0000, // 20
			0x5b8000, 0x6b8000, 0x5c0000, 0x6c0000, // 24
			0x5c8000, 0x6c8000, 0x5d0000, 0x6d0000, // 28
			0x458000, 0x558000, 0x460000, 0x560000, // 32
			0x468000, 0x568000, 0x470000, 0x570000, // 36
			0x478000, 0x578000, 0x480000, 0x580000, // 40
			0x488000, 0x588000, 0x490000, 0x590000, // 44
			0x5d8000, 0x6d8000, 0x5e0000, 0x6e0000, // 48
			0x5e8000, 0x6e8000, 0x6e8000, 0x000000, // 52
			0x000000, 0x000000, 0x000000, 0x000000, // 56
			0x000000, 0x000000, 0x000000, 0x000000, // 60
		};

		/* unscramble bank number */
		data =
			(((data >>  4) & 1) << 0) +
			(((data >>  8) & 1) << 1) +
			(((data >> 14) & 1) << 2) +
			(((data >>  2) & 1) << 3) +
			(((data >> 11) & 1) << 4) +
			(((data >> 13) & 1) << 5);

		bankaddress = 0x100000 + bankoffset[data];

		neogeo_set_cpu1_second_bank(bankaddress);
	}
}


READ16_HANDLER( mslug3_protection_r )
{
	if (offset == 0x2fe466/2)
	{
		return 0x9a37;
	}
	return neogeo_cpu1_second_bank[offset];
}

WRITE16_HANDLER( mslug3_protection_w )
{
	if (offset == 0x2fffe4/2)
	{
		/* thanks to Razoola and Mr K for the info */
		int bankaddress;
		static int bankoffset[64] =
		{
			0x000000, 0x020000, 0x040000, 0x060000, // 00
			0x070000, 0x090000, 0x0b0000, 0x0d0000, // 04
			0x0e0000, 0x0f0000, 0x120000, 0x130000, // 08
			0x140000, 0x150000, 0x180000, 0x190000, // 12
			0x1a0000, 0x1b0000, 0x1e0000, 0x1f0000, // 16
			0x200000, 0x210000, 0x240000, 0x250000, // 20
			0x260000, 0x270000, 0x2a0000, 0x2b0000, // 24
			0x2c0000, 0x2d0000, 0x300000, 0x310000, // 28
			0x320000, 0x330000, 0x360000, 0x370000, // 32
			0x380000, 0x390000, 0x3c0000, 0x3d0000, // 36
			0x400000, 0x410000, 0x440000, 0x450000, // 40
			0x460000, 0x470000, 0x4a0000, 0x4b0000, // 44
			0x4c0000, /* rest not used? */
		};

		/* unscramble bank number */
		data =
			(((data >> 14) & 1) << 0) +
			(((data >> 12) & 1) << 1) +
			(((data >> 15) & 1) << 2) +
			(((data >>  6) & 1) << 3) +
			(((data >>  3) & 1) << 4) +
			(((data >>  9) & 1) << 5);

		bankaddress = 0x100000 + bankoffset[data];

		neogeo_set_cpu1_second_bank(bankaddress);
	}
}


READ16_HANDLER( kof2000_protection_r )
{
	switch (offset)
	{
	case 0x2fe446/2: return 0x9a37;
	case 0x2fffd8/2:
	case 0x2fffda/2: return sma_random_r();
	}
	return neogeo_cpu1_second_bank[offset];
}

WRITE16_HANDLER( kof2000_protection_w )
{
	if (offset == 0x2fffec/2)
	{
		/* thanks to Razoola and Mr K for the info */
		int bankaddress;
		static int bankoffset[64] =
		{
			0x000000, 0x100000, 0x200000, 0x300000, // 00
			0x3f7800, 0x4f7800, 0x3ff800, 0x4ff800, // 04
			0x407800, 0x507800, 0x40f800, 0x50f800, // 08
			0x416800, 0x516800, 0x41d800, 0x51d800, // 12
			0x424000, 0x524000, 0x523800, 0x623800, // 16
			0x526000, 0x626000, 0x528000, 0x628000, // 20
			0x52a000, 0x62a000, 0x52b800, 0x62b800, // 24
			0x52d000, 0x62d000, 0x52e800, 0x62e800, // 28
			0x618000, 0x619000, 0x61a000, 0x61a800, // 32
		};

		/* unscramble bank number */
		data =
			(((data >> 15) & 1) << 0) +
			(((data >> 14) & 1) << 1) +
			(((data >>  7) & 1) << 2) +
			(((data >>  3) & 1) << 3) +
			(((data >> 10) & 1) << 4) +
			(((data >>  5) & 1) << 5);

		bankaddress = 0x100000 + bankoffset[data];

		neogeo_set_cpu1_second_bank(bankaddress);
	}
}


/*------------------------------------------------------
  PVC Protection

  mslug5, svcchaos, kof2003
------------------------------------------------------*/

static uint16_t CartRAM[0x1000];
/*
static void pvc_w8(uint32_t offset, uint8_t data)
{
	*(((uint8_t *)CartRAM) + offset) = data;
}

static uint8_t pvc_r8(uint32_t offset)
{
	return *(((uint8_t *)CartRAM) + offset);
}
*/
static void pvc_write_unpack_color(void)
{
	uint16_t pen = CartRAM[0xff0];

	uint8_t b = ((pen & 0x000f) << 1) | ((pen & 0x1000) >> 12);
	uint8_t g = ((pen & 0x00f0) >> 3) | ((pen & 0x2000) >> 13);
	uint8_t r = ((pen & 0x0f00) >> 7) | ((pen & 0x4000) >> 14);
	uint8_t s = (pen & 0x8000) >> 15;

	CartRAM[0xff1] = (g << 8) | b;
	CartRAM[0xff2] = (s << 8) | r;
}
/*
// on writes to e8/e9/ea/eb
static void pvc_write_pack_color(void)
{
	uint8_t b1, b2, b3, b4;

	b1 = pvc_r8(0x1fe9);
	b2 = pvc_r8(0x1fe8);
	b3 = pvc_r8(0x1feb);
	b4 = pvc_r8(0x1fea);

	pvc_w8(0x1fec, (b2 >> 1) | ((b1 >> 1) << 4));
	pvc_w8(0x1fed, (b4 >> 1) | ((b2 & 1) << 4) | ((b1 & 1) << 5) | ((b4 & 1) << 6) | ((b3 & 1) << 7));
}
*/
static void pvc_write_pack_color(void)
{
	uint16_t gb = CartRAM[0xff4];
	uint16_t sr = CartRAM[0xff5];

	CartRAM[0xff6] = 	((gb & 0x001e) >>  1) |
						((gb & 0x1e00) >>  5) |
						((sr & 0x001e) <<  7) |
						((gb & 0x0001) << 12) |
						((gb & 0x0100) <<  5) |
						((sr & 0x0001) << 14) |
						((sr & 0x0100) <<  7);
}

static void pvc_write_bankswitch(void)
{
	uint32_t bankaddress;

	bankaddress = ((CartRAM[0xff8] >> 8) | (CartRAM[0xff9] << 8));
	CartRAM[0xff8] = (CartRAM[0xff8] & 0xfe00) | 0x00a0;//
	CartRAM[0xff9] &= 0x7fff;//
//	*(((uint8_t *)CartRAM) + 0x1ff0)  = 0xa0;
//	*(((uint8_t *)CartRAM) + 0x1ff1) &= 0xfe;
//	*(((uint8_t *)CartRAM) + 0x1ff3) &= 0x7f;

	neogeo_set_cpu1_second_bank(bankaddress + 0x100000);
}

READ16_HANDLER( pvc_protection_r )
{
	if (offset >= 0x2fe000/2)
	{
		offset -= 0x2fe000/2;
		return CartRAM[offset];
	}
	return neogeo_cpu1_second_bank[offset];
}

WRITE16_HANDLER( pvc_protection_w )
{
	if (offset >= 0x2fe000/2)
	{
		offset -= 0x2fe000/2;

		COMBINE_DATA(&CartRAM[offset]);

		if (offset == 0xff0)
			pvc_write_unpack_color();
		else if (offset >= 0xff4 && offset <= 0xff5)
			pvc_write_pack_color();
		else if (offset >= 0xff8)
			pvc_write_bankswitch();
	}
}


/*------------------------------------------------------
	Brezzasoft

	実際にはプロテクトではないがプロテクトとして処理
------------------------------------------------------*/

READ16_HANDLER( brza_sram_r )
{
	if (offset < 0x210000/2)
	{
		offset -= 0x200000/2;
		return CartRAM[offset & 0xfff];		//fix crash
	}
	return 0xffff;
}

WRITE16_HANDLER( brza_sram_w )
{
	if (offset < 0x210000/2)
	{
		offset -= 0x200000/2;
		COMBINE_DATA(&CartRAM[offset & 0xfff]);		//fix crash
	}
}

READ16_HANDLER( vliner_r )
{
	switch (offset & 0x2f0000/2)
	{
	case 0x200000/2: return CartRAM[offset & 0xfff];
	case 0x280000/2: return neogeo_port_value[4];
	case 0x2c0000/2: return 0x0003;
	}
	return 0xffff;
}

/************************ AES Protection************************
  To allow console mode
***************************************************************/

void kof2000_AES_protection(void)
{
	/* Patch out loop to disable console mode */
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;
	mem16[0xa226e >> 1] = 0x4e75;
//	uint8_t *mem8 = (uint8_t *)memory_region_cpu1;
//	mem8[0x109 >> 1] = 0x01;
}

void mslug5_AES_protection(void)
{
	/* Patch out loop to disable console mode */
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;
	mem16[0x122a >> 1] = 0x4e75;
}

void nitd_AES_protection(void)
{
	/* Patch out loop to disable console mode */
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;
	mem16[0x19978 >> 1] = 0x4e75;
}

void zupapa_AES_protection(void)
{
	/* Patch out loop to disable console mode */
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;
	mem16[0x80290 >> 1] = 0x4e71;
}

void sengoku3_AES_protection(void)
{
	/* Patch out loop to disable console mode */
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;
	mem16[0x00d04 >> 1] = 0x4e71;
}

void mslug4_AES_protection(void)
{
	/* Patch out loop to disable console mode */
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;
	mem16[0xad8c >> 1] = 0x4e75;
}

void rotd_AES_protection(void)
{
	/* Patch out loop to disable console mode */
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;
	mem16[0x1020 >> 1] = 0x4e71;
	mem16[0x2400 >> 1] = 0x4e71;
}

void matrim_AES_protection(void)
{
	/* Patch out loop to disable console mode */
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;
	mem16[0x1050 >> 1] = 0x4e75;
}

/*------------------------------------------------------
	bootleg set
------------------------------------------------------*/

#if !RELEASE

WRITE16_HANDLER( kof10th_protection_w )
{
	if (offset < 0x240000/2)
	{
		offset -= 0x200000/2;

		if (!CartRAM[0xffe])
		{
			uint16_t *prom = (uint16_t *)memory_region_cpu1;
			COMBINE_DATA(&prom[(0xe0000/2) + (offset & 0xffff)]);
		}
		else
		{
			uint8_t *srom1 = memory_region_user2;
			uint8_t *srom2 = memory_region_gfx2;
			uint32_t tile = offset & ~31;

			srom1[offset] = BITSWAP8(data,7,6,0,4,3,2,1,5);
			memcpy(&srom2[tile], &srom1[tile], 32);
			neogeo_decode_fix(&srom2[tile], 32, &gfx_pen_usage[1][tile >> 5]);
			blit_set_fix_clear_flag();
		}
	}
	else
	{
		offset -= 0x240000/2;

		if (offset >= 0x5f000)
		{
			if (offset == 0x5fff8)
			{
				uint32_t bankaddress = 0x100000 + ((data & 7) << 20);

				if (bankaddress >= 0x700000)
					bankaddress = 0x100000;

				neogeo_set_cpu1_second_bank(bankaddress);
			}
			else if (offset == 0x5fffc && CartRAM[0xffc] != data)
			{
				uint8_t *src = memory_region_cpu1;

				memcpy(src + 0x10000, src + ((data & 1) ? 0x810000 : 0x710000), 0xcffff);
			}
			COMBINE_DATA(&CartRAM[offset & 0xfff]);
		}
	}
}

WRITE16_HANDLER( cthd2003_protection_w )
{
	if (offset >= 0x2ffff0/2)
	{
		uint8_t cthd2003_banks[8] = { 1,0,1,0,1,0,3,2 };

		offset -= 0x2ffff0/2;

		if (offset == 0)
		{
			neogeo_set_cpu1_second_bank(0x100000 + (cthd2003_banks[data & 7] << 20));
		}
	}
}

READ16_HANDLER( ms5plus_protection_r )
{
	if (offset >= 0x2ffff0/2)
	{
		return 0xa0;
	}
	return neogeo_cpu1_second_bank[offset];
}

WRITE16_HANDLER( ms5plus_protection_w )
{
	if (offset == 0x2ffff0/2 && data == 0xa0)
	{
		neogeo_set_cpu1_second_bank(0xa0);
	}
	else if (offset == 0x2ffff4/2)
	{
		data >>= 4;
		neogeo_set_cpu1_second_bank(data << 20);
	}
}

WRITE16_HANDLER( kf2k3bl_protection_w)
{
	if (offset >= 0x2fe000/2)
	{
		offset -= 0x2fe000/2;

		data = COMBINE_DATA(&CartRAM[offset]);

		if (offset == 0x1ff0/2 || offset == 0x1ff2/2)
		{
			uint8_t *cr = (uint8_t *)CartRAM;
			uint32_t address = (cr[0x1ff3] << 16) | (cr[0x1ff2] << 8) | cr[0x1ff1];
			uint8_t prt = cr[0x1ff2];

			cr[0x1ff0] =  0xa0;
			cr[0x1ff1] &= 0xfe;
			cr[0x1ff3] &= 0x7f;
			neogeo_set_cpu1_second_bank(address + 0x100000);

			memory_region_cpu1[0x58196] = prt;
		}
	}
}

WRITE16_HANDLER( kf2k3pl_protection_w)
{
	if (offset >= 0x2fe000/2)
	{
		offset -= 0x2fe000/2;

		data = COMBINE_DATA(&CartRAM[offset]);

		if (offset == 0x1ff0/2 || offset == 0x1ff2/2)
		{
			uint8_t *cr = (uint8_t *)CartRAM;
			uint32_t address = (cr[0x1ff3] << 16) | (cr[0x1ff2] << 8) | cr[0x1ff0];
			uint8_t prt = cr[0x1ff2];

			cr[0x1ff0] &= 0xfe;
			cr[0x1ff3] &= 0x7f;
			neogeo_set_cpu1_second_bank(address + 0x100000);

			memory_region_cpu1[0x58196] = prt;
		}
	}
}

void cthd2003_AES_protection(void)
{
	uint16_t *mem16 = (uint16_t *)memory_region_cpu1;

	// Game sets itself to MVS & English mode, patch this out
	mem16[0xed00e/2] = 0x4e71;
	mem16[0xed394/2] = 0x4e71;

	// Fix for AES mode (stop loop that triggers Watchdog)
	mem16[0xa2b7e/2] = 0x4e71;
}
#endif /* RELEASE */


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( driver )
{
	state_save_long(&neogeo_driver_type, 1);
	state_save_long(&raster_enable, 1);

	state_save_long(&scanline_read, 1);
	state_save_word(&raster_counter, 1);

	state_save_long(&display_counter, 1);
	state_save_long(&display_position_interrupt_counter, 1);
	state_save_long(&display_position_interrupt_control, 1);
	state_save_long(&display_position_interrupt_pending, 1);
	state_save_long(&vblank_interrupt_pending, 1);

	state_save_long(&sound_code, 1);
	state_save_long(&result_code, 1);
	state_save_long(&pending_command, 1);

	state_save_byte(&auto_animation_speed, 1);
	state_save_byte(&auto_animation_disabled, 1);
	state_save_byte(&auto_animation_counter, 1);
	state_save_byte(&auto_animation_frame_counter, 1);

	state_save_byte(&main_cpu_vector_table_source, 1);
	state_save_byte(&controller_select, 1);
	state_save_byte(&save_ram_unlocked, 1);

	state_save_long(&m68k_second_bank, 1);
	state_save_long(z80_bank, 4);

	state_save_long(&neogeo_prot_data, 1);
	state_save_word(&neogeo_rng, 1);
	state_save_word(&CartRAM, 0x1000);
}

STATE_LOAD( driver )
{
	uint32_t _m68k_second_bank;
	uint32_t _z80_bank[4];

	state_load_long(&neogeo_driver_type, 1);
	state_load_long(&raster_enable, 1);

	state_load_long(&scanline_read, 1);
	state_load_word(&raster_counter, 1);

	state_load_long(&display_counter, 1);
	state_load_long(&display_position_interrupt_counter, 1);
	state_load_long(&display_position_interrupt_control, 1);
	state_load_long(&display_position_interrupt_pending, 1);
	state_load_long(&vblank_interrupt_pending, 1);

	state_load_long(&sound_code, 1);
	state_load_long(&result_code, 1);
	state_load_long(&pending_command, 1);

	state_load_byte(&auto_animation_counter, 1);
	state_load_byte(&auto_animation_speed, 1);
	state_load_byte(&auto_animation_disabled, 1);
	state_load_byte(&auto_animation_frame_counter, 1);

	state_load_byte(&main_cpu_vector_table_source, 1);
	state_load_byte(&controller_select, 1);
	state_load_byte(&save_ram_unlocked, 1);

	state_load_long(&_m68k_second_bank, 1);
	state_load_long(_z80_bank, 4);

	state_load_long(&neogeo_prot_data, 1);
	state_load_word(&neogeo_rng, 1);
	state_load_word(&CartRAM, 0x1000);

	neogeo_set_cpu1_second_bank(_m68k_second_bank);
	neogeo_set_cpu2_bank(0, _z80_bank[0]);
	neogeo_set_cpu2_bank(1, _z80_bank[1]);
	neogeo_set_cpu2_bank(2, _z80_bank[2]);
	neogeo_set_cpu2_bank(3, _z80_bank[3]);
	set_main_cpu_vector_table_source(main_cpu_vector_table_source);
}

#endif /* STATE_SAVE */

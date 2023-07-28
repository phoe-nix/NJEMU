/******************************************************************************

	memintrf.c

	CPS1メモリインタフェース関数

******************************************************************************/

#include <fcntl.h>
#include "cps1.h"


#define M68K_AMASK 0x00ffffff
#define Z80_AMASK 0x0000ffff

#define READ_BYTE(mem, offset)			mem[offset ^ 1]
#define READ_WORD(mem, offset)			*(uint16_t *)&mem[offset]
#define WRITE_BYTE(mem, offset, data)	mem[offset ^ 1] = data
#define WRITE_WORD(mem, offset, data)	*(uint16_t *)&mem[offset] = data

#define str_cmp(s1, s2)		strncasecmp(s1, s2, strlen(s2))

enum
{
	REGION_CPU1 = 0,
	REGION_CPU2,
	REGION_GFX1,
	REGION_SOUND1,
	REGION_USER1,
	REGION_SKIP
};

#define MAX_CPU1ROM		16
#define MAX_CPU2ROM		8
#define MAX_GFX1ROM		32
#define MAX_SND1ROM		16


/******************************************************************************
	グローバル構造体/変数
******************************************************************************/

uint8_t *memory_region_cpu1;
uint8_t *memory_region_cpu2;
uint8_t *memory_region_gfx1;
uint8_t *memory_region_sound1;
uint8_t *memory_region_user1;
uint8_t *memory_region_user2;

uint32_t memory_length_cpu1;
uint32_t memory_length_cpu2;
uint32_t memory_length_gfx1;
uint32_t memory_length_sound1;
uint32_t memory_length_user1;
uint32_t memory_length_user2;

uint8_t  ALIGN_DATA cps1_ram[0x10000];
uint16_t ALIGN_DATA cps1_gfxram[0x30000 >> 1];
uint16_t ALIGN_DATA cps1_output[0x100 >> 1];

uint8_t *qsound_sharedram1;
uint8_t *qsound_sharedram2;


/******************************************************************************
	ローカル構造体/変数
******************************************************************************/

static struct rom_t cpu1rom[MAX_CPU1ROM];
static struct rom_t cpu2rom[MAX_CPU2ROM];
static struct rom_t gfx1rom[MAX_GFX1ROM];
static struct rom_t snd1rom[MAX_SND1ROM];

static int num_cpu1rom;
static int num_cpu2rom;
static int num_gfx1rom;
static int num_snd1rom;

static uint8_t *static_ram1;
static uint8_t *static_ram2;


/******************************************************************************
	プロトタイプ
******************************************************************************/

uint8_t (*z80_read_memory_8)(uint32_t offset);
void (*z80_write_memory_8)(uint32_t offset, uint8_t data);

static uint8_t cps1_sound_readmem(uint32_t offset);
static void cps1_sound_writemem(uint32_t offset, uint8_t data);

static uint8_t cps1_qsound_readmem(uint32_t offset);
static void cps1_qsound_writemem(uint32_t offset, uint8_t data);

#if !RELEASE
static uint8_t cps1_kodb_readmem(uint32_t offset);
static void cps1_kodb_writemem(uint32_t offset, uint8_t data);
#endif


/******************************************************************************
	ROM読み込み
******************************************************************************/

/*--------------------------------------------------------
	CPU1 (M68000 program ROM / encrypted)
--------------------------------------------------------*/

static int load_rom_cpu1(void)
{
	int i, res;
	char fname[32], *parent;

	if ((memory_region_cpu1 = memalign(MEM_ALIGN, memory_length_cpu1)) == NULL)
	{
		error_memory("REGION_CPU1");
		return 0;
	}
	memset(memory_region_cpu1, 0, memory_length_cpu1);

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_cpu1rom; )
	{
		strcpy(fname, cpu1rom[i].name);
		if ((res = file_open(game_name, parent, cpu1rom[i].crc, fname)) < 0)
		{
			if (res == -2)
				error_crc(fname);
			else
				error_file(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		i = rom_load(cpu1rom, memory_region_cpu1, i, num_cpu1rom);

		file_close();
	}

	return 1;
}


/*--------------------------------------------------------
	CPU2 (Z80 program ROM)
--------------------------------------------------------*/

static int load_rom_cpu2(void)
{
	int i, res;
	char fname[32], *parent;

	if ((memory_region_cpu2 = memalign(MEM_ALIGN, memory_length_cpu2)) == NULL)
	{
		error_memory("REGION_CPU2");
		return 0;
	}
	memset(memory_region_cpu2, 0, memory_length_cpu2);

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_cpu2rom; )
	{
		strcpy(fname, cpu2rom[i].name);
		if ((res = file_open(game_name, parent, cpu2rom[i].crc, fname)) < 0)
		{
			if (res == -2)
				error_crc(fname);
			else
				error_file(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		i = rom_load(cpu2rom, memory_region_cpu2, i, num_cpu2rom);

		file_close();
	}

	return 1;
}


/*--------------------------------------------------------
	GFX1 (graphic ROM)
--------------------------------------------------------*/

static int load_rom_gfx1(void)
{
	int i, res;
	char fname[32], *parent;

	if ((memory_region_gfx1 = memalign(MEM_ALIGN, memory_length_gfx1)) == NULL)
	{
		error_memory("REGION_GFX1");
		return 0;
	}
	memset(memory_region_gfx1, 0, memory_length_gfx1);

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_gfx1rom; )
	{
		strcpy(fname, gfx1rom[i].name);
		if ((res = file_open(game_name, parent, gfx1rom[i].crc, fname)) < 0)
		{
			if (res == -2)
				error_crc(fname);
			else
				error_file(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		i = rom_load(gfx1rom, memory_region_gfx1, i, num_gfx1rom);

		file_close();
	}

	return 1;
}


/*--------------------------------------------------------
	SOUND1 (OKI6295 ADPCM / Q-SOUND PCM ROM)
--------------------------------------------------------*/

static int load_rom_sound1(void)
{
	int i, res;
	char fname[32], *parent;

	if (!memory_length_sound1) return 1;

	if ((memory_region_sound1 = memalign(MEM_ALIGN, memory_length_sound1)) == NULL)
	{
		error_memory("REGION_SOUND1");
		return 0;
	}
	memset(memory_region_sound1, 0, memory_length_sound1);

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_snd1rom; )
	{
		strcpy(fname, snd1rom[i].name);
		if ((res = file_open(game_name, parent, snd1rom[i].crc, fname)) < 0)
		{
			if (res == -2)
				error_crc(fname);
			else
				error_file(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		i = rom_load(snd1rom, memory_region_sound1, i, num_snd1rom);

		file_close();
	}

	return 1;
}


/*--------------------------------------------------------
	USER1 (Encrypted CPU2 ROM)
--------------------------------------------------------*/

static int load_rom_user1(void)
{
	if (memory_length_user1 == 0) return 1;

	if ((memory_region_user1 = memalign(MEM_ALIGN, memory_length_user1)) == NULL)
	{
		error_memory("REGION_USER1");
		return 0;
	}
	memset(memory_region_user1, 0, memory_length_user1);

	memcpy(memory_region_user1, memory_region_cpu2, 0x8000);

	return 1;
}


/*--------------------------------------------------------
	ROM情報をデータベースで解析
--------------------------------------------------------*/

static int load_rom_info(const char *game_name)
{
	int32_t fd;
	char path[MAX_PATH];
	char *buf;
	char linebuf[256];
	int i, size;
	int rom_start = 0;
	int region = 0;

	num_cpu1rom = 0;
	num_cpu2rom = 0;
	num_gfx1rom = 0;
	num_snd1rom = 0;

	machine_driver_type = 0;
	machine_input_type   = 0;
	machine_init_type    = 0;
	machine_screen_type  = 0;

	sprintf(path, "%srominfo.cps1", launchDir);

	if ((fd = open(path, O_RDONLY, 0777)) >= 0)
	{
		size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);

		if ((buf = (char *)malloc(size)) == NULL)
		{
			close(fd);
			return 3;	// 手抜き
		}

		read(fd, buf, size);
		close(fd);

		i = 0;
		while (i < size)
		{
			char *p = &buf[i];

			while (buf[i] != '\n' && buf[i] != EOF)
				i++;

			buf[i++] = '\0';

			strcpy(linebuf, p);
			strcat(linebuf, "\n");

			if (linebuf[0] == '/' && linebuf[1] == '/')
				continue;

			if (linebuf[0] != '\t')
			{
				if (linebuf[0] == '\r' || linebuf[0] == '\n')
				{
					// 改行
					continue;
				}
				else if (str_cmp(linebuf, "FILENAME(") == 0)
				{
					char *name, *parent;
					char *machine, *input, *init, *rotate;

					strtok(linebuf, " ");
					name    = strtok(NULL, " ,");
					parent  = strtok(NULL, " ,");
					machine = strtok(NULL, " ,");
					input   = strtok(NULL, " ,");
					init    = strtok(NULL, " ,");
					rotate  = strtok(NULL, " ");

					if (strcasecmp(name, game_name) == 0)
					{
						if (str_cmp(parent, "cps1") == 0)
							parent_name[0] = '\0';
						else
							strcpy(parent_name, parent);

						sscanf(machine, "%d", &machine_driver_type);
						sscanf(input, "%d", &machine_input_type);
						sscanf(init, "%d", &machine_init_type);
						sscanf(rotate, "%d", &machine_screen_type);
						rom_start = 1;
					}
				}
				else if (rom_start && str_cmp(linebuf, "END") == 0)
				{
					free(buf);
					return 0;
				}
			}
			else if (rom_start)
			{
				if (str_cmp(&linebuf[1], "REGION(") == 0)
				{
					char *size, *type, *flag;

					strtok(&linebuf[1], " ");
					size = strtok(NULL, " ,");
					type = strtok(NULL, " ,");
					flag = strtok(NULL, " ");

					if (strcmp(type, "CPU1") == 0)
					{
						sscanf(size, "%x", &memory_length_cpu1);
						region = REGION_CPU1;
					}
					else if (strcmp(type, "CPU2") == 0)
					{
						sscanf(size, "%x", &memory_length_cpu2);
						region = REGION_CPU2;
					}
					else if (strcmp(type, "GFX1") == 0)
					{
						sscanf(size, "%x", &memory_length_gfx1);
						region = REGION_GFX1;
					}
					else if (strcmp(type, "SOUND1") == 0)
					{
						sscanf(size, "%x", &memory_length_sound1);
						region = REGION_SOUND1;
					}
					else if (strcmp(type, "USER1") == 0)
					{
						sscanf(size, "%x", &memory_length_user1);
						region = REGION_USER1;
					}
					else
					{
						region = REGION_SKIP;
					}
				}
				else if (str_cmp(&linebuf[1], "ROM(") == 0)
				{
					char *type, *name, *offset, *length, *crc;

					strtok(&linebuf[1], " ");
					type   = strtok(NULL, " ,");
					if (type[0] != '1')
						name = strtok(NULL, " ,");
					else
						name = NULL;
					offset = strtok(NULL, " ,");
					length = strtok(NULL, " ,");
					crc    = strtok(NULL, " ");

					switch (region)
					{
					case REGION_CPU1:
						sscanf(type, "%x", &cpu1rom[num_cpu1rom].type);
						sscanf(offset, "%x", &cpu1rom[num_cpu1rom].offset);
						sscanf(length, "%x", &cpu1rom[num_cpu1rom].length);
						sscanf(crc, "%x", &cpu1rom[num_cpu1rom].crc);
						if (name) strcpy(cpu1rom[num_cpu1rom].name, name);
						cpu1rom[num_cpu1rom].group = 0;
						cpu1rom[num_cpu1rom].skip = 0;
						num_cpu1rom++;
						break;

					case REGION_CPU2:
						sscanf(type, "%x", &cpu2rom[num_cpu2rom].type);
						sscanf(offset, "%x", &cpu2rom[num_cpu2rom].offset);
						sscanf(length, "%x", &cpu2rom[num_cpu2rom].length);
						sscanf(crc, "%x", &cpu2rom[num_cpu2rom].crc);
						if (name) strcpy(cpu2rom[num_cpu2rom].name, name);
						cpu2rom[num_cpu2rom].group = 0;
						cpu2rom[num_cpu2rom].skip = 0;
						num_cpu2rom++;
						break;

					case REGION_GFX1:
						sscanf(type, "%x", &gfx1rom[num_gfx1rom].type);
						sscanf(offset, "%x", &gfx1rom[num_gfx1rom].offset);
						sscanf(length, "%x", &gfx1rom[num_gfx1rom].length);
						sscanf(crc, "%x", &gfx1rom[num_gfx1rom].crc);
						if (name) strcpy(gfx1rom[num_gfx1rom].name, name);
						gfx1rom[num_gfx1rom].group = 0;
						gfx1rom[num_gfx1rom].skip = 0;
						num_gfx1rom++;
						break;

					case REGION_SOUND1:
						sscanf(type, "%x", &snd1rom[num_snd1rom].type);
						sscanf(offset, "%x", &snd1rom[num_snd1rom].offset);
						sscanf(length, "%x", &snd1rom[num_snd1rom].length);
						sscanf(crc, "%x", &snd1rom[num_snd1rom].crc);
						if (name) strcpy(snd1rom[num_snd1rom].name, name);
						snd1rom[num_snd1rom].group = 0;
						snd1rom[num_snd1rom].skip = 0;
						num_snd1rom++;
						break;
					}
				}
				else if (str_cmp(&linebuf[1], "ROMX(") == 0)
				{
					char *type, *name, *offset, *length, *crc;
					char *group, *skip;

					strtok(&linebuf[1], " ");
					type   = strtok(NULL, " ,");
					if (type[0] != '1')
						name = strtok(NULL, " ,");
					else
						name = NULL;
					offset = strtok(NULL, " ,");
					length = strtok(NULL, " ,");
					crc    = strtok(NULL, " ,");
					group  = strtok(NULL, " ,");
					skip   = strtok(NULL, " ");

					switch (region)
					{
					case REGION_CPU1:
						sscanf(type, "%x", &cpu1rom[num_cpu1rom].type);
						sscanf(offset, "%x", &cpu1rom[num_cpu1rom].offset);
						sscanf(length, "%x", &cpu1rom[num_cpu1rom].length);
						sscanf(crc, "%x", &cpu1rom[num_cpu1rom].crc);
						sscanf(group, "%x", &cpu1rom[num_cpu1rom].group);
						sscanf(skip, "%x", &cpu1rom[num_cpu1rom].skip);
						if (name) strcpy(cpu1rom[num_cpu1rom].name, name);
						num_cpu1rom++;
						break;

					case REGION_GFX1:
						sscanf(type, "%x", &gfx1rom[num_gfx1rom].type);
						sscanf(offset, "%x", &gfx1rom[num_gfx1rom].offset);
						sscanf(length, "%x", &gfx1rom[num_gfx1rom].length);
						sscanf(crc, "%x", &gfx1rom[num_gfx1rom].crc);
						sscanf(group, "%x", &gfx1rom[num_gfx1rom].group);
						sscanf(skip, "%x", &gfx1rom[num_gfx1rom].skip);
						if (name) strcpy(gfx1rom[num_gfx1rom].name, name);
						num_gfx1rom++;
						break;
					}
				}
			}
		}
		free(buf);
		return 2;
	}
	return 3;
}


/******************************************************************************
	メモリインタフェース関数
******************************************************************************/

/*------------------------------------------------------
	メモリインタフェース初期化
------------------------------------------------------*/

int memory_init(void)
{
	int i, res;

	memory_region_cpu1   = NULL;
	memory_region_cpu2   = NULL;
	memory_region_gfx1   = NULL;
	memory_region_sound1 = NULL;
	memory_region_user1  = NULL;
	memory_region_user2  = NULL;

	memory_length_cpu1   = 0;
	memory_length_cpu2   = 0;
	memory_length_gfx1   = 0;
	memory_length_sound1 = 0;
	memory_length_user1  = 0;
	memory_length_user2  = 0;

	pad_wait_clear();
	video_clear_screen();
	msg_screen_init(WP_LOGO, ICON_SYSTEM, TEXT(LOAD_ROM));

	msg_printf(TEXT(CHECKING_ROM_INFO));

	if ((res = load_rom_info(game_name)) != 0)
	{
		switch (res)
		{
		case 1: msg_printf(TEXT(THIS_GAME_NOT_SUPPORTED)); break;
		case 2: msg_printf(TEXT(ROM_NOT_FOUND)); break;
		case 3: msg_printf(TEXT(ROMINFO_NOT_FOUND)); break;
		}
		msg_printf(TEXT(PRESS_ANY_BUTTON2));
		pad_wait_press(PAD_WAIT_INFINITY);
		Loop = LOOP_BROWSER;
		return 0;
	}

	i = 0;
	driver = NULL;
	while (CPS1_driver[i].name)
	{
		if (!strcmp(game_name, CPS1_driver[i].name))
		{
			driver = &CPS1_driver[i];
			break;
		}
		i++;
	}
	if (!driver)
	{
		msg_printf(TEXT(DRIVER_FOR_x_NOT_FOUND), game_name);
		msg_printf(TEXT(PRESS_ANY_BUTTON2));
		pad_wait_press(PAD_WAIT_INFINITY);
		Loop = LOOP_BROWSER;
		return 0;
	}

	if (parent_name[0])
		msg_printf(TEXT(ROMSET_x_PARENT_x), game_name, parent_name);
	else
		msg_printf(TEXT(ROMSET_x), game_name);

	load_gamecfg(game_name);
#ifdef ADHOC
	if (adhoc_enable)
	{
		/* AdHoc通信時は一部オプションで固定の設定を使用 */
		cps_raster_enable    = 1;
		psp_cpuclock         = PSPCLOCK_333;
		option_vsync         = 0;
		option_autoframeskip = 0;
		option_frameskip     = 0;
		option_showfps       = 0;
		option_speedlimit    = 1;
		option_sound_enable  = 1;
		option_samplerate    = 0;
	}
	else
#endif
	{
#ifdef COMMAND_LIST
		if (parent_name[0])
			load_commandlist(game_name, parent_name);
		else
			load_commandlist(game_name, NULL);
#endif
	}

	set_cpu_clock(psp_cpuclock);

	if (load_rom_cpu1() == 0) return 0;
	if (load_rom_cpu2() == 0) return 0;
	if (load_rom_gfx1() == 0) return 0;
	if (load_rom_sound1() == 0) return 0;
	if (load_rom_user1() == 0) return 0;

	static_ram1 = (uint8_t *)cps1_ram - 0xff0000;
	static_ram2 = (uint8_t *)cps1_gfxram - 0x900000;

	qsound_sharedram1 = &memory_region_cpu2[0xc000];
	qsound_sharedram2 = &memory_region_cpu2[0xf000];

	memset(cps1_ram, 0, sizeof(cps1_ram));
	memset(cps1_gfxram, 0, sizeof(cps1_gfxram));
	memset(cps1_output, 0, sizeof(cps1_output));

	if (machine_driver_type == MACHINE_qsound)
	{
		machine_sound_type = SOUND_QSOUND;
		z80_read_memory_8 = cps1_qsound_readmem;
		z80_write_memory_8 = cps1_qsound_writemem;
		memory_length_user2 = 0x8000;

		if ((memory_region_user2 = (uint8_t *)memalign(MEM_ALIGN, 0x8000)) == NULL)
		{
			fatalerror(TEXT(COULD_NOT_ALLOCATE_MEMORY_0x8000BYTE));
			return 0;
		}
		memset(memory_region_user2, 0, 0x8000);
	}
	else if (machine_driver_type == MACHINE_wofhfh)
	{
		machine_sound_type = SOUND_YM2151_CPS1;
		z80_read_memory_8 = cps1_sound_readmem;
		z80_write_memory_8 = cps1_sound_writemem;

		memory_region_user2 = memory_region_cpu2;
	}
	else
	{
		machine_sound_type = SOUND_YM2151_CPS1;
#if !RELEASE
		if (machine_init_type == INIT_kodb)
		{
			z80_read_memory_8 = cps1_kodb_readmem;
			z80_write_memory_8 = cps1_kodb_writemem;
		}
		else
#endif
		{
			z80_read_memory_8 = cps1_sound_readmem;
			z80_write_memory_8 = cps1_sound_writemem;
		}
		memory_region_user2 = memory_region_cpu2;
	}

	switch (machine_init_type)
	{
	case INIT_wof:      wof_decode();      break;
	case INIT_dino:     dino_decode();     break;
	case INIT_punisher: punisher_decode(); break;
	case INIT_slammast: slammast_decode(); break;
	case INIT_pang3:    pang3_decode();    break;
#if !RELEASE
	case INIT_kodb:     kodb_init();       break;
	case INIT_sf2m13:   sf2m13_init();     break;
	case INIT_wofh:     wofh_init();       break;
	case INIT_wof3js:   wof3js_init();     break;
	case INIT_wof3sj:   wof3sj_init();     break;
	case INIT_wofsjb:   wofsjb_init();     break;
	case INIT_dinoh:    dino_decode();     break;
	case INIT_dinob:    dinob_init();      break;
	case INIT_dinohunt: dinohunt_init();   break;
	case INIT_sf2m3:    sf2m3_init();      break;
	case INIT_wofb:     wofb_init();       break;
	case INIT_wofch3p:  wofch3p_init();    break;
#endif
	}

	return 1;
}


/*------------------------------------------------------
	メモリインタフェース終了
------------------------------------------------------*/

void memory_shutdown(void)
{
	if (memory_region_cpu1)   free(memory_region_cpu1);
	if (memory_region_cpu2)   free(memory_region_cpu2);
	if (memory_region_gfx1)   free(memory_region_gfx1);
	if (memory_region_sound1) free(memory_region_sound1);
	if (memory_region_user1)  free(memory_region_user1);
	if (memory_length_user2)  free(memory_region_user2);
}


/******************************************************************************
	M68000 メモリリード/ライト関数
******************************************************************************/

/*------------------------------------------------------
	M68000メモリリード (byte)
------------------------------------------------------*/

uint8_t m68000_read_memory_8(uint32_t offset)
{
	int shift;
	uint16_t mem_mask;

	offset &= M68K_AMASK;

	if (offset < 0x200000)
	{
		return READ_BYTE(memory_region_cpu1, offset);
	}

	shift = (~offset & 1) << 3;
	mem_mask = ~(0xff << shift);

	switch (offset >> 16)
	{
	case 0xff:
		return READ_BYTE(static_ram1, offset);

	case 0x90:
	case 0x91:
	case 0x92:
		return READ_BYTE(static_ram2, offset);

	case 0xf0:
		return qsound_rom_r(offset >> 1, mem_mask) >> shift;

	case 0x80:
		switch (offset & 0xffe)
		{
		case 0x000: return cps1_inputport1_r(0, mem_mask) >> shift;
		case 0x010: return cps1_inputport1_r(0, mem_mask) >> shift;
		case 0x018: return cps1_inputport0_r(0, mem_mask) >> shift;
		case 0x01a: return cps1_dsw_a_r(0, mem_mask) >> shift;
		case 0x01c: return cps1_dsw_b_r(0, mem_mask) >> shift;
		case 0x01e: return cps1_dsw_c_r(0, mem_mask) >> shift;
		case 0x020: return 0;
		case 0x052: return forgottn_dial_0_r(0, mem_mask) >> shift;
		case 0x054: return forgottn_dial_0_r(1, mem_mask) >> shift;
		case 0x05a: return forgottn_dial_1_r(0, mem_mask) >> shift;
		case 0x05c: return forgottn_dial_1_r(1, mem_mask) >> shift;
		case 0x176: return cps1_inputport2_r(0, mem_mask) >> shift;
		case 0x178: return cps1_inputport3_r(0, mem_mask) >> shift;
		case 0x1fc: return cps1_inputport2_r(0, mem_mask) >> shift;
		default:
			if (offset >= 0x800100 && offset <= 0x8001ff)
			{
				return cps1_output_r(offset >> 1, mem_mask) >> shift;
			}
			break;
		}
		break;

#if !RELEASE
	case 0x88:
		switch (offset & 0xfffe)
		{
		case 0x0000: return cps1_inputport1_r(0, mem_mask) >> shift;
		case 0x0006:
		case 0x0008: return cps1_inputport0_r(0, mem_mask) >> shift;
		}
		break;
#endif

	case 0xf1:
		switch (offset & 0xf000)
		{
		case 0x8000:
		case 0x9000:
			return qsound_sharedram1_r(offset >> 1, mem_mask) >> shift;

		case 0xe000:
		case 0xf000:
			return qsound_sharedram2_r(offset >> 1, mem_mask) >> shift;

		case 0xc000:
			switch (offset & 0xffe)
			{
			case 0x00: return cps1_inputport2_r(0, mem_mask) >> shift;
			case 0x02: return cps1_inputport3_r(0, mem_mask) >> shift;
			case 0x06: return cps1_eeprom_port_r(offset >> 1, mem_mask) >> shift;
			}
			break;
		}
		break;
	}

	return 0xff;
}


/*------------------------------------------------------
	M68000リードメモリ (word)
------------------------------------------------------*/

uint16_t m68000_read_memory_16(uint32_t offset)
{
	offset &= M68K_AMASK;

	if (offset < 0x200000)
	{
		return READ_WORD(memory_region_cpu1, offset);
	}

	switch (offset >> 16)
	{
	case 0xff:
		return READ_WORD(static_ram1, offset);

	case 0x90:
	case 0x91:
	case 0x92:
		return READ_WORD(static_ram2, offset);

	case 0xf0:
		return qsound_rom_r(offset >> 1, 0);

	case 0x80:
		switch (offset & 0xffe)
		{
		case 0x000: return cps1_inputport1_r(0, 0);
		case 0x010: return cps1_inputport1_r(0, 0);
		case 0x018: return cps1_inputport0_r(0, 0);
		case 0x01a: return cps1_dsw_a_r(0, 0);
		case 0x01c: return cps1_dsw_b_r(0, 0);
		case 0x01e: return cps1_dsw_c_r(0, 0);
		case 0x020: return 0;
		case 0x052: return forgottn_dial_0_r(0, 0);
		case 0x054: return forgottn_dial_0_r(1, 0);
		case 0x05a: return forgottn_dial_1_r(0, 0);
		case 0x05c: return forgottn_dial_1_r(1, 0);
		case 0x176: return cps1_inputport2_r(0, 0);
		case 0x178: return cps1_inputport3_r(0, 0);
		case 0x1fc: return cps1_inputport2_r(0, 0);
		default:
			if (offset >= 0x800100 && offset <= 0x8001ff)
			{
				return cps1_output_r(offset >> 1, 0);
			}
			break;
		}
		break;

#if !RELEASE
	case 0x88:
		switch (offset & 0xfffe)
		{
		case 0x0000: return cps1_inputport1_r(0, 0);
		case 0x0006:
		case 0x0008: return cps1_inputport0_r(0, 0);
		}
		break;
#endif

	case 0xf1:
		switch (offset & 0xf000)
		{
		case 0x8000:
		case 0x9000:
			return qsound_sharedram1_r(offset >> 1, 0);

		case 0xe000:
		case 0xf000:
			return qsound_sharedram2_r(offset >> 1, 0);

		case 0xc000:
			switch (offset & 0xffe)
			{
			case 0x000: return cps1_inputport2_r(0, 0);
			case 0x002: return cps1_inputport3_r(0, 0);
			case 0x006: return cps1_eeprom_port_r(offset >> 1, 0);
			}
			break;
		}
		break;
	}

	return 0xffff;
}


/*------------------------------------------------------
	M68000ライトメモリ (byte)
------------------------------------------------------*/

void m68000_write_memory_8(uint32_t offset, uint8_t data)
{
	int shift = (~offset & 1) << 3;
	uint16_t mem_mask = ~(0xff << shift);

	offset &= M68K_AMASK;

	switch (offset >> 16)
	{
	case 0xff:
		WRITE_BYTE(static_ram1, offset, data);
		return;

	case 0x90:
	case 0x91:
	case 0x92:
		WRITE_BYTE(static_ram2, offset, data);
		return;

	case 0x80:
		switch (offset & 0xffe)
		{
		case 0x030: cps1_coinctrl_w(offset >> 1, data << shift, mem_mask); return;
		case 0x040: forgottn_dial_0_reset_w(0, 0, mem_mask); return;
		case 0x048: forgottn_dial_1_reset_w(0, 0, mem_mask); return;
		case 0x180: cps1_sound_command_w(offset >> 1, data << shift, mem_mask); return;
		case 0x188: cps1_sound_fade_timer_w(offset >> 1, data << shift, mem_mask); return;
		default:
			if (offset >= 0x800100 && offset <= 0x8001ff)
			{
				cps1_output_w(offset >> 1, data << shift, mem_mask);
				return;
			}
			break;
		}
		break;

#if !RELEASE
	case 0x88:
		switch (offset & 0xfffe)
		{
		case 0x0006:
		case 0x0008: cps1_output_w(offset >> 1, data << shift, mem_mask); return;
		}
		break;
#endif

	case 0xf1:
		switch (offset & 0xf000)
		{
		case 0x8000:
		case 0x9000:
			qsound_sharedram1_w(offset >> 1, data << shift, mem_mask);
			return;

		case 0xe000:
		case 0xf000:
			qsound_sharedram2_w(offset >> 1, data << shift, mem_mask);
			return;

		case 0xc000:
			switch (offset & 0xffe)
			{
			case 0x004: cps1_coinctrl2_w(offset >> 1, data << shift, mem_mask); return;
			case 0x006: cps1_eeprom_port_w(offset >> 1, data << shift, mem_mask); return;
			}
			break;
		}
		break;
	}
}


/*------------------------------------------------------
	M68000ライトメモリ (word)
------------------------------------------------------*/

void m68000_write_memory_16(uint32_t offset, uint16_t data)
{
	offset &= M68K_AMASK;

	switch (offset >> 16)
	{
	case 0x90:
	case 0x91:
	case 0x92:
		WRITE_WORD(static_ram2, offset, data);
		return;

	case 0xff:
		WRITE_WORD(static_ram1, offset, data);
		return;

	case 0x80:
		switch (offset & 0xffe)
		{
		case 0x030: cps1_coinctrl_w(offset >> 1, data, 0); return;
		case 0x040: forgottn_dial_0_reset_w(0, 0, 0); return;
		case 0x048: forgottn_dial_1_reset_w(0, 0, 0); return;
		case 0x180: cps1_sound_command_w(offset >> 1, data, 0); return;
		case 0x188: cps1_sound_fade_timer_w(offset >> 1, data, 0); return;
		default:
			if (offset >= 0x800100 && offset <= 0x8001ff)
			{
				cps1_output_w(offset >> 1, data, 0);
				return;
			}
			break;
		}
		break;

#if !RELEASE
	case 0x88:
		switch (offset & 0xfffe)
		{
		case 0x0006:
		case 0x0008: cps1_output_w(offset >> 1, data, 0); return;
		}
		break;
#endif

	case 0xf1:
		switch (offset & 0xf000)
		{
		case 0x8000:
		case 0x9000:
			qsound_sharedram1_w(offset >> 1, data, 0);
			return;

		case 0xe000:
		case 0xf000:
			qsound_sharedram2_w(offset >> 1, data, 0);
			return;

		case 0xc000:
			switch (offset & 0xffe)
			{
			case 0x004: cps1_coinctrl2_w(offset >> 1, data, 0); return;
			case 0x006: cps1_eeprom_port_w(offset >> 1, data, 0); return;
			}
			break;
		}
		break;
	}
}


/******************************************************************************
	Z80 メモリリード/ライト関数
******************************************************************************/

/*------------------------------------------------------
	Z80リードメモリ (byte - YM2151 + OKIM6295)
------------------------------------------------------*/

static uint8_t cps1_sound_readmem(uint32_t offset)
{
	offset &= Z80_AMASK;

	if (offset < 0xd800)
	{
		return memory_region_cpu2[offset];
	}
	switch (offset)
	{
	case 0xf001: return YM2151_status_port_r(0);
	case 0xf002: return OKIM6295_status_r(0);
	case 0xf008: return cps1_sound_command_r(0);
	case 0xf00a: return cps1_sound_fade_timer_r(0);
	}

	return 0;
}


/*------------------------------------------------------
	Z80ライトメモリ (byte - YM2151 + OKIM6295)
------------------------------------------------------*/

static void cps1_sound_writemem(uint32_t offset, uint8_t data)
{
	offset &= Z80_AMASK;

	if (offset >= 0xd000 && offset < 0xd800)
	{
		memory_region_cpu2[offset] = data;
		return;
	}
	switch (offset)
	{
	case 0xf000: YM2151_register_port_w(0, data); return;
	case 0xf001: YM2151_data_port_w(0, data); return;
	case 0xf002: OKIM6295_data_w(0, data); return;
	case 0xf004: cps1_snd_bankswitch_w(0, data); return;
	case 0xf006: cps1_oki_pin7_w(0, data); return;
	}
}


/*------------------------------------------------------
	Z80リードメモリ (byte - QSOUND)
------------------------------------------------------*/

static uint8_t cps1_qsound_readmem(uint32_t offset)
{
	offset &= Z80_AMASK;

	switch (offset & 0xf000)
	{
	case 0xd000:
		if (offset == 0xd007) return 0x80;
		break;

	default:
		// 0000-7fff: ROM
		// 8000-bfff: banked ROM
		// c000-cfff: QSOUND shared RAM 1
		// f000-ffff: QSOUND shared RAM 2
		return memory_region_cpu2[offset];
	}

	return 0;
}


/*------------------------------------------------------
	Z80ライトメモリ (byte - QSOUND)
------------------------------------------------------*/

static void cps1_qsound_writemem(uint32_t offset, uint8_t data)
{
	offset &= Z80_AMASK;

	switch (offset & 0xf000)
	{
	case 0xc000: case 0xf000:
		// c000-cfff: QSOUND shared RAM 1
		// f000-ffff: QSOUND shared RAM 2
		memory_region_cpu2[offset] = data;
		break;

	case 0xd000:
		switch (offset)
		{
		case 0xd000: qsound_data_h_w(0, data); break;
		case 0xd001: qsound_data_l_w(0, data); break;
		case 0xd002: qsound_cmd_w(0, data); break;
		case 0xd003: qsound_banksw_w(0, data); break;
		}
		break;
	}
}


#if !RELEASE
/*------------------------------------------------------
	Z80リードメモリ (byte - kodb)
------------------------------------------------------*/

static uint8_t cps1_kodb_readmem(uint32_t offset)
{
	offset &= Z80_AMASK;

	if (offset < 0xd800)
	{
		return memory_region_cpu2[offset];
	}
	switch (offset)
	{
	case 0xe001: return YM2151_status_port_r(0);
	case 0xe400: return OKIM6295_status_r(0);
	case 0xe800: return cps1_sound_command_r(0);
	}

	return 0;
}


/*------------------------------------------------------
	Z80ライトメモリ (byte - kodb)
------------------------------------------------------*/

static void cps1_kodb_writemem(uint32_t offset, uint8_t data)
{
	offset &= Z80_AMASK;

	if (offset >= 0xd000 && offset < 0xd800)
	{
		memory_region_cpu2[offset] = data;
		return;
	}
	switch (offset)
	{
	case 0xe000: YM2151_register_port_w(0, data); return;
	case 0xe001: YM2151_data_port_w(0, data); return;
	case 0xe400: OKIM6295_data_w(0, data); return;
	case 0xf004: cps1_snd_bankswitch_w(0, data); return;
	}
}
#endif


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( memory )
{
	state_save_byte(cps1_ram, sizeof(cps1_ram));
	state_save_byte(cps1_gfxram, sizeof(cps1_gfxram));
	state_save_byte(cps1_output, sizeof(cps1_output));
	if (machine_sound_type == SOUND_QSOUND)
	{
		state_save_byte(qsound_sharedram1, 0x1000);
		state_save_byte(qsound_sharedram2, 0x1000);
	}
	else
	{
		state_save_byte(&memory_region_cpu2[0xc000], 0x1800);
	}
}

STATE_LOAD( memory )
{
	state_load_byte(cps1_ram, sizeof(cps1_ram));
	state_load_byte(cps1_gfxram, sizeof(cps1_gfxram));
	state_load_byte(cps1_output, sizeof(cps1_output));
	if (machine_sound_type == SOUND_QSOUND)
	{
		state_load_byte(qsound_sharedram1, 0x1000);
		state_load_byte(qsound_sharedram2, 0x1000);
	}
	else
	{
		state_load_byte(&memory_region_cpu2[0xc000], 0x1800);
	}
}

#endif /* SAVE_STATE */

/******************************************************************************

	memintrf.c

	CPS2メモリインタフェース関数

******************************************************************************/

#include "cps2.h"


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

#define MAX_CPU1ROM		8
#define MAX_CPU2ROM		8
#define MAX_GFX1ROM		32
#define MAX_SND1ROM		8


/******************************************************************************
	グローバル変数
******************************************************************************/

uint8_t *memory_region_cpu1;
uint8_t *memory_region_cpu2;
uint8_t *memory_region_gfx1;
uint8_t *memory_region_sound1;
uint8_t *memory_region_user1;

uint32_t memory_length_cpu1;
uint32_t memory_length_cpu2;
uint32_t memory_length_gfx1;
uint32_t memory_length_sound1;
uint32_t memory_length_user1;

uint32_t gfx_total_elements[3];
uint8_t *gfx_pen_usage[3];

uint8_t  ALIGN_DATA cps1_ram[0x10000];
uint8_t  ALIGN_DATA cps2_ram[0x4000 + 2];
uint16_t ALIGN_DATA cps1_gfxram[0x30000 >> 1];
uint16_t ALIGN_DATA cps1_output[0x100 >> 1];

uint16_t ALIGN_DATA cps2_objram[2][0x2000 >> 1];
uint16_t ALIGN_DATA cps2_output[0x10 >> 1];

uint8_t *qsound_sharedram1;
uint8_t *qsound_sharedram2;

#if !USE_CACHE
char cache_parent_name[16];
#endif

#ifdef LARGE_MEMORY
uint32_t psp2k_mem_offset = PSP2K_MEM_TOP;
int32_t psp2k_mem_left = PSP2K_MEM_SIZE;
#endif


/******************************************************************************
	ローカル構造体/変数
******************************************************************************/

static struct rom_t cpu1rom[MAX_CPU1ROM];
static struct rom_t cpu2rom[MAX_CPU2ROM];
#if !USE_CACHE
static struct rom_t gfx1rom[MAX_GFX1ROM];
#endif
static struct rom_t snd1rom[MAX_SND1ROM];

static int num_cpu1rom;
static int num_cpu2rom;
#if !USE_CACHE
static int num_gfx1rom;
#endif
static int num_snd1rom;

static uint8_t *static_ram1;
static uint8_t *static_ram2;
static uint8_t *static_ram3;
static uint8_t *static_ram4;
static uint8_t *static_ram5;
static uint8_t *static_ram6;

#if !RELEASE
static int phoenix_edition;
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
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
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
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
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
#if !USE_CACHE
	int i, res;
	char fname[32], *parent;
#endif

	gfx_total_elements[TILE08] = (memory_length_gfx1 - 0x800000) >> 6;
	gfx_total_elements[TILE16] = memory_length_gfx1 >> 7;
	gfx_total_elements[TILE32] = (memory_length_gfx1 - 0x800000) >> 9;

	if (gfx_total_elements[TILE08] > 0x10000) gfx_total_elements[TILE08] = 0x10000;
	if (gfx_total_elements[TILE32] > 0x10000) gfx_total_elements[TILE32] = 0x10000;

	if ((gfx_pen_usage[TILE08] = memalign(MEM_ALIGN, gfx_total_elements[TILE08])) == NULL)
	{
		error_memory("GFX_PEN_USAGE (tile8)");
		return 0;
	}
	if ((gfx_pen_usage[TILE16] = memalign(MEM_ALIGN, gfx_total_elements[TILE16])) == NULL)
	{
		error_memory("GFX_PEN_USAGE (tile16)");
		return 0;
	}
	if ((gfx_pen_usage[TILE32] = memalign(MEM_ALIGN, gfx_total_elements[TILE32])) == NULL)
	{
		error_memory("GFX_PEN_USAGE (tile32)");
		return 0;
	}

#if USE_CACHE
	memory_length_gfx1 = driver->cache_size;

	if (cache_start() == 0)
	{
		msg_printf(TEXT(PRESS_ANY_BUTTON2));
		pad_wait_press(PAD_WAIT_INFINITY);
		Loop = LOOP_BROWSER;
		return 0;
	}
#else
	memset(gfx_pen_usage[TILE08], 0, gfx_total_elements[TILE08]);
	memset(gfx_pen_usage[TILE16], 0, gfx_total_elements[TILE16]);
	memset(gfx_pen_usage[TILE32], 0, gfx_total_elements[TILE32]);

	memory_region_gfx1 = (uint8_t *)psp2k_mem_offset;
	psp2k_mem_offset += memory_length_gfx1;
	psp2k_mem_left -= memory_length_gfx1;

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_gfx1rom; )
	{
		strcpy(fname, gfx1rom[i].name);
		if ((res = file_open(game_name, parent, gfx1rom[i].crc, fname)) < 0)
		{
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		i = rom_load(gfx1rom, memory_region_gfx1, i, num_gfx1rom);

		file_close();
	}

	msg_printf(TEXT(DECODING_GFX), fname);
	cps2_gfx_decode();
#endif

	return 1;
}


/*--------------------------------------------------------
	SOUND1 (Q-SOUND PCM ROM)
--------------------------------------------------------*/

static int load_rom_sound1(void)
{
	int i, res;
	char fname[32], *parent;

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
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		i = rom_load(snd1rom, memory_region_sound1, i, num_snd1rom);

		file_close();
	}

	return 1;
}


/*--------------------------------------------------------
	USER1 (MC68000 ROM decrypt region)
--------------------------------------------------------*/

static int load_rom_user1(void)
{
	if (memory_length_user1)
	{
		if ((memory_region_user1 = memalign(MEM_ALIGN, memory_length_user1)) == NULL)
		{
			error_memory("REGION_USER1");
			return 0;
		}
		memset(memory_region_user1, 0, memory_length_user1);
	}

	return 1;
}


/*--------------------------------------------------------
	ROM情報をデータベースで解析
--------------------------------------------------------*/

static int load_rom_info(const char *game_name)
{
	SceUID fd;
	char path[MAX_PATH];
	char *buf;
	char linebuf[256];
	int i, size;
	int rom_start = 0;
	int region = 0;

	num_cpu1rom = 0;
	num_cpu2rom = 0;
#if !USE_CACHE
	num_gfx1rom = 0;
#endif
	num_snd1rom = 0;

	machine_driver_type  = 0;
	machine_input_type   = 0;
	machine_init_type    = 0;
	machine_screen_type  = 0;

	sprintf(path, "%srominfo.cps2", launchDir);

	if ((fd = sceIoOpen(path, PSP_O_RDONLY, 0777)) >= 0)
	{
		size = sceIoLseek(fd, 0, SEEK_END);
		sceIoLseek(fd, 0, SEEK_SET);

		if ((buf = (char *)malloc(size)) == NULL)
		{
			sceIoClose(fd);
			return 3;	// 手抜き
		}

		sceIoRead(fd, buf, size);
		sceIoClose(fd);

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
						if (str_cmp(parent, "cps2") == 0)
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
#if USE_CACHE
						region = REGION_SKIP;
#else
						region = REGION_GFX1;
#endif
					}
					else if (strcmp(type, "SOUND1") == 0)
					{
						sscanf(size, "%x", &memory_length_sound1);
						region = REGION_SOUND1;
					}
					else if (strcmp(type, "USER1") == 0)
					{
						sscanf(size, "%x", &memory_length_user1);
						region = REGION_SKIP;
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
						cpu1rom[num_cpu1rom].group = 0;
						cpu1rom[num_cpu1rom].skip = 0;
						if (name) strcpy(cpu1rom[num_cpu1rom].name, name);
						num_cpu1rom++;
						break;

					case REGION_CPU2:
						sscanf(type, "%x", &cpu2rom[num_cpu2rom].type);
						sscanf(offset, "%x", &cpu2rom[num_cpu2rom].offset);
						sscanf(length, "%x", &cpu2rom[num_cpu2rom].length);
						sscanf(crc, "%x", &cpu2rom[num_cpu2rom].crc);
						cpu2rom[num_cpu2rom].group = 0;
						cpu2rom[num_cpu2rom].skip = 0;
						if (name) strcpy(cpu2rom[num_cpu2rom].name, name);
						num_cpu2rom++;
						break;

#if !USE_CACHE
					case REGION_GFX1:
						sscanf(type, "%x", &gfx1rom[num_gfx1rom].type);
						sscanf(offset, "%x", &gfx1rom[num_gfx1rom].offset);
						sscanf(length, "%x", &gfx1rom[num_gfx1rom].length);
						sscanf(crc, "%x", &gfx1rom[num_gfx1rom].crc);
						gfx1rom[num_gfx1rom].group = 0;
						gfx1rom[num_gfx1rom].skip = 0;
						if (name) strcpy(gfx1rom[num_gfx1rom].name, name);
						num_gfx1rom++;
						break;
#endif

					case REGION_SOUND1:
						sscanf(type, "%x", &snd1rom[num_snd1rom].type);
						sscanf(offset, "%x", &snd1rom[num_snd1rom].offset);
						sscanf(length, "%x", &snd1rom[num_snd1rom].length);
						sscanf(crc, "%x", &snd1rom[num_snd1rom].crc);
						snd1rom[num_snd1rom].group = 0;
						snd1rom[num_snd1rom].skip = 0;
						if (name) strcpy(snd1rom[num_snd1rom].name, name);
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

					case REGION_CPU2:
						sscanf(type, "%x", &cpu2rom[num_cpu2rom].type);
						sscanf(offset, "%x", &cpu2rom[num_cpu2rom].offset);
						sscanf(length, "%x", &cpu2rom[num_cpu2rom].length);
						sscanf(crc, "%x", &cpu2rom[num_cpu2rom].crc);
						sscanf(group, "%x", &cpu2rom[num_cpu2rom].group);
						sscanf(skip, "%x", &cpu2rom[num_cpu2rom].skip);
						if (name) strcpy(cpu2rom[num_cpu2rom].name, name);
						num_cpu2rom++;
						break;

#if !USE_CACHE
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
#endif

					case REGION_SOUND1:
						sscanf(type, "%x", &snd1rom[num_snd1rom].type);
						sscanf(offset, "%x", &snd1rom[num_snd1rom].offset);
						sscanf(length, "%x", &snd1rom[num_snd1rom].length);
						sscanf(crc, "%x", &snd1rom[num_snd1rom].crc);
						sscanf(group, "%x", &snd1rom[num_snd1rom].group);
						sscanf(skip, "%x", &snd1rom[num_snd1rom].skip);
						if (name) strcpy(snd1rom[num_snd1rom].name, name);
						num_snd1rom++;
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
-----------------------------------------------------*/

int memory_init(void)
{
	int i, res;

	memory_region_cpu1   = NULL;
	memory_region_cpu2   = NULL;
	memory_region_gfx1   = NULL;
	memory_region_sound1 = NULL;
	memory_region_user1  = NULL;

	memory_length_cpu1   = 0;
	memory_length_cpu2   = 0;
	memory_length_gfx1   = 0;
	memory_length_sound1 = 0;
	memory_length_user1  = 0;

	gfx_pen_usage[TILE08] = NULL;
	gfx_pen_usage[TILE16] = NULL;
	gfx_pen_usage[TILE32] = NULL;

#ifdef LARGE_MEMORY
	psp2k_mem_offset = PSP2K_MEM_TOP;
	psp2k_mem_left   = PSP2K_MEM_SIZE;
#endif

#if USE_CACHE
	cache_init();
#endif
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

	if (!strcmp(game_name, "ssf2ta")
	||	!strcmp(game_name, "ssf2tu")
	||	!strcmp(game_name, "ssf2tur1")
	||	!strcmp(game_name, "ssf2xj"))
	{
		strcpy(cache_parent_name, "ssf2t");
	}
	else if (!strcmp(game_name, "ssf2t"))
	{
		cache_parent_name[0] = '\0';
	}
#if USE_CACHE
	else if (!strcmp(game_name, "mpangj"))
	{
		// 多分日本語版はBAD DUMP(一部スプライトの欠けあり)
		cache_parent_name[0] = '\0';
	}
#endif
	else
	{
		strcpy(cache_parent_name, parent_name);
	}

	i = 0;
	driver = NULL;
	while (CPS2_driver[i].name)
	{
		if (!strcmp(game_name, CPS2_driver[i].name) || !strcmp(cache_parent_name, CPS2_driver[i].name))
		{
			driver = &CPS2_driver[i];
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

#if !RELEASE
	if (!strcmp(game_name, "ddtodd")
	||	!strcmp(game_name, "ecofghtrd")
	||	!strcmp(game_name, "ssf2ud")
	||	!strcmp(game_name, "ssf2tbd")
	||	!strcmp(game_name, "armwar1d")
	||	!strcmp(game_name, "avspd")
	||	!strcmp(game_name, "dstlku1d")
	||	!strcmp(game_name, "ringdstd")
	||	!strcmp(game_name, "ssf2xjd")
	||	!strcmp(game_name, "xmcotar1d")
	||	!strcmp(game_name, "mshud")
	||	!strcmp(game_name, "cybotsud")
	||	!strcmp(game_name, "cybotsjd")
	||	!strcmp(game_name, "nwarrud")
	||	!strcmp(game_name, "sfad")
	||	!strcmp(game_name, "19xxd")
	||	!strcmp(game_name, "ddsomud")
	||	!strcmp(game_name, "gigaman2")
	||	!strcmp(game_name, "megamn2d")
	||	!strcmp(game_name, "sfz2ad")
	||	!strcmp(game_name, "sfz2jd")
	||	!strcmp(game_name, "spf2td")
	||	!strcmp(game_name, "spf2xjd")
	||	!strcmp(game_name, "sfz2ald")
	||	!strcmp(game_name, "xmvsfu1d")
	||	!strcmp(game_name, "batcird")
	||	!strcmp(game_name, "csclub1d")
	||	!strcmp(game_name, "mshvsfu1d")
	||	!strcmp(game_name, "sgemfd")
	||	!strcmp(game_name, "vsavd")
	||	!strcmp(game_name, "vhunt2d")
	||	!strcmp(game_name, "vsav2d")
	||	!strcmp(game_name, "mvscud")
	||	!strcmp(game_name, "sfa3ud")
	||	!strcmp(game_name, "sfz3jr2d")
	||	!strcmp(game_name, "gigawingd")
	||	!strcmp(game_name, "gigawingjd")
	||	!strcmp(game_name, "1944d")
	||	!strcmp(game_name, "dimahoud")
	||	!strcmp(game_name, "mmatrixd")
	||	!strcmp(game_name, "progearud")
	||	!strcmp(game_name, "progearjd")
	||	!strcmp(game_name, "progearjbl") // not an actual phoenix set, but works as one
	||	!strcmp(game_name, "hsf2d"))
		phoenix_edition = 1;
	else
		phoenix_edition = 0;
#endif

	if (parent_name[0])
		msg_printf(TEXT(ROMSET_x_PARENT_x), game_name, parent_name);
	else
		msg_printf(TEXT(ROMSET_x), game_name);

	load_gamecfg(game_name);
#ifdef ADHOC
	if (adhoc_enable)
	{
		/* AdHoc通信時は一部オプションで固定の設定を使用 */
#if ENABLE_RASTER_OPTION
		cps_raster_enable    = 1;
#endif
		psp_cpuclock         = PSPCLOCK_333;
		option_vsync         = 0;
		option_autoframeskip = 0;
		option_frameskip     = 0;
		option_showfps       = 0;
		option_speedlimit    = 1;
		option_sound_enable  = 1;
	}
	else
#endif
	{
#ifdef COMMAND_LIST
		if (cache_parent_name[0])
			load_commandlist(game_name, cache_parent_name);
		else
			load_commandlist(game_name, NULL);
#endif
	}

	set_cpu_clock(psp_cpuclock);

	if (load_rom_cpu1() == 0) return 0;
	if (load_rom_user1() == 0) return 0;
	if (load_rom_cpu2() == 0) return 0;
	if (option_sound_enable)
	{
		if (load_rom_sound1() == 0) return 0;
	}
	if (load_rom_gfx1() == 0) return 0;

	static_ram1 = (uint8_t *)cps1_ram    - 0xff0000;
	static_ram2 = (uint8_t *)cps1_gfxram - 0x900000;
	static_ram3 = (uint8_t *)cps2_ram    - 0x660000;
	static_ram4 = (uint8_t *)cps2_output - 0x400000;
	static_ram5 = (uint8_t *)cps2_objram[0];
	static_ram6 = (uint8_t *)cps2_objram[1];

	qsound_sharedram1 = &memory_region_cpu2[0xc000];
	qsound_sharedram2 = &memory_region_cpu2[0xf000];

	memory_region_cpu2[0xd007] = 0x80;

	return 1;
}


/*------------------------------------------------------
	メモリインタフェース終了
------------------------------------------------------*/

void memory_shutdown(void)
{
#if USE_CACHE
	cache_shutdown();
#endif

	if (gfx_pen_usage[TILE08]) free(gfx_pen_usage[TILE08]);
	if (gfx_pen_usage[TILE16]) free(gfx_pen_usage[TILE16]);
	if (gfx_pen_usage[TILE32]) free(gfx_pen_usage[TILE32]);

	if (memory_region_cpu1)   free(memory_region_cpu1);
	if (memory_region_cpu2)   free(memory_region_cpu2);
#if USE_CACHE
	if (memory_region_gfx1)   free(memory_region_gfx1);
#endif
	if (memory_region_sound1) free(memory_region_sound1);
	if (memory_region_user1)  free(memory_region_user1);

#ifdef LARGE_MEMORY
	psp2k_mem_offset = PSP2K_MEM_TOP;
	psp2k_mem_left   = PSP2K_MEM_SIZE;
#endif

#if (USE_CACHE && VIDEO_32BPP)
	GFX_MEMORY = NULL;
#endif
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

	if (offset < memory_length_cpu1)
	{
		return READ_BYTE(memory_region_cpu1, offset);
	}

	shift = (~offset & 1) << 3;
	mem_mask = ~(0xff << shift);

	switch (offset >> 16)
	{
	case 0x40:
		return READ_BYTE(static_ram4, offset);

	case 0x61:
		return qsound_sharedram1_r(offset >> 1, mem_mask) >> shift;

	case 0x66:
		return READ_BYTE(static_ram3, offset);

	case 0x70:
		if (offset & 0x8000)
			return READ_BYTE(static_ram6, (offset & 0x1fff));
		else
			return READ_BYTE(static_ram5, (offset & 0x1fff));
		break;

	case 0x80:
		switch (offset & 0xff00)
		{
		case 0x0100:
		case 0x4100:
			return cps1_output_r(offset >> 1, mem_mask) >> shift;

		case 0x4000:
			switch (offset & 0xfe)
			{
			case 0x00: return cps2_inputport0_r(offset >> 1, mem_mask) >> shift;
			case 0x10: return cps2_inputport1_r(offset >> 1, mem_mask) >> shift;
			case 0x20: return cps2_eeprom_port_r(offset >> 1, mem_mask) >> shift;
			case 0x30: return cps2_qsound_volume_r(offset >> 1, mem_mask) >> shift;
			}
			break;
		}
		break;

	case 0x90:
	case 0x91:
	case 0x92:
		return READ_BYTE(static_ram2, offset);

	case 0xff:
		return READ_BYTE(static_ram1, offset);
	}

	return 0xff;
}


/*------------------------------------------------------
	M68000リードメモリ (word)
------------------------------------------------------*/

uint16_t m68000_read_memory_16(uint32_t offset)
{
	offset &= M68K_AMASK;

	if (offset < memory_length_cpu1)
	{
		return READ_WORD(memory_region_cpu1, offset);
	}

	switch (offset >> 16)
	{
	case 0x40:
		return READ_WORD(static_ram4, offset);

	case 0x61:
		return qsound_sharedram1_r(offset >> 1, 0);

	case 0x66:
		return READ_WORD(static_ram3, offset);

	case 0x70:
		if (offset & 0x8000)
			return READ_WORD(static_ram6, (offset & 0x1fff));
		else
			return READ_WORD(static_ram5, (offset & 0x1fff));
		break;

	case 0x80:
		switch (offset & 0xff00)
		{
		case 0x0100:
		case 0x4100:
			return cps1_output_r(offset >> 1, 0);

		case 0x4000:
			switch (offset & 0xfe)
			{
			case 0x00: return cps2_inputport0_r(offset >> 1, 0);
			case 0x10: return cps2_inputport1_r(offset >> 1, 0);
			case 0x20: return cps2_eeprom_port_r(offset >> 1, 0);
			case 0x30: return cps2_qsound_volume_r(offset >> 1, 0);
			}
			break;
		}
		break;

	case 0x90:
	case 0x91:
	case 0x92:
		return READ_WORD(static_ram2, offset);

	case 0xff:
		return READ_WORD(static_ram1, offset);
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
	case 0x40:
#if !RELEASE
		if (!phoenix_edition)
#endif
			WRITE_BYTE(static_ram4, offset, data);
		return;

	case 0x61:
		qsound_sharedram1_w(offset >> 1, data << shift, mem_mask);
		return;

	case 0x66:
		WRITE_BYTE(static_ram3, offset, data);
		return;

	case 0x70:
		if (offset & 0x8000)
			WRITE_BYTE(static_ram6, (offset & 0x1fff), data);
		else
			WRITE_BYTE(static_ram5, (offset & 0x1fff), data);
		return;

	case 0x80:
		switch (offset & 0xff00)
		{
		case 0x0100:
		case 0x4100:
			cps1_output_w(offset >> 1, data << shift, mem_mask);
			return;

		case 0x4000:
			switch (offset & 0xfe)
			{
			case 0x40:
				cps2_eeprom_port_w(offset >> 1, data << shift, mem_mask);
				return;

			case 0xe0:
				if (offset & 1)
				{
					cps2_objram_bank = data & 1;
					static_ram6 = (uint8_t *)cps2_objram[cps2_objram_bank ^ 1];
				}
				return;
			}
			break;
		}
		break;

	case 0x90:
	case 0x91:
	case 0x92:
		WRITE_BYTE(static_ram2, offset, data);
		return;

	case 0xff:
#if !RELEASE
		if (phoenix_edition)
		{
			if (offset >= 0xfffff0)
			{
				offset -= 0xbffff0;
				WRITE_BYTE(static_ram4, offset, data);
				return;
			}
		}
#endif
		WRITE_BYTE(static_ram1, offset, data);
		return;
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
	case 0x40:
#if !RELEASE
		if (!phoenix_edition)
#endif
			WRITE_WORD(static_ram4, offset, data);
		return;

	case 0x61:
		qsound_sharedram1_w(offset >> 1, data, 0);
		return;

	case 0x66:
		WRITE_WORD(static_ram3, offset, data);
		return;

	case 0x70:
		if (offset & 0x8000)
			WRITE_WORD(static_ram6, (offset & 0x1fff), data);
		else
			WRITE_WORD(static_ram5, (offset & 0x1fff), data);
		break;

	case 0x80:
		switch (offset & 0xff00)
		{
		case 0x0100:
		case 0x4100:
			cps1_output_w(offset >> 1, data, 0);
			return;

		case 0x4000:
			switch (offset & 0xfe)
			{
			case 0x40:
				cps2_eeprom_port_w(offset >> 1, data, 0);
				return;

			case 0xe0:
				cps2_objram_bank = data & 1;
				static_ram6 = (uint8_t *)cps2_objram[cps2_objram_bank ^ 1];
				return;
			}
			break;
		}
		break;

	case 0x90:
	case 0x91:
	case 0x92:
		WRITE_WORD(static_ram2, offset, data);
		return;

	case 0xff:
#if !RELEASE
		if (phoenix_edition)
		{
			if (offset >= 0xfffff0)
			{
				offset -= 0xbffff0;
				WRITE_WORD(static_ram4, offset, data);
				return;
			}
		}
#endif
		WRITE_WORD(static_ram1, offset, data);
		return;
	}
}


/******************************************************************************
	Z80 メモリリード/ライト関数
******************************************************************************/

/*------------------------------------------------------
	Z80リードメモリ (byte)
------------------------------------------------------*/

uint8_t z80_read_memory_8(uint32_t offset)
{
	return memory_region_cpu2[offset & Z80_AMASK];
}


/*------------------------------------------------------
	Z80ライトメモリ (byte)
------------------------------------------------------*/

void z80_write_memory_8(uint32_t offset, uint8_t data)
{
	offset &= Z80_AMASK;

	switch (offset & 0xf000)
	{
	case 0xc000: case 0xf000:
		// c000-cfff: QSOUND shared RAM
		// f000-ffff: RAM
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


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( memory )
{
	state_save_byte(cps1_ram, 0x10000);
	state_save_byte(cps1_gfxram, 0x30000);
	state_save_byte(cps1_output, 0x100);
	state_save_byte(cps2_ram, 0x4002);
	state_save_byte(cps2_objram[0], 0x2000);
	state_save_byte(cps2_objram[1], 0x2000);
	state_save_byte(cps2_output, 0x10);
	state_save_byte(qsound_sharedram1, 0x1000);
	state_save_byte(qsound_sharedram2, 0x1000);
}

STATE_LOAD( memory )
{
	state_load_byte(cps1_ram, 0x10000);
	state_load_byte(cps1_gfxram, 0x30000);
	state_load_byte(cps1_output, 0x100);
	state_load_byte(cps2_ram, 0x4002);
	state_load_byte(cps2_objram[0], 0x2000);
	state_load_byte(cps2_objram[1], 0x2000);
	state_load_byte(cps2_output, 0x10);
	state_load_byte(qsound_sharedram1, 0x1000);
	state_load_byte(qsound_sharedram2, 0x1000);
}

#endif /* SAVE_STATE */

/*****************************************************************************

	romcnv.c

	ROM converter for MVSPSP

******************************************************************************/

#include "romcnv.h"

#define MAX_GAMES			512

#define SPRITE_BLANK		0x00
#define SPRITE_TRANSPARENT	0x01
#define SPRITE_OPAQUE		0x02

#define MAX_GFX2ROM			4
#define MAX_GFX3ROM			16
#define MAX_SND1ROM			8

enum
{
	REGION_CPU1 = 0,
	REGION_CPU2,
	REGION_GFX1,
	REGION_GFX2,
	REGION_GFX3,
	REGION_SOUND1,
	REGION_SOUND2,
	REGION_USER1,
	REGION_SKIP
};

enum
{
	TILE_FIX = 0,
	TILE_SPR,
	TILE_TYPE_MAX
};


/******************************************************************************
	グローバル変数
******************************************************************************/

UINT8 *memory_region_gfx2;
UINT8 *memory_region_gfx3;
UINT8 *memory_region_sound1;

UINT32 memory_length_gfx2;
UINT32 memory_length_gfx3;
UINT32 memory_length_sound1;


/******************************************************************************
	ローカル変数
******************************************************************************/

static UINT32 gfx_total_elements[TILE_TYPE_MAX];
static UINT8  *gfx_pen_usage[TILE_TYPE_MAX];

static int disable_sound;
static int machine_driver_type;
static int machine_init_type;
static int machine_input_type;
static int machine_screen_type;

static struct rom_t gfx2rom[MAX_GFX2ROM];
static struct rom_t gfx3rom[MAX_GFX3ROM];
static struct rom_t snd1rom[MAX_SND1ROM];

static int num_gfx2rom;
static int num_gfx3rom;
static int num_snd1rom;

static int encrypt_gfx2;
static int encrypt_gfx3;
static int encrypt_snd1;

static int convert_crom;
static int convert_srom;
static int convert_vrom;

static int psp2k;

static char game_names[MAX_GAMES][16];

struct cacheinfo_t
{
	const char *name;
	const char *parent;
	const int crom;
	const int srom;
	const int vrom;
};

struct cacheinfo_t MVS_cacheinfo[] =
{
//     name,  parent,  crom,  srom,  vrom
	{ "aof2a",    "aof2",     0, 0, 0 },
	{ "fatfursa", "fatfursp", 0, 0, 0 },
	{ "kof95a",   "kof95",    0, 0, 0 },
	{ "samsho3a", "samsho3",  0, 0, 0 },
	{ "fswords",  "samsho3",  0, 0, 0 },
	{ "aof3k",    "aof3",     0, 0, 0 },
	{ "kof96cn",  "kof96",    1, 1, 0 },
	{ "kof96h",   "kof96",    0, 0, 0 },
	{ "kof96ep",  "kof96",    0, 0, 0 },
	{ "kizuna",   "savagere", 1, 1, 1 },
	{ "kof97a",   "kof97",    0, 0, 0 },
	{ "kof97cn",  "kof97",    1, 1, 0 },
	{ "kof97d",   "kof97",    0, 0, 0 },
	{ "kof97k",   "kof97",    0, 0, 0 },
	{ "kof97pls", "kof97",    0, 0, 0 },
	{ "kof97pla", "kof97",    0, 1, 0 },
	{ "kof97ps",  "kof97",    1, 0, 0 },
	{ "kog",      "kof97",    1, 1, 0 },
	{ "kogd",     "kof97",    1, 1, 0 },
	{ "kof97oro", "kof97",    1, 1, 0 },
	{ "lastbldh", "lastblad", 0, 0, 0 },
	{ "lastsold", "lastblad", 0, 0, 0 },
	{ "shocktra", "shocktro", 0, 0, 0 },
	{ "rbff2h",   "rbff2",    0, 0, 0 },
	{ "rbff2k",   "rbff2",    0, 0, 0 },
	{ "kof98c",   "kof98",    1, 1, 0 },
	{ "kof98cn",  "kof98",    1, 1, 0 },
	{ "kof98k",   "kof98",    0, 0, 0 },
	{ "kof98ka",  "kof98",    0, 0, 0 },
	{ "kof98n",   "kof98",    0, 0, 0 },
	{ "kof98ae",  "kof98",    1, 1, 1 },
	{ "breakrev", "breakers", 1, 1, 1 },
	{ "lans2004", "shocktr2", 1, 1, 1 },
	{ "kof99a",   "kof99",    0, 0, 0 },
	{ "kof99e",   "kof99",    0, 0, 0 },
	{ "kof99k",   "kof99",    0, 0, 0 },
	{ "kof99p",   "kof99",    1, 1, 0 },
	{ "kof99ae",  "kof99",    1, 1, 1 },
	{ "garouo",   "garou",    0, 0, 0 },
	{ "garoubl",  "garoup",   0, 1, 1 },
	{ "mslug3h",  "mslug3",   0, 0, 0 },
	{ "mslug3n",  "mslug3",   0, 0, 0 },
	{ "mslug3b6", "mslug3",   0, 1, 0 },
	{ "kof2kd",   "kof2000",  1, 1, 0 },
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
	{ "matrimbl", "matrim",   0, 0, 1 },
	{ "mslug5b",  "mslug5",   0, 0, 0 },
	{ "mslug5h",  "mslug5",   0, 0, 0 },
	{ "ms5plus",  "mslug5",   0, 1, 0 },
	{ "svcpcba",  "svcpcb",   0, 0, 0 },
	{ "samsho5h", "samsho5",  0, 0, 0 },
	{ "samsho5b", "samsho5",  1, 1, 1 },
	{ "samsh5sh", "samsh5sp", 0, 0, 0 },
	{ "samsh5sn", "samsh5sp", 0, 0, 0 },
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
	MVS用関数
******************************************************************************/

static void neogeo_decode_spr(UINT8 *mem, UINT32 length, UINT8 *usage)
{
	UINT32 i;

	for (i = 0; i < gfx_total_elements[TILE_SPR]; i++)
	{
		UINT8 swap[128], *gfxdata;
		UINT32 x, y, pen, opaque = 0;

		gfxdata = &mem[128 * i];

		memcpy(swap, gfxdata, 128);

		for (y = 0; y < 16; y++)
		{
			UINT32 dw, data;

			dw = 0;
			for (x = 0; x < 8; x++)
			{
				pen  = ((swap[64 + 4*y + 3] >> x) & 1) << 3;
				pen |= ((swap[64 + 4*y + 1] >> x) & 1) << 2;
				pen |= ((swap[64 + 4*y + 2] >> x) & 1) << 1;
				pen |= ((swap[64 + 4*y + 0] >> x) & 1) << 0;
				opaque += (pen & 0x0f) != 0;
				dw |= pen << 4*x;
			}

			data = ((dw & 0x0000000f) >>  0) | ((dw & 0x000000f0) <<  4)
				 | ((dw & 0x00000f00) <<  8) | ((dw & 0x0000f000) << 12)
				 | ((dw & 0x000f0000) >> 12) | ((dw & 0x00f00000) >>  8)
				 | ((dw & 0x0f000000) >>  4) | ((dw & 0xf0000000) >>  0);

			*(gfxdata++) = data >>  0;
			*(gfxdata++) = data >>  8;
			*(gfxdata++) = data >> 16;
			*(gfxdata++) = data >> 24;

			dw = 0;
			for (x = 0;x < 8;x++)
			{
				pen  = ((swap[4*y + 3] >> x) & 1) << 3;
				pen |= ((swap[4*y + 1] >> x) & 1) << 2;
				pen |= ((swap[4*y + 2] >> x) & 1) << 1;
				pen |= ((swap[4*y + 0] >> x) & 1) << 0;
				opaque += (pen & 0x0f) != 0;
				dw |= pen << 4*x;
			}

			data = ((dw & 0x0000000f) >>  0) | ((dw & 0x000000f0) <<  4)
				 | ((dw & 0x00000f00) <<  8) | ((dw & 0x0000f000) << 12)
				 | ((dw & 0x000f0000) >> 12) | ((dw & 0x00f00000) >>  8)
				 | ((dw & 0x0f000000) >>  4) | ((dw & 0xf0000000) >>  0);

			*(gfxdata++) = data >>  0;
			*(gfxdata++) = data >>  8;
			*(gfxdata++) = data >> 16;
			*(gfxdata++) = data >> 24;
		}

		if (opaque)
			*usage = (opaque == 256) ? SPRITE_OPAQUE : SPRITE_TRANSPARENT;
		else
			*usage = SPRITE_BLANK;
		usage++;
	}
}


static int load_rom_gfx2(void)
{
	if (encrypt_gfx2)
	{
		int i;
		char fname[32], *parent;

		gfx_total_elements[TILE_FIX] = memory_length_gfx2 / 32;

		if ((memory_region_gfx2 = calloc(1, memory_length_gfx2)) == NULL)
		{
			error_memory("REGION_GFX2");
			return 0;
		}
		if ((gfx_pen_usage[TILE_FIX] = calloc(1, gfx_total_elements[TILE_FIX])) == NULL)
		{
			error_memory("PEN_USAGE_GFX2");
			return 0;
		}

		parent = strlen(parent_name) ? parent_name : NULL;

		for (i = 0; i < num_gfx2rom; )
		{
			int res;

			strcpy(fname, gfx2rom[i].name);
			if ((res = file_open(game_name, parent, gfx2rom[i].crc, fname)) < 0)
			{
				if (res == -1)
					error_file(fname);
				else
					error_crc(fname);
				return 0;
			}
#ifdef CHINESE
			printf("正在读取 \"%s\"\n", fname);
#else
			printf("Loading \"%s\"\n", fname);
#endif
			i = rom_load(gfx2rom, memory_region_gfx2, i, num_gfx2rom);

			file_close();
		}
	}

	return 1;
}


static int load_rom_gfx3(void)
{
	int i;
	char fname[32], *parent;

	gfx_total_elements[TILE_SPR] = memory_length_gfx3 / 128;

	if ((memory_region_gfx3 = calloc(1, memory_length_gfx3)) == NULL)
	{
		error_memory("REGION_GFX3");
		return 0;
	}
	if ((gfx_pen_usage[TILE_SPR] = calloc(1, gfx_total_elements[TILE_SPR])) == NULL)
	{
		error_memory("PEN_USAGE_GFX3");
		return 0;
	}

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_gfx3rom; )
	{
		int res;

		strcpy(fname, gfx3rom[i].name);
		if ((res = file_open(game_name, parent, gfx3rom[i].crc, fname)) < 0)
		{
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
			return 0;
		}
#ifdef CHINESE
		printf("正在读取 \"%s\"\n", fname);
#else
		printf("Loading \"%s\"\n", fname);
#endif
		i = rom_load(gfx3rom, memory_region_gfx3, i, num_gfx3rom);

		file_close();
	}

	return 1;
}


static int load_rom_sound1(void)
{
	int i;
	char fname[32], *parent;

	if ((memory_region_sound1 = calloc(1, memory_length_sound1)) == NULL)
	{
		error_memory("REGION_SOUND1");
		return 0;
	}

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_snd1rom; )
	{
		int res;

		strcpy(fname, snd1rom[i].name);
		if ((res = file_open(game_name, parent, snd1rom[i].crc, fname)) < 0)
		{
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
			return 0;
		}

#ifdef CHINESE
		printf("正在读取 \"%s\"\n", fname);
#else
		printf("Loading \"%s\"\n", fname);
#endif
		i = rom_load(snd1rom, memory_region_sound1, i, num_snd1rom);

		file_close();
	}

	return 1;
}


static int build_game_list(void)
{
	FILE *fp;
	char path[MAX_PATH];
	char buf[256];
	int num_games = 0;

	sprintf(path, "%srominfo.mvs", launchDir);

	if ((fp = fopen(path, "r")) != NULL)
	{
		while (fgets(buf, 255, fp))
		{
			if (buf[0] == '/' && buf[1] == '/')
				continue;

			if (buf[0] != '\t')
			{
				if (str_cmp(buf, "FILENAME(") == 0)
				{
					char *name;

					strtok(buf, " ");
					strcpy(game_names[num_games], strtok(NULL, " ,"));
					num_games++;
				}
			}
		}
		fclose(fp);
		return num_games;
	}
	return 0;
}


static int load_rom_info(const char *game_name)
{
	FILE *fp;
	char path[MAX_PATH];
	char buf[256];
	int rom_start = 0;
	int region = 0;
	int total_size = 0;

	num_gfx2rom = 0;
	num_gfx3rom = 0;
	num_snd1rom = 0;

	machine_driver_type = 0;
	machine_input_type  = 0;
	machine_init_type   = 0;
	machine_screen_type = 0;

	encrypt_gfx2 = 0;
	encrypt_gfx3 = 0;
	encrypt_snd1 = 0;

	disable_sound = 0;

	sprintf(path, "%srominfo.mvs", launchDir);

	if ((fp = fopen(path, "r")) != NULL)
	{
		while (fgets(buf, 255, fp))
		{
			if (buf[0] == '/' && buf[1] == '/')
				continue;

			if (buf[0] != '\t')
			{
				if (buf[0] == '\r' || buf[0] == '\n')
				{
					// 改行
					continue;
				}
				else if (str_cmp(buf, "FILENAME(") == 0)
				{
					char *name, *parent;
					char *machine, *input, *init, *rotate;

					strtok(buf, " ");
					name    = strtok(NULL, " ,");
					parent  = strtok(NULL, " ,");
					machine = strtok(NULL, " ,");
					input   = strtok(NULL, " ,");
					init    = strtok(NULL, " ,");
					rotate  = strtok(NULL, " ");

					if (strcasecmp(name, game_name) == 0)
					{
						if (str_cmp(parent, "neogeo") == 0)
						{
							parent_name[0] = '\0';
						}
						else if (str_cmp(parent, "pcb") == 0)
						{
							parent_name[0] = '\0';
						}
						else
						{
							strcpy(parent_name, parent);
						}

						sscanf(machine, "%d", &machine_driver_type);
						sscanf(input, "%d", &machine_input_type);
						sscanf(init, "%d", &machine_init_type);
						sscanf(rotate, "%d", &machine_screen_type);
						rom_start = 1;
					}
				}
/*
				else if (rom_start && str_cmp(buf, "END") == 0)
				{
					fclose(fp);
					if (total_size >= 16*1024*1024)
						return 0;
					else
						return 4;
				}
*/
				else if (rom_start && str_cmp(buf, "END") == 0)
				{
					fclose(fp);
					if (psp2k)
						{
						if ((total_size > 0x2b50000) || (encrypt_gfx3))
						return 0;
						else
						return 4;
						}
					else
						{
						if (total_size >= 16*1024*1024)
						return 0;
						else
						return 4;
						}
				}
			}
			else if (rom_start)
			{
				if (str_cmp(&buf[1], "REGION(") == 0)
				{
					char *size, *type, *flag;
					int encrypted = 0;
					int size2;

					strtok(&buf[1], " ");
					size = strtok(NULL, " ,");
					type = strtok(NULL, " ,");
					flag = strtok(NULL, " ");

					sscanf(size, "%x", &size2);
					total_size += size2;

					if (strstr(flag, "SOUND_DISABLE")) disable_sound = 1;
					if (strstr(flag, "ENCRYPTED")) encrypted = 1;

					if (strcmp(type, "GFX2") == 0)
					{
						sscanf(size, "%x", &memory_length_gfx2);
						encrypt_gfx2 = encrypted;
						region = REGION_GFX2;
					}
					else if (strcmp(type, "GFX3") == 0)
					{
						sscanf(size, "%x", &memory_length_gfx3);
						encrypt_gfx3 = encrypted;
						region = REGION_GFX3;
					}
					else if (strcmp(type, "SOUND1") == 0)
					{
						sscanf(size, "%x", &memory_length_sound1);
						encrypt_snd1 = encrypted;
						region = REGION_SOUND1;
					}
					else
					{
						region = REGION_SKIP;
					}
				}
				else if (str_cmp(&buf[1], "ROM(") == 0)
				{
					char *type, *name, *offset, *length, *crc;

					strtok(&buf[1], " ");
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
					case REGION_GFX2:
						sscanf(type, "%x", &gfx2rom[num_gfx2rom].type);
						sscanf(offset, "%x", &gfx2rom[num_gfx2rom].offset);
						sscanf(length, "%x", &gfx2rom[num_gfx2rom].length);
						sscanf(crc, "%x", &gfx2rom[num_gfx2rom].crc);
						if (name) strcpy(gfx2rom[num_gfx2rom].name, name);
						gfx2rom[num_gfx2rom].group = 0;
						gfx2rom[num_gfx2rom].skip = 0;
						num_gfx2rom++;
						break;

					case REGION_GFX3:
						sscanf(type, "%x", &gfx3rom[num_gfx3rom].type);
						sscanf(offset, "%x", &gfx3rom[num_gfx3rom].offset);
						sscanf(length, "%x", &gfx3rom[num_gfx3rom].length);
						sscanf(crc, "%x", &gfx3rom[num_gfx3rom].crc);
						if (name) strcpy(gfx3rom[num_gfx3rom].name, name);
						gfx3rom[num_gfx3rom].group = 0;
						gfx3rom[num_gfx3rom].skip = 0;
						num_gfx3rom++;
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
				else if (str_cmp(&buf[1], "ROMX(") == 0)
				{
					char *type, *name, *offset, *length, *crc;
					char *group, *skip;

					strtok(&buf[1], " ");
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
					case REGION_GFX3:
						sscanf(type, "%x", &gfx3rom[num_gfx3rom].type);
						sscanf(offset, "%x", &gfx3rom[num_gfx3rom].offset);
						sscanf(length, "%x", &gfx3rom[num_gfx3rom].length);
						sscanf(crc, "%x", &gfx3rom[num_gfx3rom].crc);
						sscanf(group, "%x", &gfx3rom[num_gfx3rom].group);
						sscanf(skip, "%x", &gfx3rom[num_gfx3rom].skip);
						if (name) strcpy(gfx3rom[num_gfx3rom].name, name);
						num_gfx3rom++;
						break;

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
		fclose(fp);
		return 2;
	}
	return 3;
}


void free_memory(void)
{
	if (memory_region_gfx2)      free(memory_region_gfx2);
	if (memory_region_gfx3)      free(memory_region_gfx3);
	if (memory_region_sound1)    free(memory_region_sound1);
	if (gfx_pen_usage[TILE_SPR]) free(gfx_pen_usage[TILE_SPR]);
	if (gfx_pen_usage[TILE_FIX]) free(gfx_pen_usage[TILE_FIX]);
}


static int convert_rom(char *game_name)
{
	int i, res;
#ifdef CHINESE
	printf("正在检查ROM文件... (%s)\n", game_name);
#else
	printf("Checking ROM file... (%s)\n", game_name);
#endif
	memory_region_gfx2   = NULL;
	memory_region_gfx3   = NULL;
	memory_region_sound1 = NULL;

	memory_length_gfx2   = 0;
	memory_length_gfx3   = 0;
	memory_length_sound1 = 0;

	gfx_pen_usage[TILE_FIX] = NULL;
	gfx_pen_usage[TILE_SPR] = NULL;

	if ((res = load_rom_info(game_name)) != 0)
	{
		switch (res)
		{
#ifdef CHINESE
		case 1: printf("错误: 此游戏暂时不支持.\n"); break;
		case 2: printf("错误: 没有找到ROM. (zip文件名不正确)\n"); break;
		case 3: printf("错误: 没有找到rominfo.mvs.\n"); break;
		case 4: printf("消息: 不需要转换此游戏.\n"); break;
#else
		case 1: printf("ERROR: This game is not supported.\n"); break;
		case 2: printf("ERROR: ROM not found. (zip file name incorrect)\n"); break;
		case 3: printf("ERROR: rominfo.mvs not found.\n"); break;
		case 4: printf("INFO: No need to convert this game.\n"); break;
#endif
		}
		return 0;
	}

	if (strlen(parent_name))
#ifdef CHINESE
		printf("子ROM版本 (主ROM名: %s)\n", parent_name);
#else
		printf("Clone set (parent: %s)\n", parent_name);
#endif
	if (psp2k) disable_sound = 0;

	if (encrypt_snd1 || disable_sound)
	{
		if (load_rom_sound1())
		{
			if (encrypt_snd1)
			{
				switch (machine_init_type)
				{
				case INIT_kof2002:	neo_pcm2_swap(0);		break;
				case INIT_mslug5:	neo_pcm2_swap(2);		break;
				case INIT_svc:     neo_pcm2_swap(3);       break;
				case INIT_samsho5:	neo_pcm2_swap(4);		break;
				case INIT_kof2003:	neo_pcm2_swap(5);		break;
				case INIT_samsh5sp:neo_pcm2_swap(6);		break;
				case INIT_pnyaa:	neo_pcm2_snk_1999(4);	break;
				case INIT_mslug4:	neo_pcm2_snk_1999(8);	break;
				case INIT_rotd:	neo_pcm2_snk_1999(16);	break;
				case INIT_matrim:	neo_pcm2_swap(1);		break;

				case INIT_ms5pcb:	neo_pcm2_swap(2);		break;
				case INIT_svcpcb:	neo_pcm2_swap(3);		break;
				case INIT_kf2k3pcb:neo_pcm2_swap(5);		break;

				case INIT_kof2002b:neo_pcm2_swap(0);		break;
				case INIT_kf2k2pls:neo_pcm2_swap(0);		break;
				case INIT_kf2k2plc:neo_pcm2_swap(0);		break;
				case INIT_kf2k2mp:	neo_pcm2_swap(0);		break;
				case INIT_kf2k2mp2:neo_pcm2_swap(0);		break;
				case INIT_ms5plus:	neo_pcm2_swap(2);		break;
				case INIT_mslug5b:	neo_pcm2_swap(2);		break;
				case INIT_samsho5b:samsho5b_vx_decrypt();	break;
				case INIT_lans2004:lans2004_vx_decrypt();	break;

				default: goto error;
				}
			}
		}
		else
		{
			goto error;
		}
	}

	if (encrypt_gfx2 || encrypt_gfx3)
	{
		if (load_rom_gfx2() && load_rom_gfx3())
		{
			switch (machine_init_type)
			{
			case INIT_kof99:	kof99_neogeo_gfx_decrypt(0x00);		break;
			case INIT_kof99k:	kof99_neogeo_gfx_decrypt(0x00);		break;
			case INIT_garou:	kof99_neogeo_gfx_decrypt(0x06);		break;
			case INIT_garouh:	kof99_neogeo_gfx_decrypt(0x06);		break;
			case INIT_mslug3:	kof99_neogeo_gfx_decrypt(0xad);		break;
			case INIT_mslug3h:	kof99_neogeo_gfx_decrypt(0xad);		break;
			case INIT_kof2000:	kof2000_neogeo_gfx_decrypt(0x00);	break;
			case INIT_kof2000n:kof2000_neogeo_gfx_decrypt(0x00);	break;
			case INIT_zupapa:	kof99_neogeo_gfx_decrypt(0xbd);		break;
			case INIT_sengoku3:kof99_neogeo_gfx_decrypt(0xfe);		break;
			case INIT_kof2001:	kof2000_neogeo_gfx_decrypt(0x1e);	break;
			case INIT_kof2002:	kof2000_neogeo_gfx_decrypt(0xec);	break;
			case INIT_mslug5:	kof2000_neogeo_gfx_decrypt(0x19);	break;
			case INIT_svc:     kof2000_neogeo_gfx_decrypt(0x57);	break;
			case INIT_samsho5:	kof2000_neogeo_gfx_decrypt(0x0f);	break;
			case INIT_kof2003:	kof2000_neogeo_gfx_decrypt(0x9d);	break;
			case INIT_samsh5sp:kof2000_neogeo_gfx_decrypt(0x0d);	break;
			case INIT_nitd:	kof99_neogeo_gfx_decrypt(0xff);		break;
			case INIT_s1945p:	kof99_neogeo_gfx_decrypt(0x05);		break;
			case INIT_pnyaa:	kof2000_neogeo_gfx_decrypt(0x2e);	break;
			case INIT_preisle2:kof99_neogeo_gfx_decrypt(0x9f);		break;
			case INIT_ganryu:	kof99_neogeo_gfx_decrypt(0x07);		break;
			case INIT_bangbead:kof99_neogeo_gfx_decrypt(0xf8);		break;
			case INIT_mslug4:	kof2000_neogeo_gfx_decrypt(0x31);	break;
			case INIT_rotd:	kof2000_neogeo_gfx_decrypt(0x3f);	break;
			case INIT_matrim:	kof2000_neogeo_gfx_decrypt(0x6a);	break;
			case INIT_jockeygp:kof2000_neogeo_gfx_decrypt(0xac);	break;

			// Jamma PCB

			case INIT_ms5pcb:
				svcpcb_gfx_decrypt();
				kof2000_neogeo_gfx_decrypt(0x19);
				svcpcb_s1data_decrypt();
				break;

			case INIT_svcpcb:
				svcpcb_gfx_decrypt();
				kof2000_neogeo_gfx_decrypt(0x57);
				svcpcb_s1data_decrypt();
				break;

			case INIT_kf2k3pcb:
				kf2k3pcb_gfx_decrypt();
				kof2000_neogeo_gfx_decrypt(0x9d);
				kf2k3pcb_decrypt_s1data();
				break;

			// bootleg

			case INIT_kof97pla:
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_garoubl:
				neogeo_bootleg_sx_decrypt(2);
				neogeo_bootleg_cx_decrypt();
				break;

			case INIT_kf2k1pls:
			case INIT_kf2k1pa:
				cmc50_neogeo_gfx_decrypt(0x1e);
				break;

			case INIT_kof2002b:
				kof2002b_cx_decrypt();
				kof2002b_sx_decrypt();
				break;

			case INIT_kf2k2pls:
				cmc50_neogeo_gfx_decrypt(0xec);
				break;

			case INIT_kf2k2plc:
			case INIT_kf2k2mp:
				cmc50_neogeo_gfx_decrypt(0xec);
				neogeo_bootleg_sx_decrypt(2);
				break;

			case INIT_kf2k2mp2:
				cmc50_neogeo_gfx_decrypt(0xec);
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_kf2k4pls:
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_mslug5b:
				kof2000_neogeo_gfx_decrypt(0x19);
				break;

			case INIT_ms5plus:
				cmc50_neogeo_gfx_decrypt(0x19);
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_svcboot:
			case INIT_svcplusa:
				svcboot_cx_decrypt();
				break;

			case INIT_svcplus:
				svcboot_cx_decrypt();
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_svcsplus:
				svcboot_cx_decrypt();
				neogeo_bootleg_sx_decrypt(2);
				break;

			case INIT_kf2k3bl:
			case INIT_kf2k3pl:
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_kf2k3upl:
				neogeo_bootleg_sx_decrypt(2);
				break;

			case INIT_kog:
				neogeo_bootleg_cx_decrypt();
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_kof97oro:
				neogeo_bootleg_cx_decrypt();
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_cthd2003:
			case INIT_cthd2k3a:
			case INIT_ct2k3sa:
				cthd2003_cx_decrypt();
				break;

			case INIT_ct2k3sp:
				cthd2003_cx_decrypt();
				ct2k3sp_sx_decrypt();
				break;

			case INIT_samsho5b:
				samsho5b_cx_decrypt();
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_lans2004:
				neogeo_bootleg_cx_decrypt();
				neogeo_bootleg_sx_decrypt(1);
				break;

			case INIT_mslug3b6:
				cmc42_neogeo_gfx_decrypt(0xad);
				neogeo_bootleg_sx_decrypt(2);
				break;

			case INIT_matrimbl:
				cthd2003_cx_decrypt();
				neogeo_sfix_decrypt();
				break;

			default: goto error;
			}

			neogeo_decode_spr(memory_region_gfx3, memory_length_gfx3, gfx_pen_usage[TILE_SPR]);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (load_rom_gfx3())
		{
			neogeo_decode_spr(memory_region_gfx3, memory_length_gfx3, gfx_pen_usage[TILE_SPR]);
		}
		else
		{
			return 0;
		}
	}

	return 1;

error:
	return 0;
}



static int create_raw_cache(char *game_name)
{
	FILE *fp;
	char version[8];
	char fname[MAX_PATH];

	sprintf(version, "MVS_V%d%d\0", VERSION_MAJOR, VERSION_MINOR);

	chdir("cache");
#ifdef CHINESE
	printf("正在创建缓存文件...\n");
#else
	printf("Create cache file...\n");
#endif
	sprintf(fname, "%s_cache", game_name);
	if (chdir(fname) != 0)
	{
#ifdef UNIX
		if (mkdir(fname, 0777) != 0)
#else
		if (mkdir(fname) != 0)
#endif
		{
#ifdef CHINESE
			printf("错误: 无法创建文件夹.\n");
#else
			printf("ERROR: Could not create folder.\n");
#endif
			chdir(launchDir);
			return 0;
		}
		chdir(fname);
	}

	if ((fp = fopen("cache_info", "wb")) == NULL) goto error;
	fwrite(version, 1, 8, fp);
	fwrite(gfx_pen_usage[TILE_SPR], 1, gfx_total_elements[TILE_SPR], fp);
	fclose(fp);

	if (convert_crom)
	{
		if ((fp = fopen("crom", "wb")) == NULL) goto error;
		fwrite(memory_region_gfx3, 1, memory_length_gfx3, fp);
		fclose(fp);
	}
	if (convert_srom && encrypt_gfx2)
	{
		if ((fp = fopen("srom", "wb")) == NULL) goto error;
		fwrite(memory_region_gfx2, 1, memory_length_gfx2, fp);
		fclose(fp);
	}
	if (convert_vrom && (encrypt_snd1 || disable_sound))
	{
		if ((fp = fopen("vrom", "wb")) == NULL) goto error;
		fwrite(memory_region_sound1, 1, memory_length_sound1, fp);
		fclose(fp);
	}

	chdir("..");
	chdir("..");
	return 1;

error:
	remove("cache_info");
	if (convert_crom)
	{
		remove("crom");
	}
	if (convert_srom && encrypt_gfx2)
	{
		remove("srom");
	}
	if (convert_vrom && (encrypt_snd1 || disable_sound))
	{
		remove("vrom");
	}

	chdir("..");

	sprintf(fname, "cache_%s", game_name);
	rmdir(fname);
#ifdef CHINESE
	printf("错误: 无法创建文件.\n");
#else
	printf("ERROR: Could not create file.\n");
#endif
	chdir("..");
	return 0;
}


int main(int argc, char *argv[])
{
	char *p, path[MAX_PATH];
	int i, path_found = 0, all = 0, zip = 0, res = 1;
#ifdef WIN32
	int pause = 1;

	check_windows_version();
#endif
	check_byte_order();
#ifdef CHINESE
	printf("----------------------------------------------\n");
	printf(" MVSPSP ROM 转换器  " VERSION_STR "\n");
	printf("----------------------------------------------\n\n");
#else
	printf("----------------------------------------------\n");
	printf(" ROM converter for MVSPSP " VERSION_STR "\n");
	printf("----------------------------------------------\n\n");
#endif

	psp2k = 0;
	if (argc > 1)
	{
		for (i = 1; i < argc; i++)
		{
			if (!strcasecmp(argv[i], "-all"))
			{
				all = 1;
			}
			else if (!strcasecmp(argv[i], "-slim"))
			{
				psp2k = 1;
			}
#ifdef WIN32
			else if (!strcasecmp(argv[i], "-batch"))
			{
				pause = 0;
			}
			else if (strchr(argv[i], ':') != NULL || strchr(argv[i], DELIMITER) != NULL)
#else
			else if (strchr(argv[i], DELIMITER) != NULL)
#endif
			{
				path_found = i;
			}
		}
	}

#ifndef WIN32
	if (!path_found)
	{
		printf("usage: romcnv_mvs fullpath%cgamename.zip\n", DELIMITER);
		printf("  or   romcnv_mvs fullpath -all\n\n", DELIMITER);
		return 0;
	}
#endif

	if (chdir("cache") != 0)
	{
#ifdef UNIX
		if (mkdir("cache", 0777) != 0)
#else
		if (mkdir("cache") != 0)
#endif
		{
#ifdef CHINESE
			printf("错误: 无法创建\"cache\"目录.\n");
#else
			printf("ERROR: Could not create directory \"cache\".\n");
#endif
			goto error;
		}
	}
	else chdir("..");

#ifdef WIN32
	strcpy(launchDir, argv[0]);
	convert_delimiter(launchDir);

	if ((p = strrchr(launchDir, delimiter)) != NULL)
	{
		*(p + 1) = '\0';
	}
	else
	{
		getcwd(launchDir, MAX_PATH);

		convert_delimiter(launchDir);
		if (is_win9x)
			strcat(launchDir, "\\");
		else
			strcat(launchDir, "/");
	}
#else
	getcwd(launchDir, MAX_PATH);
	strcat(launchDir, "/");
#endif

	if (all)
	{
		int total_games;

#ifdef WIN32
		if (!folder_dialog(NULL, zip_dir)) goto error;
		convert_delimiter(zip_dir);

		strcpy(game_dir, zip_dir);

		if (is_win9x)
			strcat(game_dir, "\\");
		else
			strcat(game_dir, "/");
#else
		strcpy(zip_dir, argv[path_found]);
		strcpy(game_dir, zip_dir);
		strcat(game_dir, "/");
#endif

		total_games = build_game_list();

		for (i = 0; i < total_games; i++)
		{
			int j = 0;

			res = 1;

			strcpy(game_name, game_names[i]);

			convert_crom = 1;
			convert_srom = 1;
			convert_vrom = 1;

			while (MVS_cacheinfo[j].name)
			{
				if (strcmp(game_name, MVS_cacheinfo[j].name) == 0)
				{
					convert_crom = MVS_cacheinfo[j].crom;
					convert_srom = MVS_cacheinfo[j].srom;
					convert_vrom = MVS_cacheinfo[j].vrom;
					break;
				}
				j++;
			}
			if (!convert_crom && !convert_srom && !convert_vrom)
				continue;
#ifdef CHINESE
			printf("\n-------------------------------------------\n");
			printf("  ROM set: %s\n", game_name);
			printf("-------------------------------------------\n\n");
#else
			printf("\n-------------------------------------------\n");
			printf("  ROM set: %s\n", game_name);
			printf("-------------------------------------------\n\n");
#endif

			chdir(launchDir);
			if (!convert_rom(game_name))
			{
#ifdef CHINESE
				printf("跳过.\n\n");
#else
				printf("Skip.\n\n");
#endif
			}
			else
			{
				if (create_raw_cache(game_name))
				{
#ifdef CHINESE
					printf("完成.\n\n");
#else
					printf("Done.\n\n");
#endif
				}
			}
			free_memory();
		}
#ifdef CHINESE
		printf("完成.\n");
		printf("请将cache内的文件夹复制到\"/PSP/GAMES/mvspsp/cache\".\n");
#else
		printf("complete.\n");
		printf("Please copy these files to directory \"/PSP/GAMES/mvspsp/cache\".\n");
#endif
	}
	else
	{
#ifdef WIN32
		if (!path_found)
		{
#ifdef CHINESE
			printf("请选择ROM文件.\n");
#else
			printf("Please select ROM file.\n");
#endif
			if (!file_dialog(NULL, "zip file (*.zip)\0*.zip\0", game_dir, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY))
				goto error;
		}
		else
		{
			strcpy(path, argv[path_found]);
			strcpy(game_dir, strtok(path, "\""));
		}
		convert_delimiter(game_dir);
#else
		strcpy(game_dir, argv[path_found]);
#endif

		if ((p = strrchr(game_dir, delimiter)) != NULL)
		{
			strcpy(game_name, p + 1);
			strcpy(zip_dir, game_dir);
			*strrchr(zip_dir, delimiter) = '\0';
		}
		else
		{
			strcpy(game_name, game_dir);
			strcpy(zip_dir, "");
		}

		p = game_name;
		while (*p)
		{
			*p = tolower(*p);
			*p++;
		}
#ifdef CHINESE
		printf("路径: %s\n", zip_dir);
		printf("文件名: %s\n", game_name);
#else
		printf("path: %s\n", zip_dir);
		printf("filename: %s\n", game_name);
#endif
		if ((p = strrchr(game_name, '.')) == NULL)
		{
#ifdef CHINESE
			printf("请输入正确的路径.\n");
#else
			printf("Please input correct path.\n");
#endif
			goto error;
		}
		*p = '\0';
#ifdef CHINESE
		printf("缓存文件夹名: cache%c%s_cache\n", delimiter, game_name);
#else
		printf("cache folder name: cache%c%s_cache\n", delimiter, game_name);
#endif

		convert_crom = 1;
		convert_srom = 1;
		convert_vrom = 1;

		chdir(launchDir);
		if (!convert_rom(game_name))
		{
			res = 0;
		}
		else
		{
			res = create_raw_cache(game_name);
		}
#ifdef WIN32
		if (res && pause)
#else
		if (res)
#endif
		{
#ifdef CHINESE
			printf("完成.\n");
			printf("请将\"cache%c%s_cache\"文件夹复制到\"/PSP/GAMES/mvspsp/cache\".\n", delimiter, game_name);
#else
			printf("complete.\n");
			printf("Please copy \"cache%c%s_cache\" folder to directory \"/PSP/GAMES/mvspsp/cache\".\n", delimiter, game_name);
#endif
		}
		free_memory();
	}

error:
#ifdef WIN32
	if (pause)
	{
#ifdef CHINESE
		printf("请按任意键退出.\n");
#else
		printf("Press any key to exit.\n");
#endif
		getch();
	}
#endif
	return res;
}

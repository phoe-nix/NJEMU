/******************************************************************************

	memintrf.c

	MVSメモリインタフェース関数

******************************************************************************/

#include "mvs.h"

#define M68K_AMASK 0x00ffffff
#define Z80_AMASK 0x0000ffff

#define READ_BYTE(mem, offset)			mem[offset ^ 1]
#define READ_WORD(mem, offset)			*(UINT16 *)&mem[offset]
#define WRITE_BYTE(mem, offset, data)	mem[offset ^ 1] = data
#define WRITE_WORD(mem, offset, data)	*(UINT16 *)&mem[offset] = data

#define READ_MIRROR_BYTE(mem, offset, amask)			mem[(offset & amask) ^ 1]
#define READ_MIRROR_WORD(mem, offset, amask)			*(UINT16 *)&mem[offset & amask]
#define WRITE_MIRROR_BYTE(mem, offset, data, amask)		mem[(offset & amask) ^ 1] = data
#define WRITE_MIRROR_WORD(mem, offset, data, amask)		*(UINT16 *)&mem[offset & amask] = data

#define str_cmp(s1, s2)		strnicmp(s1, s2, strlen(s2))


enum
{
	REGION_CPU1 = 0,
	REGION_CPU2,
	REGION_GFX1,
	REGION_GFX2,
	REGION_GFX3,
	REGION_GFX4,
	REGION_SOUND1,
	REGION_SOUND2,
	REGION_USER1,
#if !RELEASE
	REGION_USER2,
#endif
	REGION_USER3,
	REGION_SKIP
};

#define MAX_CPU1ROM		8
#define MAX_CPU2ROM		8
#define MAX_GFX2ROM		4
#define MAX_GFX3ROM		16
#define MAX_SND1ROM		8
#define MAX_SND2ROM		8
#define MAX_USR1ROM		1
#define MAX_USR2ROM		1


/******************************************************************************
	グローバル変数
******************************************************************************/

UINT8 *memory_region_cpu1;
UINT8 *memory_region_cpu2;
UINT8 *memory_region_gfx1;
UINT8 *memory_region_gfx2;
UINT8 *memory_region_gfx3;
UINT8 *memory_region_gfx4;
UINT8 *memory_region_sound1;
UINT8 *memory_region_sound2;
UINT8 *memory_region_user1;
#if !RELEASE
UINT8 *memory_region_user2;
#endif
UINT8 *memory_region_user3;

UINT32 memory_length_cpu1;
UINT32 memory_length_cpu2;
UINT32 memory_length_gfx1;
UINT32 memory_length_gfx2;
UINT32 memory_length_gfx3;
UINT32 memory_length_gfx4;
UINT32 memory_length_sound1;
UINT32 memory_length_sound2;
UINT32 memory_length_user1;
#if !RELEASE
UINT32 memory_length_user2;
#endif
UINT32 memory_length_user3;

UINT8 ALIGN_DATA neogeo_memcard[0x800];
UINT8 ALIGN_DATA neogeo_ram[0x10000];
UINT16 ALIGN_DATA neogeo_sram16[0x8000];

int neogeo_machine_mode;

int disable_sound;
int use_parent_crom;
int use_parent_srom;
int use_parent_vrom;

#ifdef PSP_SLIM
UINT32 psp2k_mem_offset = PSP2K_MEM_TOP;
INT32 psp2k_mem_left = PSP2K_MEM_SIZE;
#endif


/******************************************************************************
	ローカル構造体/変数
******************************************************************************/

static struct rom_t cpu1rom[MAX_CPU1ROM];
static struct rom_t cpu2rom[MAX_CPU2ROM];
static struct rom_t gfx2rom[MAX_GFX2ROM];
static struct rom_t gfx3rom[MAX_GFX3ROM];
static struct rom_t snd1rom[MAX_SND1ROM];
static struct rom_t snd2rom[MAX_SND2ROM];
static struct rom_t usr1rom[MAX_USR1ROM];
#if !RELEASE
static struct rom_t usr2rom[MAX_USR2ROM];
#endif

static int num_cpu1rom;
static int num_cpu2rom;
static int num_gfx2rom;
static int num_gfx3rom;
static int num_snd1rom;
static int num_snd2rom;
static int num_usr1rom;
#if !RELEASE
static int num_usr2rom;
#endif

static int encrypt_cpu1;
static int encrypt_cpu2;
static int encrypt_snd1;
static int encrypt_gfx2;
static int encrypt_gfx3;
static int encrypt_usr1;

static UINT32 bios_amask;

static UINT8 *neogeo_sram;


/******************************************************************************
	プロトタイプ
******************************************************************************/

static UINT16 (*neogeo_protection_r)(UINT32 offset, UINT16 mem_mask);
static void (*neogeo_protection_w)(UINT32 offset, UINT16 data, UINT16 mem_mask);


/****************************************************************************
	Decode GFX
****************************************************************************/

/*------------------------------------------------------
	Decode sprite (SPR)
------------------------------------------------------*/

static void neogeo_decode_spr(UINT8 *mem, UINT32 length, UINT8 *usage)
{
	UINT32 tileno, numtiles = length / 128;

	for (tileno = 0; tileno < numtiles; tileno++)
	{
		UINT8 swap[128];
		UINT8 *gfxdata;
		int x,y;
		UINT32 pen;
		int opaque = 0;

		gfxdata = &mem[128 * tileno];

		memcpy(swap, gfxdata, 128);

		for (y = 0;y < 16;y++)
		{
			UINT32 dw, data;

			dw = 0;
			for (x = 0; x < 8; x++)
			{
				pen  = ((swap[64 + 4*y + 3] >> x) & 1) << 3;
				pen |= ((swap[64 + 4*y + 1] >> x) & 1) << 2;
				pen |= ((swap[64 + 4*y + 2] >> x) & 1) << 1;
				pen |= ((swap[64 + 4*y + 0] >> x) & 1) << 0;
				if (pen & 0x0f) opaque++;
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
			for (x = 0; x < 8; x++)
			{
				pen  = ((swap[4*y + 3] >> x) & 1) << 3;
				pen |= ((swap[4*y + 1] >> x) & 1) << 2;
				pen |= ((swap[4*y + 2] >> x) & 1) << 1;
				pen |= ((swap[4*y + 0] >> x) & 1) << 0;
				if (pen & 0x0f) opaque++;
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
			*usage = 0;
		usage++;
	}
}

/*------------------------------------------------------
	Decode fixed sprite (FIX)
------------------------------------------------------*/

#define decode_fix(n)				\
{									\
	tile = buf[n];					\
	*p++ = tile;					\
	if (tile & 0x0f) opaque++;		\
	if (tile >> 4) opaque++;		\
}

#if !RELEASE
void neogeo_decode_fix(UINT8 *mem, UINT32 length, UINT8 *usage)
#else
static void neogeo_decode_fix(UINT8 *mem, UINT32 length, UINT8 *usage)
#endif
{
	UINT32 i, j;
	UINT8 tile, opaque;
	UINT8 *p, buf[32];
	UINT32 *gfx = (UINT32 *)mem;

	for (i = 0; i < length; i += 32)
	{
		opaque  = 0;

		memcpy(buf, &mem[i], 32);
		p = &mem[i];

		for (j = 0; j < 8; j++)
		{
			decode_fix(j + 16);
			decode_fix(j + 24);
			decode_fix(j +  0);
			decode_fix(j +  8);
		}

		if (opaque)
			*usage = (opaque == 64) ? SPRITE_OPAQUE : SPRITE_TRANSPARENT;
		else
			*usage = 0;
		usage++;
	}

	for (i = 0; i < length/4; i++)
	{
		UINT32 dw = gfx[i];
		UINT32 data = ((dw & 0x0000000f) >>  0) | ((dw & 0x000000f0) <<  4)
				 | ((dw & 0x00000f00) <<  8) | ((dw & 0x0000f000) << 12)
				 | ((dw & 0x000f0000) >> 12) | ((dw & 0x00f00000) >>  8)
				 | ((dw & 0x0f000000) >>  4) | ((dw & 0xf0000000) >>  0);
		gfx[i] = data;
	}
}


/*------------------------------------------------------
	Build zoom tables
------------------------------------------------------*/

static int build_zoom_tables(void)
{
	UINT8 *zoomy_rom;
	UINT8 *skip_table;
	UINT8 *tile_table;
	UINT8 *skip_fullmode0;
	UINT8 *tile_fullmode0;
	UINT8 *skip_fullmode1;
	UINT8 *tile_fullmode1;
	int zoom_y;

	if ((memory_region_user3 = memalign(MEM_ALIGN, 0x10000)) == NULL)
	{
		error_memory("REGION_USER3");
		return 0;
	}
	memset(memory_region_user3, 0, 0x10000);

	skip_fullmode0 = &memory_region_user3[0x100*0x40*0];
	tile_fullmode0 = &memory_region_user3[0x100*0x40*1];
	skip_fullmode1 = &memory_region_user3[0x100*0x40*2];
	tile_fullmode1 = &memory_region_user3[0x100*0x40*3];

	for (zoom_y = 0; zoom_y < 0x100; zoom_y++)
	{
		int row = 0;
		int prev_tile = 0;
		int sprite_line = 0;
		int tile_size = 0;

		zoomy_rom  = &memory_region_gfx4[zoom_y << 8];
		skip_table = &skip_fullmode0[zoom_y << 6];
		tile_table = &tile_fullmode0[zoom_y << 6];

		while (sprite_line <= 0x200)
		{
			int tile = 0;

			if (sprite_line < 0x200)
			{
				int zoom_line = sprite_line & 0xff;
				int invert = sprite_line & 0x100;

				if (invert)
					zoom_line ^= 0xff;

				tile = zoomy_rom[zoom_line] >> 4;

				if (invert)
					tile ^= 0x1f;
			}

			if (sprite_line == 0)
			{
				prev_tile = tile;
			}
			if (prev_tile != tile || sprite_line == 0x200)
			{
				tile_table[row] = prev_tile << 1;
				prev_tile = tile;
				row++;
			}
			if (sprite_line != 0x200)
			{
				skip_table[row]++;
			}

			sprite_line++;
		}

		skip_table = &skip_fullmode0[zoom_y << 6];

		for (row = 0; row < 0x0f; row++)
		{
			if (skip_table[row] > 0x10)
				break;

			tile_size += skip_table[row];
		}

		skip_table[0x3f] = (zoom_y + 1) - tile_size;
	}

	for (zoom_y = 0; zoom_y < 0x100; zoom_y++)
	{
		int row = 0;
		int prev_tile = 0;
		int sprite_line = 0;

		zoomy_rom  = &memory_region_gfx4[zoom_y << 8];
		skip_table = &skip_fullmode1[zoom_y << 6];
		tile_table = &tile_fullmode1[zoom_y << 6];

		while (sprite_line <= 0x200)
		{
			int tile = 0;

			if (sprite_line < 0x200)
			{
				int zoom_line = sprite_line & 0xff;
				int invert = sprite_line & 0x100;

				if (invert)
					zoom_line ^= 0xff;

				zoom_line %= (zoom_y + 1) << 1;
				if (zoom_line >= (zoom_y + 1))
				{
					zoom_line = ((zoom_y + 1) << 1) - 1 - zoom_line;
					invert = !invert;
				}

				tile = zoomy_rom[zoom_line] >> 4;

				if (invert)
					tile ^= 0x1f;
			}

			if (sprite_line == 0)
			{
				prev_tile = tile;
			}
			if (prev_tile != tile || sprite_line == 0x200)
			{
				tile_table[row] = prev_tile << 1;
				prev_tile = tile;

				if (row != 0 && (tile == 0 || skip_table[row] == 0))
					break;

				if (zoom_y == 0 && row == 2)
					break;

				row++;
			}
			if (sprite_line != 0x200)
			{
				skip_table[row]++;
			}

			sprite_line++;
		}
	}

	return 1;
}


/******************************************************************************
	PSP-2000用メモリ管理
******************************************************************************/

#ifdef PSP_SLIM

#define MEMORY_IS_PSP2K(mem)	((UINT32)mem >= PSP2K_MEM_TOP)

/*--------------------------------------------------------
	拡張された領域からメモリを確保
--------------------------------------------------------*/

static void *psp2k_mem_alloc(INT32 size)
{
	UINT8 *mem = NULL;

	if (size <= psp2k_mem_left)
	{
		mem = (UINT8 *)psp2k_mem_offset;
		psp2k_mem_offset += size;
		psp2k_mem_left -= size;
	}
	return mem;
}


/*--------------------------------------------------------
	拡張された領域へメモリを移動
--------------------------------------------------------*/

static void *psp2k_mem_move(void *mem, INT32 size)
{
	if (!mem) return NULL;

	if (size <= psp2k_mem_left)
	{
		memcpy((UINT8 *)psp2k_mem_offset, mem, size);
		free(mem);

		mem = (UINT8 *)psp2k_mem_offset;
		psp2k_mem_offset += size;
		psp2k_mem_left   -= size;
	}
	return mem;
}


/*--------------------------------------------------------
	メモリ範囲を確認してfree()
--------------------------------------------------------*/

static void psp2k_mem_free(void *mem)
{
	if (!mem || MEMORY_IS_PSP2K(mem))
		return;	// 拡張メモリは解放しない(フリーズする)

	free(mem);
}

#endif


/******************************************************************************
	ROM読み込み
******************************************************************************/

/*--------------------------------------------------------
	CPU1 (M68000 program ROM)
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

	if (encrypt_cpu1)
	{
		res = 1;

		switch (machine_init_type)
		{
		case INIT_kof99:    kof99_decrypt_68k();          break;
		case INIT_garou:    garou_decrypt_68k();          break;
		case INIT_garouo:   garouo_decrypt_68k();         break;
		case INIT_mslug3:   mslug3_decrypt_68k();         break;
		case INIT_kof2000:  kof2000_decrypt_68k();        break;

		case INIT_kof98:    res = kof98_decrypt_68k();    break;
		case INIT_kof2002:  res = kof2002_decrypt_68k();  break;
		case INIT_mslug5:   res = mslug5_decrypt_68k();   break;
		case INIT_svchaosa: res = svcchaos_px_decrypt();  break;
		case INIT_samsho5:  res = samsho5_decrypt_68k();  break;
		case INIT_kof2003:  res = kof2003_decrypt_68k();  break;
		case INIT_samsh5sp: res = samsh5p_decrypt_68k();  break;
		case INIT_matrim:   res = kof2002_decrypt_68k();  break;

		case INIT_ms5pcb:   res = mslug5_decrypt_68k();   break;
		case INIT_svcpcb:   res = svcchaos_px_decrypt();  break;
		case INIT_kf2k3pcb: res = kf2k3pcb_decrypt_68k(); break;

#if !RELEASE
		case INIT_kof96ep:  kof96ep_px_decrypt();         break;
		case INIT_cthd2k3a: res = cthd2k3a_px_decrypt();  break;
		case INIT_kof2002b: res = kof2002_decrypt_68k();  break;
		case INIT_kf2k2pls: res = kof2002_decrypt_68k();  break;
		case INIT_kf2k2plc: res = kof2002_decrypt_68k();  break;
		case INIT_kf2k2mp:  res = kf2k2mp_px_decrypt();   break;
		case INIT_kf2k2mp2: res = kf2k2mp2_px_decrypt();  break;
		case INIT_svcboot:  res = svcboot_px_decrypt();   break;
		case INIT_svcplus:  res = svcplus_px_decrypt();   break;
		case INIT_svcplusa: res = svcplusa_px_decrypt();  break;
		case INIT_svcsplus: res = svcsplus_px_decrypt();  break;
		case INIT_samsho5b: res = samsho5b_px_decrypt();  break;
		case INIT_kf2k3bl:  res = kf2k3bl_px_decrypt();   break;
		case INIT_kf2k3pl:  res = kf2k3pl_px_decrypt();   break;
		case INIT_kf2k3upl: kf2k3upl_px_decrypt();        break;
		case INIT_kog:      res = kog_px_decrypt();       break;
		case INIT_kof10th:  res = kof10th_px_decrypt();   break;
		case INIT_kf10thep: res = kf10thep_px_decrypt();  break;
		case INIT_kf2k5uni: res = kf2k5uni_px_decrypt();  break;
		case INIT_kof2k4se: res = kof2k4se_px_decrypt();  break;
		case INIT_kf2k4pls: res = kf2k4pls_px_decrypt();  break;
		case INIT_lans2004: res = lans2004_px_decrypt();  break;
		case INIT_matrimbl: res = kof2002_decrypt_68k();  break;
#endif

		default: res = 0;
		}

		if (res == 0)
		{
			msg_printf(TEXT(COULD_NOT_ALLOCATE_MEMORY_FOR_DECRYPT_ROM));
			msg_printf(TEXT(PRESS_ANY_BUTTON2));
			pad_wait_press(PAD_WAIT_INFINITY);
			Loop = LOOP_BROWSER;
			return 0;
		}
	}

	neogeo_ngh = m68000_read_memory_16(0x0108);

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

	parent = strlen(parent_name) ? parent_name : (char *)"neogeo";

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

#if !RELEASE
	if (encrypt_cpu2)
	{
		switch (machine_init_type)
		{
		case INIT_cthd2003: cthd2003_mx_decrypt(); break;
		case INIT_cthd2k3a: cthd2003_mx_decrypt(); break;
		case INIT_ct2k3sp:  cthd2003_mx_decrypt(); break;
		case INIT_ct2k3sa:  cthd2003_mx_decrypt(); break;
		case INIT_kf2k5uni: kf2k5uni_mx_decrypt(); break;
		case INIT_matrimbl: matrimbl_mx_decrypt(); break;
		}
	}
#endif

	memcpy(memory_region_cpu2, &memory_region_cpu2[0x10000], 0x10000);

	return 1;
}

/*--------------------------------------------------------
	GFX1 (System FIX sprite ROM)
--------------------------------------------------------*/

static int load_rom_gfx1(void)
{
	int res;
	char fname[32];

	if ((memory_region_gfx1 = memalign(MEM_ALIGN, memory_length_gfx1)) == NULL)
	{
		error_memory("REGION_GFX1");
		return 0;
	}
	memset(memory_region_gfx1, 0, memory_length_gfx1);

	strcpy(fname, sfix_name);
	if ((res = file_open(game_name, bios_zip, sfix_crc, fname)) < 0)
	{
		if (res == -1)
			error_file(fname);
		else
			error_crc(fname);
		return 0;
	}

	msg_printf(TEXT(LOADING), fname);
	file_read(memory_region_gfx1, memory_length_gfx1);
	file_close();

	neogeo_decode_fix(memory_region_gfx1, memory_length_gfx1, gfx_pen_usage[0]);

	return 1;
}

/*--------------------------------------------------------
	GFX2 (FIX sprite ROM)
--------------------------------------------------------*/

static int load_rom_gfx2(void)
{
	if ((memory_region_gfx2 = memalign(MEM_ALIGN, memory_length_gfx2)) == NULL)
	{
		error_memory("REGION_GFX2");
		return 0;
	}
	memset(memory_region_gfx2, 0, memory_length_gfx2);

	if (encrypt_gfx2)
	{
		SceUID fd;

		if ((fd = cachefile_open(CACHE_SROM)) < 0)
		{
			error_file("cache/srom");
			return 0;
		}

		msg_printf(TEXT(LOADING_DECRYPTED_GFX2_ROM));
		sceIoRead(fd, memory_region_gfx2, memory_length_gfx2);
		sceIoClose(fd);
	}
	else
	{
		int i, res;
		char fname[32], *parent;

		parent = strlen(parent_name) ? parent_name : (char *)bios_zip;

		for (i = 0; i < num_gfx2rom; )
		{
			strcpy(fname, gfx2rom[i].name);
			if ((res = file_open(game_name, parent, gfx2rom[i].crc, fname)) < 0)
			{
				if (res == -1)
					error_file(fname);
				else
					error_crc(fname);
				return 0;
			}

			msg_printf(TEXT(LOADING), fname);

			i = rom_load(gfx2rom, memory_region_gfx2, i, num_gfx2rom);

			file_close();
		}

#if !RELEASE
		switch (machine_init_type)
		{
		case INIT_cthd2003: cthd2003_sx_decrypt(); break;
		case INIT_cthd2k3a: cthd2003_sx_decrypt(); break;
		case INIT_ct2k3sp:  cthd2003_sx_decrypt(); break;
		case INIT_kf2k1pa:  kf2k1pa_sx_decrypt();  break;
		case INIT_kf2k5uni: kf2k5uni_sx_decrypt(); break;
		case INIT_kf10thep: kf10thep_sx_decrypt(); break;
		}
#endif
	}

	neogeo_decode_fix(memory_region_gfx2, memory_length_gfx2, gfx_pen_usage[1]);

	return 1;
}

/*--------------------------------------------------------
	GFX3 (OBJ sprite ROM)
--------------------------------------------------------*/

static int load_rom_gfx3(void)
{
	if (!encrypt_gfx3)
	{
#ifdef PSP_SLIM
		if ((memory_region_gfx3 = psp2k_mem_alloc(memory_length_gfx3)) == NULL)
#endif
		{
			memory_region_gfx3 = memalign(MEM_ALIGN, memory_length_gfx3);
		}

		if (memory_region_gfx3 != NULL)
		{
			int i, res;
			char fname[32], *parent;

			memset(memory_region_gfx3, 0, memory_length_gfx3);

			parent = strlen(parent_name) ? parent_name : NULL;

			for (i = 0; i < num_gfx3rom; )
			{
				strcpy(fname, gfx3rom[i].name);
				if ((res = file_open(game_name, parent, gfx3rom[i].crc, fname)) < 0)
				{
					if (res == -1)
						error_file(fname);
					else
						error_crc(fname);
					return 0;
				}

				msg_printf(TEXT(LOADING), fname);

				i = rom_load(gfx3rom, memory_region_gfx3, i, num_gfx3rom);

				file_close();
			}

			neogeo_decode_spr(memory_region_gfx3, memory_length_gfx3, gfx_pen_usage[2]);
		}
		else
		{
			msg_printf(TEXT(COULD_NOT_ALLOCATE_MEMORY_FOR_SPRITE_DATA));
			msg_printf(TEXT(TRY_TO_USE_SPRITE_CACHE));
		}
	}

	if (memory_region_gfx3 == NULL)
	{
		if (cache_start() == 0)
		{
			msg_printf(TEXT(PRESS_ANY_BUTTON2));
			pad_wait_press(PAD_WAIT_INFINITY);
			Loop = LOOP_BROWSER;
			return 0;
		}
	}

	return 1;
}

/*--------------------------------------------------------
	GFX4 (lo ROM)
--------------------------------------------------------*/

static int load_rom_gfx4(void)
{
	int res;
	char fname[32];

	if ((memory_region_gfx4 = memalign(MEM_ALIGN, memory_length_gfx4)) == NULL)
	{
		error_memory("REGION_GFX4");
		return 0;
	}
	memset(memory_region_gfx4, 0, memory_length_gfx4);

	strcpy(fname, lorom_name);
	if ((res = file_open(game_name, bios_zip, lorom_crc, fname)) < 0)
	{
		if (res == -1)
			error_file(fname);
		else
			error_crc(fname);
		return 0;
	}

	msg_printf(TEXT(LOADING), fname);
	file_read(memory_region_gfx4, memory_length_gfx4);
	file_close();

	return build_zoom_tables();
}


/*--------------------------------------------------------
	SOUND1 (YM2610 ADPCM-A ROM)
--------------------------------------------------------*/

static int load_rom_sound1(void)
{
	if (!option_sound_enable)
	{
		memory_length_sound1 = 0;
		return 1;
	}
#ifndef PSP_SLIM
	if (disable_sound)
	{
		return 1;
	}

	if ((memory_region_sound1 = memalign(MEM_ALIGN, memory_length_sound1)) == NULL)
	{
		error_memory("REGION_SOUND1");
		return 0;
	}
#else
	if ((memory_region_sound1 = memalign(MEM_ALIGN, memory_length_sound1)) == NULL)
	{
		if (disable_sound)
		{
			if ((memory_region_sound1 = psp2k_mem_alloc(memory_length_sound1)) == NULL)
			{
				return 1;
			}
		}
		else
		{
			error_memory("REGION_SOUND1");
			return 0;
		}
	}

	disable_sound = 0;
#endif

	memset(memory_region_sound1, 0, memory_length_sound1);

	if (encrypt_snd1)
	{
		SceUID fd;

		if ((fd = cachefile_open(CACHE_VROM)) < 0)
		{
			error_file("cache/vrom");
			return 0;
		}

		msg_printf(TEXT(LOADING_DECRYPTED_SOUND1_ROM));
		sceIoRead(fd, memory_region_sound1, memory_length_sound1);
		sceIoClose(fd);
	}
	else
	{
		int i, res;
		char fname[32], *parent;

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
	}

	return 1;
}

/*--------------------------------------------------------
	SOUND2 (YM2610 Delta-T PCM ROM)
--------------------------------------------------------*/

static int load_rom_sound2(void)
{
	int i, res;
	char fname[32], *parent;

	if (memory_length_sound2 == 0 || !option_sound_enable || disable_sound)
	{
		memory_length_sound2 = 0;
		return 1;
	}

	if ((memory_region_sound2 = memalign(MEM_ALIGN, memory_length_sound2)) == NULL)
	{
		error_memory("REGION_SOUND2");
		return 0;
	}
	memset(memory_region_sound2, 0, memory_length_sound2);

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_snd2rom; )
	{
		strcpy(fname, snd2rom[i].name);
		if ((res = file_open(game_name, parent, snd2rom[i].crc, fname)) < 0)
		{
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		i = rom_load(snd2rom, memory_region_sound2, i, num_snd2rom);

		file_close();
	}

	return 1;
}

/*--------------------------------------------------------
	USER1 (M68000 BIOS ROM)
--------------------------------------------------------*/

static int load_rom_user1(int reload)
{
	int res;
	char fname[32];
	char *parent;
	UINT32 patch = 0;

	if (!reload)
	{
		if ((memory_region_user1 = memalign(MEM_ALIGN, memory_length_user1)) == NULL)
		{
			error_memory("REGION_USER1");
			return 0;
		}
		memset(memory_region_user1, 0, memory_length_user1);
	}

	parent = strlen(parent_name) ? parent_name : (char *)"neogeo";

	if (!num_usr1rom)
	{
		patch = bios_patch_address[neogeo_bios];

		strcpy(fname, bios_name[neogeo_bios]);
		if ((res = file_open(game_name, bios_zip, bios_crc[neogeo_bios], fname)) < 0)
		{
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
			neogeo_bios = -1;
			return 0;
		}
		if (!reload) msg_printf(TEXT(LOADING_BIOS), fname, bios_name[neogeo_bios]);
		file_read(memory_region_user1, memory_length_user1);
		file_close();
	}
	else if (!reload)
	{
		if (!strcmp(game_name, "irrmaze")) patch = 0x010d8c;

		strcpy(fname, usr1rom[0].name);
		if ((res = file_open(game_name, parent, usr1rom[0].crc, fname)) < 0)
		{
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		file_read(memory_region_user1, memory_length_user1);
		file_close();

		if (encrypt_usr1)
		{
			if (machine_init_type == INIT_kf2k3pcb)
			{
				if (kof2003biosdecode() == 0)
				{
					msg_printf(TEXT(COULD_NOT_ALLOCATE_MEMORY_FOR_DECRYPT_ROM));
					msg_printf(TEXT(PRESS_ANY_BUTTON2));
					pad_wait_press(PAD_WAIT_INFINITY);
					Loop = LOOP_BROWSER;
					return 0;
				}
			}
		}
	}

	bios_amask = memory_length_user1 - 1;

	if (patch)
	{
		UINT16 *mem16 = (UINT16 *)memory_region_user1;
		UINT16 value;

		if (!neogeo_region)
			value = mem16[0x00400 >> 1] & 0x03;
		else
			value = neogeo_region - 1;

		if (!neogeo_machine_mode)
			value |= mem16[0x00400 >> 1] & 0x8000;
		else
			value |= (neogeo_machine_mode - 1) ? 0x8000 : 0;

		mem16[0x00400 >> 1] = value;
		mem16[(patch + 0) >> 1] = 0x4e71;
		mem16[(patch + 2) >> 1] = 0x4e71;
	}

	return 1;
}

/*--------------------------------------------------------
	USER2 (kof10th/kf10thep専用)
--------------------------------------------------------*/

#if !RELEASE
static int load_rom_user2(void)
{
	int i, res;
	char fname[32], *parent;

	if (memory_length_user2 == 0)
	{
		return 1;
	}

	if ((memory_region_user2 = memalign(MEM_ALIGN, memory_length_user2)) == NULL)
	{
		error_memory("REGION_USER2");
		return 0;
	}
	memset(memory_region_user2, 0, memory_length_user2);

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_usr2rom; )
	{
		strcpy(fname, usr2rom[i].name);
		if ((res = file_open(game_name, parent, usr2rom[i].crc, fname)) < 0)
		{
			if (res == -1)
				error_file(fname);
			else
				error_crc(fname);
			return 0;
		}

		msg_printf(TEXT(LOADING), fname);

		i = rom_load(usr2rom, memory_region_user2, i, num_usr2rom);

		file_close();
	}

	return 1;
}
#endif

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
	num_gfx2rom = 0;
	num_gfx3rom = 0;
	num_snd1rom = 0;
	num_snd2rom = 0;
	num_usr1rom = 0;
#if !RELEASE
	num_usr2rom = 0;
#endif

	encrypt_cpu1 = 0;
	encrypt_cpu2 = 0;
	encrypt_snd1 = 0;
	encrypt_gfx2 = 0;
	encrypt_gfx3 = 0;
	encrypt_usr1 = 0;

	disable_sound = 0;

	sprintf(path, "%srominfo.mvs", launchDir);

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

					if (stricmp(name, game_name) == 0)
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
					int encrypted = 0;

					strtok(&linebuf[1], " ");
					size = strtok(NULL, " ,");
					type = strtok(NULL, " ,");
					flag = strtok(NULL, " ");

					if (strstr(flag, "SOUND_DISABLE")) disable_sound = 1;
					if (strstr(flag, "ENCRYPTED")) encrypted = 1;

					if (strcmp(type, "CPU1") == 0)
					{
						sscanf(size, "%x", &memory_length_cpu1);
						encrypt_cpu1 = encrypted;
						region = REGION_CPU1;
					}
					else if (strcmp(type, "CPU2") == 0)
					{
						sscanf(size, "%x", &memory_length_cpu2);
						encrypt_cpu2 = encrypted;
						region = REGION_CPU2;
					}
					else if (strcmp(type, "GFX2") == 0)
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
					else if (strcmp(type, "SOUND2") == 0)
					{
						sscanf(size, "%x", &memory_length_sound2);
						region = REGION_SOUND2;
					}
					else if (strcmp(type, "USER1") == 0)
					{
						sscanf(size, "%x", &memory_length_user1);
						encrypt_usr1 = encrypted;
						region = REGION_USER1;
					}
#if !RELEASE
					else if (strcmp(type, "USER2") == 0)
					{
						sscanf(size, "%x", &memory_length_user2);
						region = REGION_USER2;
					}
#endif
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

					case REGION_SOUND2:
						sscanf(type, "%x", &snd2rom[num_snd2rom].type);
						sscanf(offset, "%x", &snd2rom[num_snd2rom].offset);
						sscanf(length, "%x", &snd2rom[num_snd2rom].length);
						sscanf(crc, "%x", &snd2rom[num_snd2rom].crc);
						if (name) strcpy(snd2rom[num_snd2rom].name, name);
						snd2rom[num_snd2rom].group = 0;
						snd2rom[num_snd2rom].skip = 0;
						num_snd2rom++;
						break;

					case REGION_USER1:
						sscanf(type, "%x", &usr1rom[num_usr1rom].type);
						sscanf(offset, "%x", &usr1rom[num_usr1rom].offset);
						sscanf(length, "%x", &usr1rom[num_usr1rom].length);
						sscanf(crc, "%x", &usr1rom[num_usr1rom].crc);
						if (name) strcpy(usr1rom[num_usr1rom].name, name);
						usr1rom[num_usr1rom].group = 0;
						usr1rom[num_usr1rom].skip = 0;
						num_usr1rom++;
						break;

#if !RELEASE
					case REGION_USER2:
						sscanf(type, "%x", &usr2rom[num_usr2rom].type);
						sscanf(offset, "%x", &usr2rom[num_usr2rom].offset);
						sscanf(length, "%x", &usr2rom[num_usr2rom].length);
						sscanf(crc, "%x", &usr2rom[num_usr2rom].crc);
						if (name) strcpy(usr2rom[num_usr2rom].name, name);
						usr2rom[num_usr2rom].group = 0;
						usr2rom[num_usr2rom].skip = 0;
						num_usr2rom++;
						break;
#endif
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
	int res;

	memory_region_cpu1   = NULL;
	memory_region_cpu2   = NULL;
	memory_region_gfx1   = NULL;
	memory_region_gfx2   = NULL;
	memory_region_gfx3   = NULL;
	memory_region_gfx4   = NULL;
	memory_region_sound1 = NULL;
	memory_region_sound2 = NULL;
	memory_region_user1  = NULL;
#if !RELEASE
	memory_region_user2  = NULL;
#endif
	memory_region_user3  = NULL;

	memory_length_cpu1   = 0;
	memory_length_cpu2   = 0;
	memory_length_gfx1   = 0x20000;
	memory_length_gfx2   = 0;
	memory_length_gfx3   = 0;
	memory_length_gfx4   = 0x10000;
	memory_length_sound1 = 0;
	memory_length_sound2 = 0;
	memory_length_user1  = 0x20000;
#if !RELEASE
	memory_length_user2  = 0;
#endif
	memory_length_user3  = 0x10000;

	gfx_pen_usage[0] = NULL;
	gfx_pen_usage[1] = NULL;
	gfx_pen_usage[2] = NULL;

#ifdef PSP_SLIM
	psp2k_mem_offset = PSP2K_MEM_TOP;
	psp2k_mem_left   = PSP2K_MEM_SIZE;
#endif

#ifdef ADHOC
	if (adhoc_enable)
	{
		bios_select(2);

		if (neogeo_bios == -1)
			return 0;
	}
#endif

	cache_init();
	pad_wait_clear();
	video_clear_screen();
	msg_screen_init(WP_LOGO, ICON_SYSTEM, TEXT(LOAD_ROM));

	load_gamecfg(game_name);

	memset(neogeo_memcard, 0, 0x800);
	memset(neogeo_ram, 0, 0x10000);
	memset(neogeo_sram16, 0, 0x10000);

#ifdef ADHOC
	if (adhoc_enable)
	{
		/* AdHoc通信時は一部オプションで固定の設定を使用 */
		neogeo_raster_enable = 0;
		psp_cpuclock         = PSPCLOCK_333;
		option_vsync         = 0;
		option_autoframeskip = 0;
		option_frameskip     = 0;
		option_showfps       = 0;
		option_speedlimit    = 1;
		option_sound_enable  = 1;
		option_samplerate    = 1;
		neogeo_dipswitch     = 0xff;
	}
#endif

	set_cpu_clock(psp_cpuclock);

	msg_printf(TEXT(CHECKING_BIOS));

	if ((res = neogeo_bios) != -1)
	{
		res = file_open(bios_zip, NULL, bios_crc[neogeo_bios], NULL);
		file_close();
	}
	if (res < 0)
	{
		pad_wait_clear();
		video_clear_screen();
		bios_select(1);
		if (neogeo_bios == -1)
		{
			Loop = LOOP_BROWSER;
			return 0;
		}

		pad_wait_clear();
		video_clear_screen();
		msg_screen_init(WP_LOGO, ICON_SYSTEM, TEXT(LOAD_ROM));
		msg_printf(TEXT(CHECKING_BIOS));
		msg_printf(TEXT(ALL_NVRAM_FILES_ARE_REMOVED));
	}

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

	if (parent_name[0])
	{
		int i = 0;

		use_parent_crom = 1;
		use_parent_srom = 1;
		use_parent_vrom = 1;

		while (MVS_cacheinfo[i].name)
		{
			if (strcmp(game_name, MVS_cacheinfo[i].name) == 0)
			{
				use_parent_crom = !MVS_cacheinfo[i].crom;
				use_parent_srom = !MVS_cacheinfo[i].srom;
				use_parent_vrom = !MVS_cacheinfo[i].vrom;
				break;
			}
			i++;
		}

		msg_printf(TEXT(ROMSET_x_PARENT_x), game_name, parent_name);
	}
	else
	{
		use_parent_crom = 0;
		use_parent_srom = 0;
		use_parent_vrom = 0;

		msg_printf(TEXT(ROMSET_x), game_name);
	}

#ifdef ADHOC
	if (!adhoc_enable)
#endif
	{
#ifdef COMMAND_LIST
		if (parent_name[0])
			load_commandlist(game_name, parent_name);
		else
			load_commandlist(game_name, NULL);
#endif
	}

	if ((gfx_pen_usage[0] = memalign(MEM_ALIGN, memory_length_gfx1 / 32)) == NULL)
	{
		error_memory("GFX_PEN_USAGE (sfix)");
		return 0;
	}
	if ((gfx_pen_usage[1] = memalign(MEM_ALIGN, memory_length_gfx2 / 32)) == NULL)
	{
		error_memory("GFX_PEN_USAGE (fix)");
		return 0;
	}
	if ((gfx_pen_usage[2] = memalign(MEM_ALIGN, memory_length_gfx3 / 128)) == NULL)
	{
		error_memory("GFX_PEN_USAGE (spr)");
		return 0;
	}

#if !RELEASE
	if (load_rom_user2() == 0) return 0;
#endif
	if (load_rom_cpu1() == 0) return 0;
	if (load_rom_user1(0) == 0) return 0;
	if (load_rom_cpu2() == 0) return 0;
	if (load_rom_gfx1() == 0) return 0;
	if (load_rom_gfx2() == 0) return 0;
	if (load_rom_gfx4() == 0) return 0;

	if (load_rom_sound1() == 0) return 0;

#ifdef PSP_SLIM
	if (psp2k_mem_left != PSP2K_MEM_SIZE)
	{
		// sound1で拡張メモリに確保した場合

		// キャッシュ領域を極力多く取るためにこれまで確保したメモリで移動可能なものを
		// 拡張メモリに移動する。
		// 極力連続した大きな領域を空けたいので、確保したのと逆の順で移動。

		memory_region_user3 = psp2k_mem_move(memory_region_user3, memory_length_user3);
		memory_region_gfx4  = psp2k_mem_move(memory_region_gfx4,  memory_length_gfx4);
		memory_region_gfx2  = psp2k_mem_move(memory_region_gfx2,  memory_length_gfx2);
		memory_region_gfx1  = psp2k_mem_move(memory_region_gfx1,  memory_length_gfx1);
		memory_region_cpu2  = psp2k_mem_move(memory_region_cpu2,  memory_length_cpu2);
		memory_region_user1 = psp2k_mem_move(memory_region_user1, memory_length_user1);
		memory_region_cpu1  = psp2k_mem_move(memory_region_cpu1,  memory_length_cpu1);

		gfx_pen_usage[2] = psp2k_mem_move(gfx_pen_usage[2], memory_length_gfx3 / 128);
		gfx_pen_usage[1] = psp2k_mem_move(gfx_pen_usage[1], memory_length_gfx2 / 32);
		gfx_pen_usage[0] = psp2k_mem_move(gfx_pen_usage[0], memory_length_gfx1 / 32);
	}
#endif

	if (load_rom_sound2() == 0) return 0;
	if (load_rom_gfx3() == 0) return 0;

	if (disable_sound)
	{
		neogeo_save_sound_flag = option_sound_enable;
		option_sound_enable = 0;
	}
	else
	{
		neogeo_save_sound_flag = 0;
	}

	// FIXバンクタイプ設定
	switch (machine_init_type)
	{
	case INIT_garou:
	case INIT_garouo:
	case INIT_mslug4:
	case INIT_mslug3:
	case INIT_mslug3n:
	case INIT_samsho5:
	case INIT_samsh5sp:
		neogeo_fix_bank_type = 1;
		break;

	case INIT_kof2000:
	case INIT_kof2000n:
	case INIT_matrim:
	case INIT_svcpcb:
	case INIT_svchaosa:
	case INIT_kf2k3pcb:
	case INIT_kof2003:
#if !RELEASE
	case INIT_matrimbl:
#endif
		neogeo_fix_bank_type = 2;
		break;

	default:
		neogeo_fix_bank_type = 0;
		break;
	}

	neogeo_protection_r = neogeo_secondbank_r;
	neogeo_protection_w = neogeo_secondbank_w;

	switch (machine_init_type)
	{
	case INIT_fatfury2:
		neogeo_protection_r = fatfury2_protection_r;
		neogeo_protection_w = fatfury2_protection_w;
		break;

	case INIT_kof98:
		neogeo_protection_r = neogeo_secondbank_r;
		neogeo_protection_w = kof98_protection_w;
		break;

	case INIT_mslugx:
		mslugx_install_protection();
		break;

	case INIT_kof99:
		neogeo_protection_r = kof99_protection_r;
		neogeo_protection_w = kof99_protection_w;
		break;

	case INIT_garou:
		neogeo_protection_r = garou_protection_r;
		neogeo_protection_w = garou_protection_w;
		break;

	case INIT_garouo:
		neogeo_protection_r = garou_protection_r;
		neogeo_protection_w = garouo_protection_w;
		break;

	case INIT_mslug3:
		neogeo_protection_r = mslug3_protection_r;
		neogeo_protection_w = mslug3_protection_w;
		break;

	case INIT_kof2000:
		neogeo_protection_r = kof2000_protection_r;
		neogeo_protection_w = kof2000_protection_w;
		kof2000_AES_protection();
		break;

	case INIT_mslug5:
	case INIT_ms5pcb:
		neogeo_protection_r = pvc_protection_r;
		neogeo_protection_w = pvc_protection_w;
		mslug5_AES_protection();
		break;

	case INIT_svcpcb:
	case INIT_svchaosa:
	case INIT_kof2003:
	case INIT_kf2k3pcb:
		neogeo_protection_r = pvc_protection_r;
		neogeo_protection_w = pvc_protection_w;
		break;

	case INIT_vliner:
		neogeo_protection_r = vliner_r;
		neogeo_protection_w = brza_sram_w;
		neogeo_ngh = NGH_vliner;
		break;

	case INIT_jockeygp:
		neogeo_protection_r = brza_sram_r;
		neogeo_protection_w = brza_sram_w;
		neogeo_ngh = NGH_jockeygp;
		break;

	case INIT_nitd:
		nitd_AES_protection();
		break;

	case INIT_zupapa:
		zupapa_AES_protection();
		break;

	case INIT_sengoku3:
		sengoku3_AES_protection();
		break;

	case INIT_mslug4:
		mslug4_AES_protection();
		break;

	case INIT_rotd:
		rotd_AES_protection();
		break;

	case INIT_matrim:
		matrim_AES_protection();
		break;

#if !RELEASE
	case INIT_kof97pla:
		patch_kof97pla();
		break;

	case INIT_cthd2003:
	case INIT_ct2k3sp:
		patch_cthd2003();
		neogeo_protection_w = cthd2003_protection_w;
		cthd2003_AES_protection();
		break;

	case INIT_ct2k3sa:
		patch_cthd2003();
		cthd2003_AES_protection();
		break;

	case INIT_cthd2k3a:
		cthd2003_AES_protection();
		break;

	case INIT_mslug5b:
		neogeo_protection_r = pvc_protection_r;
		neogeo_protection_w = pvc_protection_w;
		mslug5_AES_protection();
		break;

	case INIT_ms5plus:
		neogeo_protection_r = ms5plus_protection_r;
		neogeo_protection_w = ms5plus_protection_w;
		mslug5_AES_protection();
		break;

	case INIT_kf2k3bl:
	case INIT_kf2k3upl:
		neogeo_protection_r = pvc_protection_r;
		neogeo_protection_w = kf2k3bl_protection_w;
		break;

	case INIT_kf2k3pl:
		neogeo_protection_r = pvc_protection_r;
		neogeo_protection_w = kf2k3pl_protection_w;
		break;

	case INIT_svcboot:
	case INIT_svcsplus:
		neogeo_protection_r = pvc_protection_r;
		neogeo_protection_w = pvc_protection_w;
		break;

	case INIT_kof10th:
		neogeo_protection_r = pvc_protection_r;
		neogeo_protection_w = kof10th_protection_w;
		break;

	case INIT_matrimbl:
		matrim_AES_protection();
		break;

	case INIT_ms4plus:
		mslug4_AES_protection();
		break;

	case INIT_fr2ch:
		patch_fr2ch();
		neogeo_protection_w = fr2ch_protection_w;
		break;
#endif
	}

	neogeo_sram = (UINT8 *)neogeo_sram16;

	return 1;
}


/*------------------------------------------------------
	メモリインタフェース終了
------------------------------------------------------*/

void memory_shutdown(void)
{
	int i;

	cache_shutdown();

#ifdef PSP_SLIM
	for (i = 0; i < 3; i++)
		psp2k_mem_free(gfx_pen_usage[i]);

	psp2k_mem_free(memory_region_cpu1);
	psp2k_mem_free(memory_region_cpu2);
	psp2k_mem_free(memory_region_gfx1);
	psp2k_mem_free(memory_region_gfx2);
	psp2k_mem_free(memory_region_gfx3);
	psp2k_mem_free(memory_region_gfx4);
	psp2k_mem_free(memory_region_sound1);
	psp2k_mem_free(memory_region_sound2);
	psp2k_mem_free(memory_region_user1);
#if !RELEASE
	psp2k_mem_free(memory_region_user2);
#endif
	psp2k_mem_free(memory_region_user3);

	psp2k_mem_offset = PSP2K_MEM_TOP + PSP2K_MEM_SIZE;
	psp2k_mem_left   = PSP2K_MEM_SIZE;
#else
	for (i = 0; i < 3; i++)
	{
		if (gfx_pen_usage[i])
			free(gfx_pen_usage[i]);
	}

	if (memory_region_cpu1)   free(memory_region_cpu1);
	if (memory_region_cpu2)   free(memory_region_cpu2);
	if (memory_region_gfx1)   free(memory_region_gfx1);
	if (memory_region_gfx2)   free(memory_region_gfx2);
	if (memory_region_gfx3)   free(memory_region_gfx3);
	if (memory_region_gfx4)   free(memory_region_gfx4);
	if (memory_region_sound1) free(memory_region_sound1);
	if (memory_region_sound2) free(memory_region_sound2);
	if (memory_region_user1)  free(memory_region_user1);
#if !RELEASE
	if (memory_region_user2)  free(memory_region_user2);
#endif
	if (memory_region_user3)  free(memory_region_user3);
#endif

#if PSP_VIDEO_32BPP
	GFX_MEMORY = NULL;
#endif
}


/******************************************************************************
	M68000 メモリリード/ライト関数
******************************************************************************/

/*------------------------------------------------------
	M68000メモリリード (byte)
------------------------------------------------------*/

UINT8 m68000_read_memory_8(UINT32 offset)
{
	int shift = (~offset & 1) << 3;
	UINT16 mem_mask = ~(0xff << shift);

	offset &= M68K_AMASK;

	switch (offset >> 20)
	{
	case 0x0: return READ_BYTE(memory_region_cpu1, offset);
	case 0x1: return READ_MIRROR_BYTE(neogeo_ram, offset, 0x00ffff);
	case 0xc: return READ_MIRROR_BYTE(memory_region_user1, offset, bios_amask);
	case 0xd: return READ_MIRROR_BYTE(neogeo_sram, offset, 0x00ffff);

	case 0x2: return (*neogeo_protection_r)(offset >> 1, mem_mask) >> shift;
	case 0x4: return neogeo_paletteram_r(offset >> 1, mem_mask) >> shift;
	case 0x8: return neogeo_memcard16_r(offset >> 1, mem_mask) >> shift;

	case 0x3:
		switch (offset >> 16)
		{
		case 0x30: return neogeo_controller1and4_r(offset >> 1, mem_mask) >> shift;
		case 0x32: return neogeo_timer_r(offset >> 1, mem_mask) >> shift;
		case 0x34: return neogeo_controller2_r(offset >> 1, mem_mask) >> shift;
		case 0x38: return neogeo_controller3_r(offset >> 1, mem_mask) >> shift;
		case 0x3c: return neogeo_video_register_r(offset >> 1, mem_mask) >> shift;
		}
		break;
	}
	return 0xff;
}


/*------------------------------------------------------
	M68000リードメモリ (word)
------------------------------------------------------*/

UINT16 m68000_read_memory_16(UINT32 offset)
{
	offset &= M68K_AMASK;

	switch (offset >> 20)
	{
	case 0x0: return READ_WORD(memory_region_cpu1, offset);
	case 0x1: return READ_MIRROR_WORD(neogeo_ram, offset, 0x00ffff);
	case 0xc: return READ_MIRROR_WORD(memory_region_user1, offset, bios_amask);
	case 0xd: return READ_MIRROR_WORD(neogeo_sram, offset, 0x00ffff);

	case 0x2: return (*neogeo_protection_r)(offset >> 1, 0);
	case 0x4: return neogeo_paletteram_r(offset >> 1, 0);
	case 0x8: return neogeo_memcard16_r(offset >> 1, 0);

	case 0x3:
		switch (offset >> 16)
		{
		case 0x30: return neogeo_controller1and4_r(offset >> 1, 0);
		case 0x32: return neogeo_timer_r(offset >> 1, 0);
		case 0x34: return neogeo_controller2_r(offset >> 1, 0);
		case 0x38: return neogeo_controller3_r(offset >> 1, 0);
		case 0x3c: return neogeo_video_register_r(offset >> 1, 0);
		}
		break;
	}
	return 0xffff;
}


/*------------------------------------------------------
	M68000ライトメモリ (byte)
------------------------------------------------------*/

void m68000_write_memory_8(UINT32 offset, UINT8 data)
{
	int shift = (~offset & 1) << 3;
	UINT16 mem_mask = ~(0xff << shift);

	offset &= M68K_AMASK;

	switch (offset >> 20)
	{
	case 0x1: WRITE_MIRROR_BYTE(neogeo_ram, offset, data, 0x00ffff); return;

	case 0x2: (*neogeo_protection_w)(offset >> 1, data << shift, mem_mask); return;
	case 0x4: neogeo_paletteram_w(offset >> 1, data << shift, mem_mask); return;
	case 0x8: neogeo_memcard16_w(offset >> 1, data << shift, mem_mask); return;
	case 0xd: neogeo_sram16_w(offset >> 1, data << shift, mem_mask); return;

	case 0x3:
		switch (offset >> 16)
		{
		case 0x30: watchdog_reset_w(offset >> 1, data << shift, mem_mask); return;
		case 0x32: neogeo_z80_w(offset >> 1, data << shift, mem_mask); return;
		case 0x38: io_control_w(offset >> 1, data << shift, mem_mask); return;
		case 0x3a: system_control_w(offset >> 1, data << shift, mem_mask); return;
		case 0x3c: neogeo_video_register_w(offset >> 1, data << shift, mem_mask); return;
		}
		break;
	}
}


/*------------------------------------------------------
	M68000ライトメモリ (word)
------------------------------------------------------*/

void m68000_write_memory_16(UINT32 offset, UINT16 data)
{
	offset &= M68K_AMASK;

	switch (offset >> 20)
	{
	case 0x1: WRITE_MIRROR_WORD(neogeo_ram, offset, data, 0x00ffff); return;

	case 0x2: (*neogeo_protection_w)(offset >> 1, data, 0); return;
	case 0x4: neogeo_paletteram_w(offset >> 1, data, 0); return;
	case 0x8: neogeo_memcard16_w(offset >> 1, data, 0); return;
	case 0xd: neogeo_sram16_w(offset >> 1, data, 0); return;

	case 0x3:
		switch (offset >> 16)
		{
		case 0x30: watchdog_reset_w(offset >> 1, data , 0); return;
		case 0x32: neogeo_z80_w(offset >> 1, data, 0); return;
		case 0x38: io_control_w(offset >> 1, data, 0); return;
		case 0x3a: system_control_w(offset >> 1, data, 0); return;
		case 0x3c: neogeo_video_register_w(offset >> 1, data, 0); return;
		}
		break;
	}
}


/******************************************************************************
	Z80 メモリリード/ライト関数
******************************************************************************/

/*------------------------------------------------------
	Z80リードメモリ (byte)
------------------------------------------------------*/

UINT8 z80_read_memory_8(UINT32 offset)
{
	return memory_region_cpu2[offset & Z80_AMASK];
}


/*------------------------------------------------------
	Z80ライトメモリ (byte)
------------------------------------------------------*/

void z80_write_memory_8(UINT32 offset, UINT8 data)
{
	offset &= Z80_AMASK;

	if (offset >= 0xf800)
		memory_region_cpu2[offset] = data;
}


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( memory )
{
	state_save_long(&neogeo_bios, 1);
	state_save_long(&neogeo_region, 1);
	state_save_long(&neogeo_machine_mode, 1);

	state_save_byte(neogeo_ram, 0x10000);
	state_save_byte(&memory_region_cpu2[0xf800], 0x800);
	state_save_byte(neogeo_sram16, 0x10000);

	state_save_long(&neogeo_hard_dipsw, 1);
}

STATE_LOAD( memory )
{
	int bios, region, mode, harddip;

	state_load_long(&bios, 1);
	state_load_long(&region, 1);
	state_load_long(&mode, 1);

	state_load_byte(neogeo_ram, 0x10000);
	state_load_byte(&memory_region_cpu2[0xf800], 0x800);
	state_load_byte(neogeo_sram16, 0x10000);

	state_load_long(&harddip, 1);

	if (machine_init_type != INIT_ms5pcb
	&&	machine_init_type != INIT_svcpcb
	&&	machine_init_type != INIT_kf2k3pcb)
	{
		if (neogeo_bios != bios
		||	neogeo_region != region
		||	neogeo_machine_mode != mode)
		{
			neogeo_bios = bios;
			neogeo_region = region;
			neogeo_machine_mode = mode;
			state_reload_bios = 1;
		}
	}
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
}


int reload_bios(void)
{
	return load_rom_user1(1);
}

#endif /* SAVE_STATE */

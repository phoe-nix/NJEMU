/*****************************************************************************

	romcnv.c

	ROM converter for CPS2PSP

******************************************************************************/

#include "romcnv.h"


#define SPRITE_BLANK		0x00
#define SPRITE_TRANSPARENT	0x02
#define SPRITE_OPAQUE		0x01

#define MAX_GFX1ROM			32

enum
{
	REGION_GFX1 = 0,
	REGION_SKIP
};

enum
{
	TILE08 = 0,
	TILE16,
	TILE32,
	TILE_TYPE_MAX
};


/******************************************************************************
	ローカル変数
******************************************************************************/

static uint8_t  *memory_region_gfx1;
static uint32_t memory_length_gfx1;

static uint32_t gfx_total_elements[TILE_TYPE_MAX];
static uint8_t  *gfx_pen_usage[TILE_TYPE_MAX];

static struct rom_t gfx1rom[MAX_GFX1ROM];
static int num_gfx1rom;

static uint8_t block_empty[0x200];

static uint8_t null_tile[128] =
{
	0x67,0x66,0x66,0x66,0x66,0x66,0x66,0x56,
	0x56,0x55,0x55,0x55,0x55,0x55,0x55,0x45,
	0x56,0x51,0x15,0x51,0x11,0x15,0x51,0x45,
	0x56,0x11,0x15,0x51,0x11,0x15,0x51,0x45,
	0x56,0x11,0x11,0x51,0x11,0x15,0x51,0x45,
	0x56,0x11,0x15,0x51,0x11,0x15,0x51,0x45,
	0x56,0x11,0x55,0x51,0x11,0x11,0x51,0x45,
	0x56,0x55,0x55,0x55,0x55,0x55,0x55,0x45,
	0x56,0x11,0x55,0x55,0x11,0x55,0x55,0x45,
	0x56,0x11,0x55,0x55,0x11,0x55,0x55,0x45,
	0x56,0x11,0x55,0x55,0x11,0x55,0x55,0x45,
	0x56,0x11,0x55,0x55,0x11,0x55,0x55,0x45,
	0x56,0x11,0x11,0x51,0x11,0x11,0x51,0x45,
	0x56,0x55,0x55,0x55,0x55,0x55,0x55,0x45,
	0x56,0x55,0x55,0x55,0x55,0x55,0x55,0x45,
	0x45,0x44,0x44,0x44,0x44,0x44,0x44,0x34
};

static uint8_t blank_tile[128] =
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0x11,0x11,0x11,0x11,0x11,0x11,0xff,
	0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,
	0x1f,0xff,0xff,0x1f,0xff,0xff,0xf1,0xf1,
	0x1f,0xff,0xff,0xff,0xf1,0x1f,0x1f,0xf1,
	0x1f,0xff,0xff,0xff,0xff,0xff,0xf1,0xf1,
	0x1f,0xff,0xff,0x1f,0xff,0xff,0xff,0xf1,
	0x1f,0xff,0xff,0x1f,0xff,0xff,0xff,0xf1,
	0x1f,0xff,0xf1,0xff,0xf1,0x1f,0xff,0xf1,
	0x1f,0x1f,0xff,0xff,0xf1,0xff,0xf1,0xf1,
	0x1f,0x1f,0xff,0xff,0x1f,0xff,0xf1,0xf1,
	0x1f,0x1f,0x1f,0xff,0x1f,0xff,0xf1,0xf1,
	0x1f,0xff,0xff,0x11,0xf1,0xff,0xff,0xf1,
	0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,
	0xff,0x11,0x11,0x11,0x11,0x11,0x11,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};


struct cacheinfo_t
{
	const char *name;
	uint32_t  zip;
	uint32_t  object_start;
	uint32_t  object_end;
	uint32_t  scroll1_start;
	uint32_t  scroll1_end;
	uint32_t  scroll2_start;
	uint32_t  scroll2_end;
	uint32_t  scroll3_start;
	uint32_t  scroll3_end;
	uint32_t  object2_start;
	uint32_t  object2_end;
};

struct cacheinfo_t *cacheinfo;

struct cacheinfo_t CPS2_cacheinfo[] =
{
//    name           object              scroll1             scroll2             scroll3             object/scroll2
	{ "ssf2",     0, 0x000000, 0x7fffff, 0x800000, 0x88ffff, 0x900000, 0xabffff, 0xac0000, 0xbbffff, 0,         0         },
	{ "ddtod",    0, 0x000000, 0x7fffff, 0x800000, 0x8fffff, 0x900000, 0xafffff, 0xac0000, 0xbfffff, 0,         0         },
	{ "ecofghtr", 1, 0x000000, 0x7fffff, 0x800000, 0x83ffff, 0x880000, 0x99ffff, 0xa00000, 0xabffff, 0,         0         },
	{ "ssf2t",    0, 0x000000, 0x7fffff, 0x800000, 0x88ffff, 0x900000, 0xabffff, 0xac0000, 0xffffff, 0,         0         },
	{ "xmcota",   0, 0x000000, 0x7dffff, 0x800000, 0x8dffff, 0xb00000, 0xfdffff, 0x8e0000, 0xafffff, 0x1000000, 0x1ffffff },
	{ "armwar",   0, 0x000000, 0x7fffff, 0x800000, 0x85ffff, 0x860000, 0x9bffff, 0x9c0000, 0xa5ffff, 0xa60000,  0x12fffff },
	{ "avsp",     1, 0x000000, 0x7fffff, 0x800000, 0x87ffff, 0x880000, 0x9fffff, 0xa00000, 0xafffff, 0,         0         },
	{ "dstlk",    0, 0x000000, 0x7cffff, 0x800000, 0x87ffff, 0x880000, 0x9bffff, 0x9c0000, 0xabffff, 0xac0000,  0x13fffff },
	{ "ringdest", 0, 0x000000, 0x7fffff, 0x800000, 0x87ffff, 0x880000, 0x9fffff, 0xac0000, 0xcfffff, 0xd40000,  0x11fffff },
	{ "cybots",   0, 0x000000, 0x7dffff, 0x800000, 0x8bffff, 0x8c0000, 0xb3ffff, 0xb40000, 0xcbffff, 0xcc0000,  0x1ffffff },
	{ "msh",      0, 0x000000, 0x7fffff, 0x800000, 0x8cffff, 0xb00000, 0xffffff, 0x8e0000, 0xafffff, 0x1000000, 0x1ffffff },
	{ "nwarr",    0, 0x000000, 0x7cffff, 0x800000, 0x87ffff, 0x880000, 0x9bffff, 0x9c0000, 0xabffff, 0xac0000,  0x1f8ffff },
	{ "sfa",      1, 0x000000, 0x000000, 0x800000, 0x81ffff, 0x820000, 0xf8ffff, 0xfa0000, 0xfeffff, 0,         0         },
	{ "mmancp2u", 1, 0x000000, 0x000000, 0x800000, 0x85ffff, 0x860000, 0xe6ffff, 0xe80000, 0xfeffff, 0,         0         },
	{ "19xx",     1, 0x000000, 0x16ffff, 0x800000, 0x83ffff, 0x840000, 0x9bffff, 0x9c0000, 0xafffff, 0xb00000,  0xffffff  },
	{ "ddsom",    0, 0x000000, 0x7dffff, 0x800000, 0x8bffff, 0x8c0000, 0xbdffff, 0xbe0000, 0xdbffff, 0xde0000,  0x179ffff },
	{ "megaman2", 0, 0x000000, 0x000000, 0x800000, 0x85ffff, 0x860000, 0xecffff, 0xee0000, 0xffffff, 0,         0         },
	{ "qndream",  1, 0x000000, 0x000000, 0x800000, 0x81ffff, 0x840000, 0xefffff, 0x820000, 0x83ffff, 0,         0         },
	{ "sfa2",     0, 0x000000, 0x79ffff, 0x800000, 0x91ffff, 0xa40000, 0xccffff, 0x920000, 0xa3ffff, 0xd20000,  0x138ffff },
	{ "spf2t",    1, 0x000000, 0x000000, 0x800000, 0x82ffff, 0x840000, 0xb8ffff, 0xb90000, 0xbcffff, 0,         0         },
	{ "xmvsf",    0, 0x000000, 0x7effff, 0x800000, 0x8fffff, 0xaa0000, 0xffffff, 0x900000, 0xa7ffff, 0x1000000, 0x1ffffff },
	{ "batcir",   0, 0x000000, 0x7dffff, 0x800000, 0x817fff, 0x818000, 0x937fff, 0x938000, 0xa3ffff, 0xa48000,  0xd8ffff  },
	{ "csclub",   1, 0x000000, 0x000000, 0x8c0000, 0x8fffff, 0x900000, 0xffffff, 0x800000, 0x8bffff, 0,         0         },
	{ "mshvsf",   0, 0x000000, 0x7fffff, 0x800000, 0x8dffff, 0xa80000, 0xfeffff, 0x8e0000, 0xa6ffff, 0x1000000, 0x1feffff },
	{ "sgemf",    0, 0x000000, 0x7fffff, 0x800000, 0x8d1fff, 0xa22000, 0xfdffff, 0x8d2000, 0xa21fff, 0x1000000, 0x13fffff },
	{ "vhunt2",   0, 0x000000, 0x7affff, 0x800000, 0x8affff, 0xa10000, 0xfdffff, 0x8c0000, 0xa0ffff, 0x1000000, 0x1fdffff },
	{ "vsav",     0, 0x000000, 0x7fffff, 0x800000, 0x8bffff, 0x9c0000, 0xffffff, 0x8c0000, 0x9bffff, 0x1000000, 0x1feffff },
	{ "vsav2",    0, 0x000000, 0x7fffff, 0x800000, 0x8affff, 0xa10000, 0xfdffff, 0x8c0000, 0xa0ffff, 0x1000000, 0x1fdffff },
	{ "mvsc",     0, 0x000000, 0x7cffff, 0x800000, 0x91ffff, 0xb40000, 0xd0ffff, 0x920000, 0xb2ffff, 0xd20000,  0x1feffff },
	{ "sfa3",     0, 0x000000, 0x7dffff, 0x800000, 0x95ffff, 0xb60000, 0xffffff, 0x960000, 0xb5ffff, 0x1000000, 0x1fcffff },
	{ "jyangoku", 1, 0x000000, 0x7fffff, 0x800000, 0xffffff, 0x800000, 0xffffff, 0x800000, 0xffffff, 0,         0         },
	{ "hsf2",     0, 0x000000, 0x7fffff, 0x800000, 0x1ffffff,0x800000, 0x1ffffff,0x800000, 0x1ffffff,0,         0         },
	{ "gigawing", 0, 0x000000, 0x7fffff, 0x800000, 0x87ffff, 0x880000, 0xa7ffff, 0xa80000, 0xdcffff, 0xe00000,  0xffffff  },
	{ "mmatrix",  0, 0x000000, 0x7fffff, 0x800000, 0x8fffff, 0x800000, 0xd677ff, 0x800000, 0xd677ff, 0x1000000, 0x1ffffff },
	{ "mpangj",   1, 0x000000, 0x000000, 0x800000, 0x82ffff, 0x840000, 0x9dffff, 0xa00000, 0xbdffff, 0xc00000,  0xffffff  },
	{ "mpang",    1, 0x000000, 0x000000, 0x800000, 0x82ffff, 0x840000, 0x9dffff, 0xa00000, 0xbdffff, 0xc00000,  0xffffff  },
	{ "pzloop2",  1, 0x000000, 0x81ffff, 0x800000, 0x97ffff, 0xa00000, 0xc8ffff, 0xd80000, 0xebffff, 0,         0         },
	{ "choko",    1, 0x000000, 0x000000, 0x800000, 0xffffff, 0x800000, 0xffffff, 0x800000, 0xffffff, 0,         0         },
	{ "dimahoo",  0, 0x000000, 0x7fffff, 0x800000, 0x8bffff, 0xb80000, 0xffffff, 0x8e0000, 0xb6ffff, 0,         0         },
	{ "1944",     0, 0x000000, 0x7fffff, 0x800000, 0x87ffff, 0x880000, 0xcdffff, 0xd00000, 0xfeffff, 0x1000000, 0x13bffff },
	{ "progear",  0, 0x000000, 0x7fffff, 0x800000, 0xa0afff, 0xa0b000, 0xd86fff, 0xd87000, 0xffffff, 0,         0         },
	{ NULL }
};


/******************************************************************************
	CPS2用関数
******************************************************************************/

static void unshuffle(uint64_t *buf, int len)
{
	int i;
	uint64_t t;

	if (len == 2) return;

	len /= 2;

	unshuffle(buf, len);
	unshuffle(buf + len, len);

	for (i = 0; i < len / 2; i++)
	{
		t = buf[len / 2 + i];
		buf[len / 2 + i] = buf[len + i];
		buf[len + i] = t;
	}
}


static void cps2_gfx_decode(void)
{
	int i, j;
	uint8_t *gfx = memory_region_gfx1;

	for (i = 0; i < memory_length_gfx1; i += 0x200000)
		unshuffle((uint64_t *)&memory_region_gfx1[i], 0x200000 / 8);

	for (i = 0; i < memory_length_gfx1 / 4; i++)
	{
		uint32_t src = gfx[4 * i] + (gfx[4 * i + 1] << 8) + (gfx[4 * i + 2] << 16) + (gfx[4 * i + 3] << 24);
		uint32_t dw = 0, data;

		for (j = 0; j < 8; j++)
		{
			int n = 0;
			uint32_t mask = (0x80808080 >> j) & src;

			if (mask & 0x000000ff) n |= 1;
			if (mask & 0x0000ff00) n |= 2;
			if (mask & 0x00ff0000) n |= 4;
			if (mask & 0xff000000) n |= 8;

			dw |= n << (j * 4);
		}

		data = ((dw & 0x0000000f) >>  0) | ((dw & 0x000000f0) <<  4)
			 | ((dw & 0x00000f00) <<  8) | ((dw & 0x0000f000) << 12)
			 | ((dw & 0x000f0000) >> 12) | ((dw & 0x00f00000) >>  8)
			 | ((dw & 0x0f000000) >>  4) | ((dw & 0xf0000000) >>  0);

		gfx[4 * i + 0] = data >>  0;
		gfx[4 * i + 1] = data >>  8;
		gfx[4 * i + 2] = data >> 16;
		gfx[4 * i + 3] = data >> 24;
	}
}


static void clear_empty_blocks(void)
{
	int i, j, size;
	uint8_t temp[512];
	int blocks_available = 0;

	memset(block_empty, 0, 0x200);

	for (i = 0; i < memory_length_gfx1; i += 128)
	{
		if (memcmp(&memory_region_gfx1[i], null_tile, 128) == 0
		||	memcmp(&memory_region_gfx1[i], blank_tile, 128) == 0)
			memset(&memory_region_gfx1[i], 0xff, 128);
	}

	if (!strcmp(cacheinfo->name, "avsp"))
	{
		for (i = 0xb0; i <= 0xff; i++)
			memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
	}
	else if (!strcmp(cacheinfo->name, "ddtod"))
	{
		memcpy(temp, &memory_region_gfx1[0x5be800], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x657a00], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x707800], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x710b80], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x77d080], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x780000], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x7b5580], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x7d7800], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x93bd00], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0x9a5380], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0xa3eb80], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0xa70300], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0xa84f00], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
		memcpy(temp, &memory_region_gfx1[0xb75000], 512);
		for (i = 0; i < memory_length_gfx1; i += 512)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 512) == 0)
				memset(&memory_region_gfx1[i], 0xff, 512);
		}
		memcpy(temp, &memory_region_gfx1[0xb90600], 512);
		for (i = 0; i < memory_length_gfx1; i += 512)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 512) == 0)
				memset(&memory_region_gfx1[i], 0xff, 512);
		}
		memcpy(temp, &memory_region_gfx1[0xbcb200], 512);
		for (i = 0; i < memory_length_gfx1; i += 512)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 512) == 0)
				memset(&memory_region_gfx1[i], 0xff, 512);
		}
		memcpy(temp, &memory_region_gfx1[0xbd0000], 512);
		for (i = 0; i < memory_length_gfx1; i += 512)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 512) == 0)
				memset(&memory_region_gfx1[i], 0xff, 512);
		}
	}
	else if (!strcmp(cacheinfo->name, "dstlk") || !strcmp(cacheinfo->name, "nwarr"))
	{
		for (i = 0x7d; i <= 0x7f; i++)
			memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		memset(&memory_region_gfx1[0xff0000 + (16*29)*128], 0xff, 0x10000-(16*29)*128);
		memset(&memory_region_gfx1[0x13f0000 + (16*11)*128], 0xff, 0x10000-(16*11)*128);

		memcpy(temp, &memory_region_gfx1[0x10000], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}
	}
	else if (!strcmp(cacheinfo->name, "ringdest"))
	{
		for (i = 0xa0; i <= 0xab; i++)
			memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0xd0; i <= 0xd3; i++)
			memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
	}
	else if (!strcmp(cacheinfo->name, "mpang") || !strcmp(cacheinfo->name, "mpangj"))
	{
		memset(&memory_region_gfx1[0x820000 + 16*11*128], 0xff, 16*21*128);
		memset(&memory_region_gfx1[0x830000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0x840000 + (16*31+13)*128], 0xff, 0x10000-(16*31+13)*128);
		memset(&memory_region_gfx1[0x850000], 0xff, 16*16*128);
		memset(&memory_region_gfx1[0x9d0000 + (16*22+13)*128], 0xff, 0x10000-(16*22+13)*128);
		memset(&memory_region_gfx1[0x9e0000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0x9f0000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0xbd0000 + (16*4+8)*128], 0xff, 0x10000-(16*4+8)*128);
		memset(&memory_region_gfx1[0xbe0000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0xbf0000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0xd50000 + (16*12)*128], 0xff, 0x10000-(16*12)*128);
		memset(&memory_region_gfx1[0xd60000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0xd70000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0xdf0000 + (16*24)*128], 0xff, 0x10000-(16*24)*128);
		memset(&memory_region_gfx1[0xef0000 + (16*31)*128], 0xff, 0x10000-(16*31)*128);
		memset(&memory_region_gfx1[0xfb0000 + (16*14)*128], 0xff, 0x10000-(16*14)*128);
		memset(&memory_region_gfx1[0xff0000 + (16*12)*128], 0xff, 0x10000-(16*12)*128);
	}
	else if (!strcmp(cacheinfo->name, "mmatrix"))
	{
		memset(&memory_region_gfx1[0xd67600], 0xff, (16*17+4)*128);
		for (i = 0xd7; i <= 0xff; i++)
			memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
	}
	else if (!strcmp(cacheinfo->name, "pzloop2"))
	{
		memset(&memory_region_gfx1[0x170000 + 16*16*128], 0xff, 16*16*128);
		memset(&memory_region_gfx1[0x1c0000 + 16* 9*128], 0xff, 16*23*128);
		memset(&memory_region_gfx1[0x230000 + 16* 7*128], 0xff, 16*25*128);
		memset(&memory_region_gfx1[0x270000 + 16*17*128], 0xff, 16*15*128);
		memset(&memory_region_gfx1[0x290000 + 16*23*128], 0xff, 16* 9*128);
		memset(&memory_region_gfx1[0x2d0000 + 16*21*128], 0xff, 16*11*128);
		memset(&memory_region_gfx1[0x390000 + 16*30*128], 0xff, 16* 2*128);
		memset(&memory_region_gfx1[0x410000 + 16*17*128], 0xff, 16*15*128);
		memset(&memory_region_gfx1[0x530000 + 16* 6*128], 0xff, 16*26*128);
		memset(&memory_region_gfx1[0x590000 + 16* 4*128], 0xff, 16*28*128);
		memset(&memory_region_gfx1[0x670000 + 16* 9*128], 0xff, 16*23*128);
		memset(&memory_region_gfx1[0x730000 + 16*12*128], 0xff, 16*20*128);
		memset(&memory_region_gfx1[0x7a0000 + 16*10*128], 0xff, 16*22*128);
		memset(&memory_region_gfx1[0x802000 + 2*128], 0xff, 14*128);
		memset(&memory_region_gfx1[0x806800 + 4*128], 0xff, 12*128);
		memset(&memory_region_gfx1[0x810000 + 16*19*128 + 128], 0xff, 16*13*128 - 128);
		memset(&memory_region_gfx1[0xc80000 + 11*128], 0xff, 0x10000 - 11*128);
		memset(&memory_region_gfx1[0x970000 + (16*27+11)*128], 0xff, 0x10000 - (16*17+11)*128);
		memset(&memory_region_gfx1[0xeb0000 + (16*2+9)*512], 0xff, 0x10000 - (16*2+9)*512);

		for (i = 0x1d; i <= 0x1f; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x2a; i <= 0x2b; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x3a; i <= 0x3f; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x42; i <= 0x47; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x54; i <= 0x57; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x5a; i <= 0x5f; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x74; i <= 0x77; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x7b; i <= 0x7f; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x82; i <= 0x87; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0x98; i <= 0x9f; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0xc9; i <= 0xd7; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
		for (i = 0xec; i <= 0xff; i++) memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
	}
	else if (!strcmp(cacheinfo->name, "1944"))
	{
		for (i = 0x140; i <= 0x1ff; i++)
			memset(&memory_region_gfx1[i << 16], 0xff, 0x10000);
	}
	else if (!strcmp(cacheinfo->name, "choko"))
	{
		memcpy(temp, &memory_region_gfx1[0xa60000+128], 128);
		for (i = 0; i < memory_length_gfx1; i += 128)
		{
			if (memcmp(&memory_region_gfx1[i], temp, 128) == 0)
				memset(&memory_region_gfx1[i], 0xff, 128);
		}

		memset(memory_region_gfx1, 0xff, 0x800000);
		memset(&memory_region_gfx1[0x860000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0x870000], 0xff, 0x10000);

		memset(&memory_region_gfx1[0xa60000+128*16], 0xff, 0x10000-128*16);
		for (i = 0xa70000; i < 0xb00000; i += 0x10000)
			memset(&memory_region_gfx1[i], 0xff, 0x10000);

		memset(&memory_region_gfx1[0xc00000+128*16*3], 0xff, 0x10000-128*16*3);
		for (i = 0xc10000; i < 0xd00000; i += 0x10000)
			memset(&memory_region_gfx1[i], 0xff, 0x10000);

		memset(&memory_region_gfx1[0xfa0000+128*16*12], 0xff, 0x10000-128*16*12);
		memset(&memory_region_gfx1[0xfb0000], 0xff, 0x10000);

		memset(&memory_region_gfx1[0xfd0000+128*16*17+128*5], 0xff, 0x10000-(128*16*17+128*5));
		memset(&memory_region_gfx1[0xfe0000], 0xff, 0x10000);
		memset(&memory_region_gfx1[0xff0000], 0xff, 0x10000);
	}
	else if (!strcmp(cacheinfo->name, "jyangoku"))
	{
		memset(memory_region_gfx1, 0xff, 0x800000);
	}

	if (cacheinfo->object_end == 0)
	{
		memset(memory_region_gfx1, 0xff, 0x800000);
	}
	else if (cacheinfo->object_end != 0x7fffff)
	{
		for (i = cacheinfo->object_end + 1; i < 0x800000; i += 0x10000)
		{
			memset(&memory_region_gfx1[i], 0xff, 0x10000);
		}
	}

	for (i = 0; i < memory_length_gfx1 >> 16; i++)
	{
		int empty = 1;
		uint32_t offset = i << 16;

		for (j = 0; j < 0x10000; j++)
		{
			if (memory_region_gfx1[offset + j] != 0xff)
			{
				empty = 0;
				break;
			}
		}

		block_empty[i] = empty;
	}
	for (; i < 0x200; i++)
	{
		block_empty[i] = 1;
	}

	for (i = 0; i < memory_length_gfx1 >> 16; i++)
	{
		if (!block_empty[i]) blocks_available++;
	}
//	printf("cache required size = %x\n", blocks_available << 16);

	size = blocks_available << 16;
	if (size != memory_length_gfx1)
	{
#ifdef CHINESE
		printf("删除空白砖块 (完整大小: %d 字节 -> %d 字节)\n", memory_length_gfx1, size);
#else
		printf("remove empty tiles (total size: %d bytes -> %d bytes)\n", memory_length_gfx1, size);
#endif
	}
}


static int calc_pen_usage(void)
{
	int i, j, k, size;
	uint32_t *tile, data;
	uint32_t s0 = cacheinfo->object_start;
	uint32_t e0 = cacheinfo->object_end;
	uint32_t s1 = cacheinfo->scroll1_start;
	uint32_t e1 = cacheinfo->scroll1_end;
	uint32_t s2 = cacheinfo->scroll2_start;
	uint32_t e2 = cacheinfo->scroll2_end;
	uint32_t s3 = cacheinfo->scroll3_start;
	uint32_t e3 = cacheinfo->scroll3_end;
	uint32_t s4 = cacheinfo->object2_start;
	uint32_t e4 = cacheinfo->object2_end;

	gfx_total_elements[TILE08] = (memory_length_gfx1 - 0x800000) >> 6;
	gfx_total_elements[TILE16] = memory_length_gfx1 >> 7;
	gfx_total_elements[TILE32] = (memory_length_gfx1 - 0x800000) >> 9;

	if (gfx_total_elements[TILE08] >= 0x10000) gfx_total_elements[TILE08] = 0x10000;
	if (gfx_total_elements[TILE32] >= 0x10000) gfx_total_elements[TILE32] = 0x10000;

	gfx_pen_usage[TILE08] = malloc(gfx_total_elements[TILE08]);
	gfx_pen_usage[TILE16] = malloc(gfx_total_elements[TILE16]);
	gfx_pen_usage[TILE32] = malloc(gfx_total_elements[TILE32]);

	if (!gfx_pen_usage[TILE08] || !gfx_pen_usage[TILE16] || !gfx_pen_usage[TILE32])
	{
#ifdef CHINESE
		printf("错误: 无法分配内存.\n");
#else
		printf("ERROR: Could not allocate memory.\n");
#endif
		return 0;
	}

	memset(gfx_pen_usage[TILE08], 0, gfx_total_elements[TILE08]);
	memset(gfx_pen_usage[TILE16], 0, gfx_total_elements[TILE16]);
	memset(gfx_pen_usage[TILE32], 0, gfx_total_elements[TILE32]);

	for (i = 0; i < gfx_total_elements[TILE08]; i++)
	{
		int count = 0;
		uint32_t offset = (0x20000 + i) << 6;
		int s5 = 0x000000;
		int e5 = 0x000000;

		if (!strcmp(cacheinfo->name, "pzloop2"))
		{
			s5 = 0x802800;
			e5 = 0x87ffff;
		}

		if ((offset >= s1 && offset <= e1) && !(offset >= s5 && offset <= e5))
		{
			tile = (uint32_t *)&memory_region_gfx1[offset];

			for (j = 0; j < 8; j++)
			{
				tile++;
				data = *tile++;
				for (k = 0; k < 8; k++)
				{
					if ((data & 0x0f) == 0x0f)
						count++;
					data >>= 4;
				}
			}
			if (count == 0)
				gfx_pen_usage[TILE08][i] = SPRITE_OPAQUE;
			else if (count != 8*8)
				gfx_pen_usage[TILE08][i] = SPRITE_TRANSPARENT;
		}
	}

	for (i = 0; i < gfx_total_elements[TILE16]; i++)
	{
		uint32_t s5 = 0;
		uint32_t e5 = 0;
		uint32_t offset = i << 7;

		if (!strcmp(cacheinfo->name, "ssf2t"))
		{
			s5 = 0xc00000;
			e5 = 0xfaffff;
		}
		else if (!strcmp(cacheinfo->name, "gigawing"))
		{
			s5 = 0xc00000;
			e5 = 0xc7ffff;
		}
		else if (!strcmp(cacheinfo->name, "progear"))
		{
			s5 = 0xf27000;
			e5 = 0xf86fff;
		}

		if ((offset >= s0 && offset <= e0)
		||	(offset >= s2 && offset <= e2)
		||	(offset >= s4 && offset <= e4)
		||	(offset >= s5 && offset <= e5))
		{
			int count = 0;

			tile = (uint32_t *)&memory_region_gfx1[offset];

			for (j = 0; j < 2*16; j++)
			{
				data = *tile++;
				for (k = 0; k < 8; k++)
				{
					if ((data & 0x0f) == 0x0f)
						count++;
					data >>= 4;
				}
			}
			if (count == 0)
				gfx_pen_usage[TILE16][i] = SPRITE_OPAQUE;
			else if (count != 2*16*8)
				gfx_pen_usage[TILE16][i] = SPRITE_TRANSPARENT;
		}
	}

	for (i = 0; i < gfx_total_elements[TILE32]; i++)
	{
		int count  = 0;
		uint32_t offset = (0x4000 + i) << 9;

		if (!strcmp(cacheinfo->name, "ssf2t"))
		{
			if (offset >= 0xc00000 && offset <= 0xfaffff)
				continue;
		}
		else if (!strcmp(cacheinfo->name, "gigawing"))
		{
			if (offset >= 0xc00000 && offset <= 0xc7ffff)
				continue;
		}
		else if (!strcmp(cacheinfo->name, "progear"))
		{
			if (offset >= 0xf27000 && offset <= 0xf86fff)
				continue;
		}

		if (offset >= s3 && offset <= e3)
		{
			tile = (uint32_t *)&memory_region_gfx1[offset];

			for (j = 0; j < 4*32; j++)
			{
				data = *tile++;
				for (k = 0; k < 8; k++)
				{
					if ((data & 0x0f) == 0x0f)
						count++;
					data >>= 4;
				}
			}
			if (count == 0)
				gfx_pen_usage[TILE32][i] = SPRITE_OPAQUE;
			else if (count != 4*32*8)
				gfx_pen_usage[TILE32][i] = SPRITE_TRANSPARENT;
		}
	}

	return 1;
}


static int load_rom_gfx1(void)
{
	int i, res;
	char fname[32], *parent;

	if ((memory_region_gfx1 = calloc(1, memory_length_gfx1)) == NULL)
	{
		error_memory("REGION_GFX1");
		return 0;
	}

	parent = strlen(parent_name) ? parent_name : NULL;

	for (i = 0; i < num_gfx1rom; )
	{
		strcpy(fname, gfx1rom[i].name);
		if ((res = file_open(game_name, parent_name, gfx1rom[i].crc, fname)) < 0)
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

		i = rom_load(gfx1rom, memory_region_gfx1, i, num_gfx1rom);

		file_close();
	}

	return 1;
}


static int load_rom_info(const char *game_name)
{
	FILE *fp;
	char path[MAX_PATH];
	char buf[256];
	int rom_start = 0;
	int region = 0;

	num_gfx1rom = 0;

	sprintf(path, "%srominfo.cps2", launchDir);

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
					continue;
				}
				else if (str_cmp(buf, "FILENAME(") == 0)
				{
					char *name, *parent;

					strtok(buf, " ");
					name    = strtok(NULL, " ,");
					parent  = strtok(NULL, " ,");

					if (strcasecmp(name, game_name) == 0)
					{
						if (str_cmp(parent, "cps2") == 0)
							parent_name[0] = '\0';
						else
							strcpy(parent_name, parent);

						rom_start = 1;
					}
				}
				else if (rom_start && str_cmp(buf, "END") == 0)
				{
					fclose(fp);
					return 0;
				}
			}
			else if (rom_start)
			{
				if (str_cmp(&buf[1], "REGION(") == 0)
				{
					char *size, *type;

					strtok(&buf[1], " ");
					size = strtok(NULL, " ,");
					type = strtok(NULL, " ,");

					if (strcmp(type, "GFX1") == 0)
					{
						sscanf(size, "%x", &memory_length_gfx1);
						region = REGION_GFX1;
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
		fclose(fp);
		return 2;
	}
	return 3;
}


static void free_memory(void)
{
	if (memory_region_gfx1) free(memory_region_gfx1);
	if (gfx_pen_usage[TILE08]) free(gfx_pen_usage[TILE08]);
	if (gfx_pen_usage[TILE16]) free(gfx_pen_usage[TILE16]);
	if (gfx_pen_usage[TILE32]) free(gfx_pen_usage[TILE32]);
}


static int convert_rom(char *game_name)
{
	int i, res;
#ifdef CHINESE
	printf("正在检查ROM文件... (%s)\n", game_name);
#else
	printf("Checking ROM file... (%s)\n", game_name);
#endif

	memory_region_gfx1 = NULL;
	memory_length_gfx1 = 0;

	gfx_pen_usage[0] = NULL;
	gfx_pen_usage[1] = NULL;
	gfx_pen_usage[2] = NULL;

	if ((res = load_rom_info(game_name)) != 0)
	{
		switch (res)
		{
#ifdef CHINESE
		case 1: printf("错误: 此游戏暂时不支持.\n"); break;
		case 2: printf("错误: 没有找到ROM. (zip文件名不正确)\n"); break;
		case 3: printf("错误: 没有找到rominfo.cps2.\n"); break;
#else
		case 1: printf("ERROR: This game is not supported.\n"); break;
		case 2: printf("ERROR: ROM not found. (zip file name incorrect)\n"); break;
		case 3: printf("ERROR: rominfo.cps2 not found.\n"); break;
#endif
		}
		return 0;
	}

	if (!strcmp(game_name, "ssf2ta")
	||	!strcmp(game_name, "ssf2tu")
	||	!strcmp(game_name, "ssf2tur1")
	||	!strcmp(game_name, "ssf2xj"))
	{
		strcpy(cache_name, "ssf2t");
	}
	else if (!strcmp(game_name, "ssf2t"))
	{
		cache_name[0] = '\0';
	}
	else if (!strcmp(game_name, "mpangj"))
	{
		cache_name[0] = '\0';
	}
	else
	{
		strcpy(cache_name, parent_name);
	}

	if (strlen(parent_name))
#ifdef CHINESE
		printf("子ROM版本 (主ROM名: %s)\n", parent_name);
#else
		printf("Clone set (parent: %s)\n", parent_name);
#endif

	i = 0;
	cacheinfo = NULL;
	while (CPS2_cacheinfo[i].name)
	{
		if (!strcmp(game_name, CPS2_cacheinfo[i].name))
		{
			cacheinfo = &CPS2_cacheinfo[i];
			break;
		}
		if (!strcmp(cache_name, CPS2_cacheinfo[i].name))
		{
			cacheinfo = &CPS2_cacheinfo[i];
			break;
		}
		i++;
	}

	if (cacheinfo)
	{
		if (load_rom_gfx1())
		{
			cps2_gfx_decode();
			clear_empty_blocks();
			if (calc_pen_usage()) return 1;
		}
	}
	else
	{
#ifdef CHINESE
		printf("错误: 未知的romset.\n");
#else
		printf("ERROR: Unknown romset.\n");
#endif
	}

	return 0;
}


static int create_raw_cache(char *game_name)
{
	FILE *fp;
	int i, offset;
	char version[8];
	uint32_t header_size, aligned_size, block[0x200];
	char fname[MAX_PATH];

	sprintf(version, "CPS2V%d%d\0", VERSION_MAJOR, VERSION_MINOR);

	chdir("cache");

	header_size = 8;
	header_size += gfx_total_elements[TILE08];
	header_size += gfx_total_elements[TILE16];
	header_size += gfx_total_elements[TILE32];
	header_size += 0x200 * sizeof(uint32_t);

	aligned_size = (header_size + 0xffff) & ~0xffff;

	offset = aligned_size;
	for (i = 0; i < 0x200; i++)
	{
		if (block_empty[i])
		{
			block[i] = 0xffffffff;
		}
		else
		{
			if (lsb_first)
			{
				block[i] = offset;
			}
			else
			{
				block[i] = ((offset & 0x000000ff) << 24)
						 | ((offset & 0x0000ff00) <<  8)
						 | ((offset & 0x00ff0000) >>  8)
						 | ((offset & 0xff000000) >> 24);
			}
			offset += 0x10000;
		}
	}

	sprintf(fname, "%s.cache", game_name);
	if ((fp = fopen(fname, "wb")) == NULL)
	{
		chdir("..");
#ifdef CHINESE
		printf("错误: 无法创建文件.\n");
#else
		printf("ERROR: Could not create file.\n");
#endif
		return 0;
	}

#ifdef CHINESE
	printf("缓存名: cache%c%s.cache\n", delimiter, game_name);
	printf("正在创建缓存文件...\n");
#else
	printf("cache name: cache%c%s.cache\n", delimiter, game_name);
	printf("Create cache file...\n");
#endif

	fwrite(version, 1, sizeof(version), fp);
	fwrite(gfx_pen_usage[TILE08], 1, gfx_total_elements[TILE08], fp);
	fwrite(gfx_pen_usage[TILE16], 1, gfx_total_elements[TILE16], fp);
	fwrite(gfx_pen_usage[TILE32], 1, gfx_total_elements[TILE32], fp);
	fwrite(block, 1, 0x200 * sizeof(uint32_t), fp);

	for (i = header_size; i < aligned_size; i++)
		fputc(0, fp);

	for (i = 0; i < 0x200; i++)
	{
		if (block_empty[i]) continue;

		fwrite(&memory_region_gfx1[i << 16], 1, 0x10000, fp);
	}

	fclose(fp);

	chdir("..");

	return 1;
}


static void print_progress(int count, int total)
{
	int i, progress = (count * 100) / total;

	printf("%3d%% [", progress);
	for (i = 0; i < progress/2; i++) printf("*");
	for (; i < 50; i++) printf(".");
	printf("]\r");
}


static int create_zip_cache(char *game_name)
{
	int fd;
	uint32_t block, res = 0, total = 0, count = 0;
	char version[8], fname[MAX_PATH], zipname[MAX_PATH];

	sprintf(version, "CPS2V%d%d\0", VERSION_MAJOR, VERSION_MINOR);

	chdir("cache");

	sprintf(zipname, "%s%ccache%c%s_cache.zip", launchDir, delimiter, delimiter, game_name);
	remove(zipname);

#ifdef CHINESE
	printf("缓存名: cache%c%s_cache.zip\n", delimiter, game_name);
	printf("正在创建缓存文件...\n");
#else
	printf("cache name: cache%c%s_cache.zip\n", delimiter, game_name);
	printf("Create cache file...\n");
#endif
	if (zip_open(zipname, "wb") < 0)
	{
#ifdef CHINESE
		printf("错误: 无法创建zip文件 \"cache%c%s_cache.zip\".\n", delimiter, game_name);
#else
		printf("ERROR: Could not create zip file \"cache%c%s_cache.zip\".\n", delimiter, game_name);
#endif
		goto error;
	}

#ifdef CHINESE
	printf("压缩为zip文件... \"cache%c%s_cache.zip\"\n", delimiter, game_name);
#else
	printf("Compress to zip file... \"cache%c%s_cache.zip\"\n", delimiter, game_name);
#endif

	for (block = 0; block < 0x200; block++)
		if (!block_empty[block]) total++;
	total++;

	print_progress(0, total);

	for (block = 0; block < 0x200; block++)
	{
		if (block_empty[block]) continue;

		sprintf(fname, "%03x", block);
		if ((fd = zopen(fname)) < 0) goto error;
		zwrite(fd, &memory_region_gfx1[block << 16], 0x10000);
		zclose(fd);
		print_progress(++count, total);
	}

	if ((fd = zopen("cache_info")) < 0) goto error;
	zwrite(fd, version, 8);
	zwrite(fd, gfx_pen_usage[TILE08], gfx_total_elements[TILE08]);
	zwrite(fd, gfx_pen_usage[TILE16], gfx_total_elements[TILE16]);
	zwrite(fd, gfx_pen_usage[TILE32], gfx_total_elements[TILE32]);
	zwrite(fd, block_empty, 0x200);
	zclose(fd);

	print_progress(++count, total);
	printf("\n");

	res = 1;

error:
	zip_close();

#ifdef CHINESE
	if (!res) printf("错误: 无法创建文件.\n");
#else
	if (!res) printf("ERROR: Could not create file.\n");
#endif

	chdir("..");

	return res;
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
	printf(" CPS2PSP ROM 转换器  " VERSION_STR "\n");
	printf("----------------------------------------------\n\n");
#else
	printf("----------------------------------------------\n");
	printf(" ROM converter for CPS2PSP " VERSION_STR "\n");
	printf("----------------------------------------------\n\n");
#endif

	if (argc > 1)
	{
		for (i = 1; i < argc; i++)
		{
			if (!strcasecmp(argv[i], "-all"))
			{
				all = 1;
			}
			else if (!strcasecmp(argv[i], "-zip"))
			{
				zip = 1;
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
		printf("usage: romcnv_cps2 fullpath%cgamename.zip [-zip]\n", DELIMITER);
		printf("  or   romcnv_cps2 fullpath -all [-zip]\n\n", DELIMITER);
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

		for (i = 0; CPS2_cacheinfo[i].name; i++)
		{
			res = 1;

			strcpy(game_name, CPS2_cacheinfo[i].name);

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
				printf("错误: 转换失败. - 跳过\n\n");
#else
				printf("ERROR: Convert failed. - Skip\n\n");
#endif
			}
			else
			{
				if (zip || cacheinfo->zip)
					res = create_zip_cache(game_name);
				else
					res = create_raw_cache(game_name);

#ifdef CHINESE
				if (res) printf("完成.\n\n");
#else
				if (res) printf("Done.\n\n");
#endif
			}
			free_memory();
		}
#ifdef CHINESE
		printf("完成.\n");
		printf("请将cache内的文件夹复制到\"/PSP/GAMES/cps2psp/cache\".\n");
#else
		printf("complete.\n");
		printf("Please copy these files to directory \"/PSP/GAMES/cps2psp/cache\".\n");
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
		printf("file name: %s\n", game_name);
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

		chdir(launchDir);
		if (!convert_rom(game_name))
		{
			res = 0;
		}
		else
		{
			if (zip || cacheinfo->zip)
				res = create_zip_cache(game_name);
			else
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
			printf("请将\"cache%c%s_cache\"文件夹复制到\"/PSP/GAMES/cps2psp/cache\".\n", delimiter, game_name);
#else
			printf("complete.\n");
			printf("Please copy \"cache%c%s.cache\" to directory \"/PSP/GAMES/cps2psp/cache\".\n", delimiter, game_name);
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

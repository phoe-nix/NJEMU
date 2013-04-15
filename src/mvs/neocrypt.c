/***************************************************************************

    NeoGeo 'P' Rom Encryption
      Used on various games

      kof98
        - unique early encryption
      kof99, garou, garouo, mslug3, kof2000
        - complex SMA chip which appears to contain part of the game rom
          internally and decrypts the 68k code on the board.  Also has a
          random number generator and  custom bankswitching
          (see machine/neoprot.c)
      kof2002, matrim, samsho5, samsh5p
        - some basic block / bank swapping
      svcchaos, kof2003, mslug5
        - different scrambling with additional xor

***************************************************************************/

#include "mvs.h"


/***************************************************************************

NeoGeo 'P' ROM encryption

***************************************************************************/

/* Kof98 uses an early encryption, quite different from the others */
int kof98_decrypt_68k(void)
{
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x200000);
#endif
	UINT32 i, j, k;
	const UINT32 sec[] = { 0x000000, 0x100000, 0x000004, 0x100004, 0x10000a, 0x00000a, 0x10000e, 0x00000e };
	const UINT32 pos[] = { 0x000, 0x004, 0x00a, 0x00e };

	if (dst)
	{
		memcpy(dst, src, 0x200000);

		for (i = 0x800; i < 0x100000; i += 0x200)
		{
			for (j = 0; j < 0x100; j += 0x10)
			{
				for (k = 0; k < 16; k += 2)
				{
					memcpy(&src[i + j + k        ], &dst[i + j + sec[k / 2] + 0x100 ], 2);
					memcpy(&src[i + j + k + 0x100], &dst[i + j + sec[k / 2]         ], 2);
				}
				if (i >= 0x080000 && i < 0x0c0000)
				{
					for (k = 0; k < 4; k++)
					{
						memcpy(&src[i + j + pos[k]      ], &dst[i + j + pos[k]        ], 2);
						memcpy(&src[i + j + pos[k]+0x100], &dst[i + j + pos[k] + 0x100], 2);
					}
				}
				else if (i >= 0x0c0000)
				{
					for (k = 0; k < 4; k++)
					{
						memcpy(&src[i + j + pos[k]        ], &dst[i + j + pos[k] + 0x100], 2);
						memcpy(&src[i + j + pos[k] + 0x100], &dst[i + j + pos[k]        ], 2);
					}
				}
			}
			memcpy(&src[i + 0x000000], &dst[i + 0x000000], 2);
			memcpy(&src[i + 0x000002], &dst[i + 0x100000], 2);
			memcpy(&src[i + 0x000100], &dst[i + 0x000100], 2);
			memcpy(&src[i + 0x000102], &dst[i + 0x100100], 2);
		}
		memcpy(&src[0x100000], &src[0x200000], 0x400000);

#ifndef PSP_SLIM
		free(dst);
#endif
		return 1;
	}
	return 0;
}

/* kof99, garou, garouo, mslug3 and kof2000 have and SMA chip which contains program code and decrypts the 68k roms */
void kof99_decrypt_68k(void)
{
	UINT16 *rom;
	UINT32 i, j;

	rom = (UINT16 *)(memory_region_cpu1 + 0x100000);

	/* swap data lines on the whole ROMs */
	for (i = 0; i < 0x800000/2; i++)
	{
		rom[i] = BITSWAP16(rom[i],13,7,3,0,9,4,5,6,1,12,8,14,10,11,2,15);
	}

	/* swap address lines for the banked part */
	for (i = 0; i < 0x600000/2; i += 0x800/2)
	{
		UINT16 buffer[0x800/2];

		memcpy(buffer, &rom[i], 0x800);
		for (j = 0; j < 0x800/2; j++)
		{
			rom[i + j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,13,12,11,10,6,2,4,9,8,3,1,7,0,5)];
		}
	}

	/* swap address lines & relocate fixed part */
	rom = (UINT16 *)memory_region_cpu1;
	for (i = 0; i < 0x0c0000/2; i++)
	{
		rom[i] = rom[0x700000/2 + BITSWAP24(i,23,22,21,20,19,18,11,6,14,17,16,5,8,10,12,0,4,3,2,7,9,15,13,1)];
	}
}

void garou_decrypt_68k(void)
{
	UINT16 *rom;
	UINT32 i, j;

	/* thanks to Razoola and Mr K for the info */
	rom = (UINT16 *)(memory_region_cpu1 + 0x100000);
	/* swap data lines on the whole ROMs */
	for (i = 0; i < 0x800000/2; i++)
	{
		rom[i] = BITSWAP16(rom[i],13,12,14,10,8,2,3,1,5,9,11,4,15,0,6,7);
	}

	/* swap address lines & relocate fixed part */
	rom = (UINT16 *)memory_region_cpu1;
	for (i = 0; i < 0x0c0000/2; i++)
	{
		rom[i] = rom[0x710000/2 + BITSWAP24(i,23,22,21,20,19,18,4,5,16,14,7,9,6,13,17,15,3,1,2,12,11,8,10,0)];
	}

	/* swap address lines for the banked part */
	rom = (UINT16 *)(memory_region_cpu1 + 0x100000);
	for (i = 0; i < 0x800000/2; i += 0x8000/2)
	{
		UINT16 buffer[0x8000/2];

		memcpy(buffer, &rom[i], 0x8000);
		for (j = 0; j < 0x8000/2; j++)
		{
			rom[i + j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,9,4,8,3,13,6,2,7,0,12,1,11,10,5)];
		}
	}
}

void garouo_decrypt_68k(void)
{
	UINT16 *rom;
	UINT32 i, j;

	/* thanks to Razoola and Mr K for the info */
	rom = (UINT16 *)(memory_region_cpu1 + 0x100000);
	/* swap data lines on the whole ROMs */
	for (i = 0; i < 0x800000/2; i++)
	{
		rom[i] = BITSWAP16(rom[i],14,5,1,11,7,4,10,15,3,12,8,13,0,2,9,6);
	}

	/* swap address lines & relocate fixed part */
	rom = (UINT16 *)memory_region_cpu1;
	for (i = 0; i < 0x0c0000/2; i++)
	{
		rom[i] = rom[0x7f8000/2 + BITSWAP24(i,23,22,21,20,19,18,5,16,11,2,6,7,17,3,12,8,14,4,0,9,1,10,15,13)];
	}

	/* swap address lines for the banked part */
	rom = (UINT16 *)(memory_region_cpu1 + 0x100000);
	for (i = 0; i < 0x800000/2; i += 0x8000/2)
	{
		UINT16 buffer[0x8000/2];

		memcpy(buffer, &rom[i], 0x8000);
		for (j = 0; j < 0x8000/2; j++)
		{
			rom[i + j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,12,8,1,7,11,3,13,10,6,9,5,4,0,2)];
		}
	}
}

void mslug3_decrypt_68k(void)
{
	UINT16 *rom;
	UINT32 i, j;

	/* thanks to Razoola and Mr K for the info */
	rom = (UINT16 *)(memory_region_cpu1 + 0x100000);
	/* swap data lines on the whole ROMs */
	for (i = 0; i < 0x800000/2; i++)
	{
		rom[i] = BITSWAP16(rom[i],4,11,14,3,1,13,0,7,2,8,12,15,10,9,5,6);
	}

	/* swap address lines & relocate fixed part */
	rom = (UINT16 *)memory_region_cpu1;
	for (i = 0; i < 0x0c0000/2; i++)
	{
		rom[i] = rom[0x5d0000/2 + BITSWAP24(i,23,22,21,20,19,18,15,2,1,13,3,0,9,6,16,4,11,5,7,12,17,14,10,8)];
	}

	/* swap address lines for the banked part */
	rom = (UINT16 *)(memory_region_cpu1 + 0x100000);
	for (i = 0; i < 0x800000/2; i += 0x10000/2)
	{
		UINT16 buffer[0x10000/2];

		memcpy(buffer, &rom[i], 0x10000);
		for (j = 0; j < 0x10000/2; j++)
		{
			rom[i + j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,2,11,0,14,6,4,13,8,9,3,10,7,5,12,1)];
		}
	}
}

void kof2000_decrypt_68k(void)
{
	UINT16 *rom;
	UINT32 i, j;

	/* thanks to Razoola and Mr K for the info */
	rom = (UINT16 *)(memory_region_cpu1 + 0x100000);
	/* swap data lines on the whole ROMs */
	for (i = 0; i < 0x800000/2; i++)
	{
		rom[i] = BITSWAP16(rom[i],12,8,11,3,15,14,7,0,10,13,6,5,9,2,1,4);
	}

	/* swap address lines for the banked part */
	for (i = 0; i < 0x63a000/2; i += 0x800/2)
	{
		UINT16 buffer[0x800/2];

		memcpy(buffer, &rom[i], 0x800);
		for (j = 0; j < 0x800/2; j++)
		{
			rom[i + j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,13,12,11,10,4,1,3,8,6,2,7,0,9,5)];
		}
	}

	/* swap address lines & relocate fixed part */
	rom = (UINT16 *)memory_region_cpu1;
	for (i = 0; i < 0x0c0000/2; i++)
	{
		rom[i] = rom[0x73a000/2 + BITSWAP24(i,23,22,21,20,19,18,8,4,15,13,3,14,16,2,6,17,7,12,10,0,5,11,1,9)];
	}
}


/***************************************************************************

	kof2002, matrim, samsho5, samsh5p have some simple block swapping

***************************************************************************/

#define TEMP	0xff
#define END		0xfe

static int kof2002_descrample_68k(const UINT8 *sec)
{
	UINT8 *rom = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *buf = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *buf = (UINT8 *)malloc(0x80000);
#endif

	if (buf)
	{
		UINT8 i, *src, *dst;

		for (i = 0; sec[i + 1] != END; i++)
		{
			dst = (sec[i + 0] == TEMP) ? buf : &rom[sec[i + 0] << 16];
			src = (sec[i + 1] == TEMP) ? buf : &rom[sec[i + 1] << 16];
			memcpy(dst, src, 0x80000);
		}

#ifndef PSP_SLIM
		free(buf);
#endif
		return 1;
	}
	return 0;
}

int kof2002_decrypt_68k(void)
{
	const UINT8 sec[] =
	{
		TEMP,
		0x10, 0x20, 0x40, 0x30, TEMP,
		0x18, 0x38, 0x48, TEMP,
		END
	};

	return kof2002_descrample_68k(sec);
}

int samsho5_decrypt_68k(void)
{
	const UINT8 sec[] =
	{
		TEMP,
		0x10, 0x70, 0x40, 0x30, 0x20, 0x50, 0x60, TEMP,
		0x18, 0x68, 0x58, 0x28, TEMP,
		0x38, 0x48, 0x78, TEMP,
		END
	};

	return kof2002_descrample_68k(sec);
}

int samsh5p_decrypt_68k(void)
{
	const UINT8 sec[] =
	{
		TEMP,
		0x10, 0x50, 0x40, TEMP,
		0x20, 0x60, TEMP,
		0x30, 0x70, TEMP,
		0x18, 0x48, 0x68, 0x38, 0x28, 0x58, 0x78, TEMP,
		END
	};

	return kof2002_descrample_68k(sec);
}

#undef TEMP
#undef END


/***************************************************************************

	mslug5, svcchaos, kof2003 have updated P rom scramble

***************************************************************************/

int mslug5_decrypt_68k(void)
{
	const UINT8 xor1[0x20] =
	{
		0xc2, 0x4b, 0x74, 0xfd, 0x0b, 0x34, 0xeb, 0xd7,
		0x10, 0x6d, 0xf9, 0xce, 0x5d, 0xd5, 0x61, 0x29,
		0xf5, 0xbe, 0x0d, 0x82, 0x72, 0x45, 0x0f, 0x24,
		0xb3, 0x34, 0x1b, 0x99, 0xea, 0x09, 0xf3, 0x03
	};
	const UINT8 xor2[0x20] =
	{
		0x36, 0x09, 0xb0, 0x64, 0x95, 0x0f, 0x90, 0x42,
		0x6e, 0x0f, 0x30, 0xf6, 0xe5, 0x08, 0x30, 0x64,
		0x08, 0x04, 0x00, 0x2f, 0x72, 0x09, 0xa0, 0x13,
		0xc9, 0x0b, 0xa0, 0x3e, 0xc2, 0x00, 0x40, 0x2b
	};
	UINT32 i, offset;
	UINT8 *rom = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *buf = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *buf = (UINT8 *)malloc(0x800000);
#endif

	if (buf)
	{
		for (i = 0; i < 0x100000; i++)
		{
			rom[i] ^= xor1[i & 0x1f];
		}
		for (i = 0x100000; i < 0x800000; i++)
		{
			rom[i] ^= xor2[i & 0x1f];
		}

		for (i = 0x100000; i < 0x800000; i += 4)
		{
			UINT16 rom16;

			rom16 = rom[i + 1] | (rom[i + 2] << 8);
			rom16 = BITSWAP16(rom16,15,14,13,12,10,11,8,9,6,7,4,5,3,2,1,0);
			rom[i + 1] = rom16 & 0xff;
			rom[i + 2] = rom16 >> 8;
		}
		memcpy(buf, rom, 0x800000);

		for (i = 0; i < 0x10; i++)
		{
			offset = (i & 0xf0) + BITSWAP8((i & 0x0f),7,6,5,4,1,0,3,2);
			memcpy(&rom[i << 16], &buf[offset << 16], 0x10000);
		}
		for (i = 0x100000; i < 0x800000; i += 0x100)
		{
			offset = (i & 0xf000ff) + ((i & 0x000f00) ^ 0x00700) + (BITSWAP8(((i & 0x0ff000) >> 12),5,4,7,6,1,0,3,2 ) << 12);
			memcpy(&rom[i], &buf[offset], 0x100);
		}

		memcpy(buf, rom, 0x800000);
		memcpy(&rom[0x100000], &buf[0x700000], 0x100000);
		memcpy(&rom[0x200000], &buf[0x100000], 0x600000);

#ifndef PSP_SLIM
		free(buf);
#endif
		return 1;
	}
	return 0;
}

int svcchaos_px_decrypt(void)
{
	const UINT8 xor1[0x20] =
	{
		0x3b, 0x6a, 0xf7, 0xb7, 0xe8, 0xa9, 0x20, 0x99,
		0x9f, 0x39, 0x34, 0x0c, 0xc3, 0x9a, 0xa5, 0xc8,
		0xb8, 0x18, 0xce, 0x56, 0x94, 0x44, 0xe3, 0x7a,
		0xf7, 0xdd, 0x42, 0xf0, 0x18, 0x60, 0x92, 0x9f
	};
	const UINT8 xor2[0x20] =
	{
		0x69, 0x0b, 0x60, 0xd6, 0x4f, 0x01, 0x40, 0x1a,
		0x9f, 0x0b, 0xf0, 0x75, 0x58, 0x0e, 0x60, 0xb4,
		0x14, 0x04, 0x20, 0xe4, 0xb9, 0x0d, 0x10, 0x89,
		0xeb, 0x07, 0x30, 0x90, 0x50, 0x0e, 0x20, 0x26
	};
	UINT32 i, offset;
	UINT8 *rom = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *buf = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *buf = (UINT8 *)malloc(0x800000);
#endif

	if (buf)
	{
		for (i = 0; i < 0x100000; i++)
		{
			rom[i] ^= xor1[i & 0x1f];
		}
		for (i = 0x100000; i < 0x800000; i++)
		{
			rom[i] ^= xor2[i & 0x1f];
		}

		for (i = 0x100000; i < 0x800000; i += 4)
		{
			UINT16 rom16;

			rom16 = rom[i + 1] | (rom[i + 2] << 8);
			rom16 = BITSWAP16(rom16,15,14,13,12,10,11,8,9,6,7,4,5,3,2,1,0);
			rom[i + 1] = rom16 & 0xff;
			rom[i + 2] = rom16 >> 8;
		}
		memcpy(buf, rom, 0x800000);

		for (i = 0; i < 0x10; i++)
		{
			offset = (i & 0xf0) + BITSWAP8((i & 0x0f),7,6,5,4,2,3,0,1);
			memcpy(&rom[i << 16], &buf[offset << 16], 0x10000);
		}
		for (i = 0x100000; i < 0x800000; i += 0x100)
		{
			offset = (i & 0xf000ff) + ((i & 0x000f00) ^ 0x00a00) + (BITSWAP8(((i & 0x0ff000) >> 12),4,5,6,7,1,0,3,2) << 12);
			memcpy(&rom[i], &buf[offset], 0x100);
		}

		memcpy(buf, rom, 0x800000);
		memcpy(&rom[0x100000], &buf[0x700000], 0x100000);
		memcpy(&rom[0x200000], &buf[0x100000], 0x600000);
#ifndef PSP_SLIM
		free(buf);
#endif
		return 1;
	}
	return 0;
}

int kf2k3pcb_decrypt_68k(void)
{
	const UINT8 xor2[0x20] =
	{
		0xb4, 0x0f, 0x40, 0x6c, 0x38, 0x07, 0xd0, 0x3f,
		0x53, 0x08, 0x80, 0xaa, 0xbe, 0x07, 0xc0, 0xfa,
		0xd0, 0x08, 0x10, 0xd2, 0xf1, 0x03, 0x70, 0x7e,
		0x87, 0x0b, 0x40, 0xf6, 0x2a, 0x0a, 0xe0, 0xf9
	};
	UINT32 i, offset;
	UINT8 *rom = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *buf = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *buf = (UINT8 *)malloc(0x800000);
#endif

	if (buf)
	{
		for (i = 0; i < 0x100000; i++)
		{
			rom[0x800000 + i] ^= rom[0x100002 | i];
		}
		for (i = 0x100000; i < 0x800000; i++)
		{
			rom[i] ^= xor2[i & 0x1f];
		}

		for (i = 0x100000; i < 0x800000; i += 4)
		{
			UINT16 rom16;

			rom16 = rom[i + 1] | (rom[i + 2] << 8);
			rom16 = BITSWAP16(rom16,15,14,13,12,4,5,6,7,8,9,10,11,3,2,1,0);
			rom[i + 1] = rom16 & 0xff;
			rom[i + 2] = rom16 >> 8;
		}

		for (i = 0; i < 0x10; i++)
		{
			offset = (i & 0xf0) + BITSWAP8((i & 0x0f),7,6,5,4,1,0,3,2);
			memcpy(&buf[i << 16], &rom[offset << 16], 0x10000);
		}
		memcpy(rom, buf, 0x100000);

		for (i = 0x100000; i < 0x900000; i += 0x100)
		{
			offset = (i & 0xf000ff) + ((i & 0x000f00) ^ 0x00300) + (BITSWAP8(((i & 0x0ff000) >> 12),4,5,6,7,1,0,3,2) << 12);
			memcpy(&buf[i - 0x100000], &rom[offset], 0x100);
		}

		memcpy(&rom[0x100000], &buf[0x700000], 0x100000);
		memcpy(&rom[0x200000], &buf[0x000000], 0x700000);
#ifndef PSP_SLIM
		free(buf);
#endif
		return 1;
	}
	return 0;
}

int kof2003_decrypt_68k(void)
{
	const UINT8 xor1[0x20] =
	{
		0x3B, 0x6A, 0xF7, 0xB7, 0xE8, 0xA9, 0x20, 0x99,
		0x9F, 0x39, 0x34, 0x0C, 0xC3, 0x9A, 0xA5, 0xC8,
		0xB8, 0x18, 0xCE, 0x56, 0x94, 0x44, 0xE3, 0x7A,
		0xF7, 0xDD, 0x42, 0xF0, 0x18, 0x60, 0x92, 0x9F
	};
	const UINT8 xor2[0x20] =
	{
		0x2F, 0x02, 0x60, 0xBB, 0x77, 0x01, 0x30, 0x08,
		0xD8, 0x01, 0xA0, 0xDF, 0x37, 0x0A, 0xF0, 0x65,
		0x28, 0x03, 0xD0, 0x23, 0xD3, 0x03, 0x70, 0x42,
		0xBB, 0x06, 0xF0, 0x28, 0xBA, 0x0F, 0xF0, 0x7A
	};
	UINT32 i, offset;
	UINT8 *rom = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *buf = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *buf = (UINT8 *)malloc(0x800000);
#endif

	if (buf)
	{
		for (i = 0; i < 0x100000; i++)
		{
			rom[0x800000 + i] ^= rom[0x100002 | i];
		}
		for (i = 0; i < 0x100000; i++)
		{
			rom[i] ^= xor1[i & 0x1f];
		}
		for (i = 0x100000; i < 0x800000; i++)
		{
			rom[i] ^= xor2[i & 0x1f];
		}

		for (i = 0x100000; i < 0x800000; i += 4)
		{
			UINT16 rom16;

			rom16 = rom[i + 1] | (rom[i + 2] << 8);
			rom16 = BITSWAP16(rom16,15,14,13,12,5,4,7,6,9,8,11,10,3,2,1,0);
			rom[i + 1] = rom16 & 0xff;
			rom[i + 2] = rom16 >> 8;
		}

		for (i = 0; i < 0x10; i++)
		{
			offset = (i & 0xf0) + BITSWAP8((i & 0x0f),7,6,5,4,0,1,2,3);
			memcpy(&buf[i << 16], &rom[offset << 16], 0x10000);
		}
		memcpy(rom, buf, 0x100000);

		for (i = 0x100000; i < 0x900000; i += 0x100)
		{
			offset = (i & 0xf000ff) + ((i & 0x000f00) ^ 0x00800) + (BITSWAP8(((i & 0x0ff000) >> 12),4,5,6,7,1,0,3,2) << 12);
			memcpy(&buf[i - 0x100000], &rom[offset], 0x100);
		}
		memcpy(&rom[0x100000], &buf[0x700000], 0x100000);
		memcpy(&rom[0x200000], &buf[0x000000], 0x700000);
#ifndef PSP_SLIM
		free(buf);
#endif
		return 1;
	}
	return 0;
}


int kof2003biosdecode(void)
{
	const UINT8 address[0x80] =
	{
		0xb9,0xb8,0x36,0x37,0x3d,0x3c,0xb2,0xb3,
		0xb9,0xb8,0x36,0x37,0x3d,0x3c,0xb2,0xb3,
		0x65,0xea,0x6f,0xe0,0xe1,0x6e,0xeb,0x64,
		0x65,0xea,0x6f,0xe0,0xe1,0x6e,0xeb,0x64,
		0x45,0xca,0x47,0xc8,0xc9,0x46,0xcb,0x44,
		0x45,0xca,0x47,0xc8,0xc9,0x46,0xcb,0x44,
		0x9a,0x15,0x98,0x17,0x1e,0x91,0x1c,0x93,
		0x9a,0x15,0x98,0x17,0x1e,0x91,0x1c,0x93,
		0x7e,0xf1,0x7c,0xf3,0xf0,0x7f,0xf2,0x7d,
		0x7e,0xf1,0x7c,0xf3,0xf0,0x7f,0xf2,0x7d,
		0x27,0xa8,0x25,0xaa,0xa3,0x2c,0xa1,0x2e,
		0x27,0xa8,0x25,0xaa,0xa3,0x2c,0xa1,0x2e,
		0x04,0x8b,0x06,0x89,0x80,0x0f,0x82,0x0d,
		0x04,0x8b,0x06,0x89,0x80,0x0f,0x82,0x0d,
		0xd3,0xd2,0x5c,0x5d,0x57,0x56,0xd8,0xd9,
		0xd3,0xd2,0x5c,0x5d,0x57,0x56,0xd8,0xd9,
	};
	UINT16 *src = (UINT16 *)memory_region_user1;
#ifdef PSP_SLIM
	UINT16 *buf = (UINT16 *)psp2k_mem_offset;
#else
	UINT16 *buf = (UINT16 *)malloc(0x80000);
#endif
	UINT32 a, addr;

	if (buf)
	{
		for (a = 0; a < 0x80000/2; a++)
		{
			//data xor
			if (src[a] & 0x0004) src[a] ^= 0x0001;
			if (src[a] & 0x0010) src[a] ^= 0x0002;
			if (src[a] & 0x0020) src[a] ^= 0x0008;

			//address xor
			addr  = a & ~0xff;
			addr |= address[a & 0x7f];
			if ( a & 0x00008) addr ^= 0x0008;
			if ( a & 0x00080) addr ^= 0x0080;
			if ( a & 0x00200) addr ^= 0x0100;
			if (~a & 0x02000) addr ^= 0x0400;
			if (~a & 0x10000) addr ^= 0x1000;
			if ( a & 0x02000) addr ^= 0x8000;
			buf[addr] = src[a];
		}
		memcpy(src, buf, 0x80000);

#ifndef PSP_SLIM
		free(buf);
#endif

		return 1;
	}
	return 0;
}


#if !RELEASE

/***************************************************************************

    NeoGeo *bootleg* Encryption (& Some Protection)

    Many of the NeoGeo bootlegs use their own form of encryption and
    protection, presumably to make them harder for other bootleggser to
    copy.  This encryption often involves non-trivial scrambling of the
    program roms and the games are protected using an Altera chip which
    provides some kind of rom overlay, patching parts of the code.
    The graphics roms are usually scrambled in a different way to the
    official SNK cartridges too.

***************************************************************************/

int kog_px_decrypt(void)
{
	const UINT32 sec[] = { 0x3, 0x8, 0x7, 0xC, 0x1, 0xA, 0x6, 0xD };
	UINT32 i;
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x600000);
#endif
	UINT16 *rom = (UINT16 *)memory_region_cpu1;

	if (dst)
	{
		for (i = 0; i < 8; i++)
		{
			memcpy(dst + i * 0x20000, src + sec[i] * 0x20000, 0x20000);
		}

		memcpy(dst + 0x0007A6, src + 0x0407A6, 0x000006);
		memcpy(dst + 0x0007C6, src + 0x0407C6, 0x000006);
		memcpy(dst + 0x0007E6, src + 0x0407E6, 0x000006);
		memcpy(dst + 0x090000, src + 0x040000, 0x004000);
		memcpy(dst + 0x100000, src + 0x200000, 0x400000);
		memcpy(src, dst, 0x600000);

#ifndef PSP_SLIM
		free(dst);
#endif

		for (i = 0x90000/2; i < 0x94000/2; i++)
		{
			if (((rom[i] & 0xFFBF) == 0x4EB9 || rom[i] == 0x43F9) && !rom[i + 1])
				rom[i + 1] = 0x0009;

			if (rom[i] == 0x4EB8)
				rom[i] = 0x6100;
		}

		rom[0x007A8/2] = 0x0009;
		rom[0x007C8/2] = 0x0009;
		rom[0x007E8/2] = 0x0009;
		rom[0x93408/2] = 0xF168;
		rom[0x9340C/2] = 0xFB7A;
		rom[0x924AC/2] = 0x0009;
		rom[0x9251C/2] = 0x0009;
		rom[0x93966/2] = 0xFFDA;
		rom[0x93974/2] = 0xFFCC;
		rom[0x93982/2] = 0xFFBE;
		rom[0x93990/2] = 0xFFB0;
		rom[0x9399E/2] = 0xFFA2;
		rom[0x939AC/2] = 0xFF94;
		rom[0x939BA/2] = 0xFF86;
		rom[0x939C8/2] = 0xFF78;
		rom[0x939D4/2] = 0xFA5C;
		rom[0x939E0/2] = 0xFA50;
		rom[0x939EC/2] = 0xFA44;
		rom[0x939F8/2] = 0xFA38;
		rom[0x93A04/2] = 0xFA2C;
		rom[0x93A10/2] = 0xFA20;
		rom[0x93A1C/2] = 0xFA14;
		rom[0x93A28/2] = 0xFA08;
		rom[0x93A34/2] = 0xF9FC;
		rom[0x93A40/2] = 0xF9F0;
		rom[0x93A4C/2] = 0xFD14;
		rom[0x93A58/2] = 0xFD08;
		rom[0x93A66/2] = 0xF9CA;
		rom[0x93A72/2] = 0xF9BE;

		return 1;
	}
	return 0;
}

int kof10th_px_decrypt(void)
{
	UINT32 i, j;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x900000);
#endif
	UINT8 *src = memory_region_cpu1;
	UINT16 *mem16 = (UINT16 *)memory_region_cpu1;

	if (dst)
	{
		memcpy(dst + 0x000000, src + 0x700000, 0x100000);
		memcpy(dst + 0x100000, src + 0x000000, 0x800000);

		for (i = 0; i < 0x900000; i++)
		{
			j = BITSWAP24(i,23,22,21,20,19,18,17,16,15,14,13,12,11,2,9,8,7,1,5,4,3,10,6,0);
			src[j] = dst[i];
		}

#ifndef PSP_SLIM
		free(dst);
#endif

		mem16[0x0124/2] = 0x000d;
		mem16[0x0126/2] = 0xf7a8;

		mem16[0x8bf4/2] = 0x4ef9;
		mem16[0x8bf6/2] = 0x000d;
		mem16[0x8bf8/2] = 0xf980;

		mem16[0x053162c/2] = 0x7425; // Fix System
		mem16[0x053163a/2] = 0x8084; // Fix Region
		mem16[0x0531648/2] = 0x3641; // Fix some dips

		return 1;
	}
	return 0;
}

int kf10thep_px_decrypt(void)
{
	UINT32 i;
	UINT16 *rom = (UINT16*)memory_region_cpu1;
	UINT8  *src = memory_region_cpu1;
	UINT16 *buf = (UINT16*)memory_region_user2;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x200000);
#endif

	if (dst)
	{
		memcpy(dst, buf, 0x200000);
		memcpy(src + 0x000000, dst + 0x060000, 0x20000);
		memcpy(src + 0x020000, dst + 0x100000, 0x20000);
		memcpy(src + 0x040000, dst + 0x0e0000, 0x20000);
		memcpy(src + 0x060000, dst + 0x180000, 0x20000);
		memcpy(src + 0x080000, dst + 0x020000, 0x20000);
		memcpy(src + 0x0a0000, dst + 0x140000, 0x20000);
		memcpy(src + 0x0c0000, dst + 0x0c0000, 0x20000);
		memcpy(src + 0x0e0000, dst + 0x1a0000, 0x20000);
		memcpy(src + 0x0002e0, dst + 0x0402e0, 0x6a);
		memcpy(src + 0x0f92bc, dst + 0x0492bc, 0xb9e);

		for (i = 0xf92bc/2; i < 0xf9e58/2; i++)
		{
			if (rom[i + 0] == 0x4eb9 && rom[i + 1] == 0x0000) rom[i + 1] = 0x000f;
			if (rom[i + 0] == 0x4ef9 && rom[i + 1] == 0x0000) rom[i + 1] = 0x000f;
		}
		rom[0x000342/2] = 0x000f;
		rom[0x000126/2] = 0x0010; // Allow Region change
		rom[0x000228/2] = 0x4e71; // Allow System change
		rom[0x00022a/2] = 0x4e71;
		rom[0x00022c/2] = 0x4e71;
		rom[0x000234/2] = 0x4e71; // bne
		rom[0x000236/2] = 0x4e71; // bne
#ifndef PSP_SLIM
		free(dst);
#endif
		return 1;
	}
	return 0;
}

int kf2k5uni_px_decrypt(void)
{
	UINT32 i, j, offset;
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x80);
#endif
	UINT16 *mem16 = (UINT16 *)memory_region_cpu1;

	if (dst)
	{
		for (i = 0; i < 0x800000; i += 0x80)
		{
			for (j = 0; j < 0x80; j += 2)
			{
				offset = BITSWAP8(j,0,3,4,5,6,1,2,7);
				memcpy(dst + j, src + i + offset, 2);
			}
			memcpy(src + i, dst, 0x80);
		}
#ifndef PSP_SLIM
		free(dst);
#endif

		memcpy(src, src + 0x600000, 0x100000);

		mem16[0xdf6b0/2] = 0x4e71;
		mem16[0xdf6bc/2] = 0x4e71;
		mem16[0xdf6be/2] = 0x4e71;
		mem16[0xdf6ca/2] = 0x4e71;

		return 1;
	}
	return 0;
}

int kf2k2mp_px_decrypt(void)
{
	UINT32 i, j, offset;
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x80);
#endif

	if (dst)
	{
		memmove(src, src + 0x300000, 0x500000);

		for (i = 0; i < 0x800000; i += 0x80)
		{
			for (j = 0; j < 0x80 / 2; j++)
			{
				offset = BITSWAP8(j,6,7,2,3,4,5,0,1);
				memcpy(dst + j * 2, src + i + offset * 2, 2);
			}
			memcpy(src + i, dst, 0x80);
		}
#ifndef PSP_SLIM
		free(dst);
#endif
		return 1;
	}
	return 0;
}

int kf2k2mp2_px_decrypt(void)
{
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x600000);
#endif

	if (dst)
	{
		memcpy(dst + 0x000000, src + 0x1C0000, 0x040000);
		memcpy(dst + 0x040000, src + 0x140000, 0x080000);
		memcpy(dst + 0x0C0000, src + 0x100000, 0x040000);
		memcpy(dst + 0x100000, src + 0x200000, 0x400000);
		memcpy(src + 0x000000, dst + 0x000000, 0x600000);
#ifndef PSP_SLIM
		free(dst);
#endif
		return 1;
	}
	return 0;
}

int kof2k4se_px_decrypt(void)
{
	const UINT32 sec[] = { 0x300000, 0x200000, 0x100000, 0x000000 };
	UINT32 i;
	UINT8 *src = &memory_region_cpu1[0x100000];
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x400000);
#endif

	if (dst)
	{
		memcpy(dst, src, 0x400000);

		for (i = 0; i < 4; i++)
		{
			memcpy(src + i * 0x100000, dst + sec[i], 0x100000);
		}
#ifndef PSP_SLIM
		free(dst);
#endif
		return 1;
	}
	return 0;
}

int lans2004_px_decrypt(void)
{
	const UINT8 sec[] = { 0x3, 0x8, 0x7, 0xc, 0x1, 0xa, 0x6, 0xd };
	UINT32 i;
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x600000);
#endif
	UINT16 *rom = (UINT16*)memory_region_cpu1;

	if (dst)
	{
		for (i = 0; i < 8; i++)
			memcpy(dst + i * 0x20000, src + sec[i] * 0x20000, 0x20000);

		memcpy(dst + 0x0bbb00, src + 0x045b00, 0x001710);
		memcpy(dst + 0x02fff0, src + 0x1a92be, 0x000010);
		memcpy(dst + 0x100000, src + 0x200000, 0x400000);
		memcpy(src, dst, 0x600000);
#ifndef PSP_SLIM
		free(dst);
#endif

		for (i = 0xbbb00/2; i < 0xbe000/2; i++)
		{
			if ((((rom[i] & 0xffbf) == 0x4eb9) || ((rom[i] & 0xffbf) == 0x43b9)) && (rom[i + 1] == 0x0000))
			{
				rom[i + 1] = 0x000b;
				rom[i + 2] += 0x6000;
			}
		}

		rom[0x2d15c/2] = 0x000b;
		rom[0x2d15e/2] = 0xbb00;
		rom[0x2d1e4/2] = 0x6002;
		rom[0x2ea7e/2] = 0x6002;
		rom[0xbbcd0/2] = 0x6002;
		rom[0xbbdf2/2] = 0x6002;
		rom[0xbbe42/2] = 0x6002;
		return 1;
	}
	return 0;
}

int svcboot_px_decrypt(void)
{
	const UINT8 sec[] = { 0x06, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00 };
	UINT32 i, offset;
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(memory_length_cpu1);
#endif

	if (dst)
	{
		for (i = 0; i < memory_length_cpu1 / 0x100000; i++)
		{
			memcpy(&dst[i << 20], &src[sec[i] << 20], 0x100000);
		}
		for (i = 0; i < memory_length_cpu1 / 2; i++)
		{
			offset = BITSWAP8((i & 0x0000ff),7,6,1,0,3,2,5,4);
			offset += (i & 0xffff00);
			memcpy(&src[i << 1], &dst[offset << 1], 2);
		}
#ifndef PSP_SLIM
		free(dst);
#endif
		return 1;
	}
	return 0;
}

int svcplus_px_decrypt(void)
{
	const UINT8 sec[] = { 0x00, 0x03, 0x02, 0x05, 0x04, 0x01 };
	UINT32 i, offset;
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(memory_length_cpu1);
#endif

	if (dst)
	{
		memcpy(dst, src, memory_length_cpu1);
		for (i = 0; i < memory_length_cpu1 / 2; i++)
		{
			offset = BITSWAP24((i & 0xfffff),0x17,0x16,0x15,0x14,0x13,0x00,0x01,0x02,0x0f,0x0e,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x10,0x11,0x12);
			offset ^= 0x0f0007;
			offset += (i & 0xff00000);
			memcpy(&src[i << 1], &dst[offset << 1], 2);
		}

		memcpy(dst, src, memory_length_cpu1);
		for (i = 0; i < 6; i++)
		{
			memcpy(&src[i << 20], &dst[sec[i] << 20], 0x100000);
		}
#ifndef PSP_SLIM
		free(dst);
#endif

		src[0x0f8010] = 0x40;
		src[0x0f8011] = 0x04;
		src[0x0f8012] = 0x00;
		src[0x0f8013] = 0x10;
		src[0x0f8014] = 0x40;
		src[0x0f8015] = 0x46;
		src[0x0f8016] = 0xc1;
		src[0x0f802c] = 0x16;

		return 1;
	}
	return 0;
}

int svcplusa_px_decrypt(void)
{
	const UINT8 sec[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x00 };
	UINT32 i;
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(memory_length_cpu1);
#endif

	if (dst)
	{
		memcpy(dst, src, memory_length_cpu1);
		for (i = 0; i < 6; i++)
		{
			memcpy(&src[i << 20], &dst[sec[i] << 20], 0x100000);
		}
#ifndef PSP_SLIM
		free(dst);
#endif

		src[0x0f8010] = 0x40;
		src[0x0f8011] = 0x04;
		src[0x0f8012] = 0x00;
		src[0x0f8013] = 0x10;
		src[0x0f8014] = 0x40;
		src[0x0f8015] = 0x46;
		src[0x0f8016] = 0xc1;
		src[0x0f802c] = 0x16;

		return 1;
	}
	return 0;
}

int svcsplus_px_decrypt(void)
{
	const UINT8 sec[] = { 0x06, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00 };
	UINT32 i, offset;
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(memory_length_cpu1);
#endif
	UINT16 *mem16 = (UINT16 *)memory_region_cpu1;

	if (dst)
	{
		memcpy(dst, src, memory_length_cpu1);

		for (i = 0; i < memory_length_cpu1 / 2; i++)
		{
			offset = BITSWAP16((i & 0x007fff),0x0f,0x00,0x08,0x09,0x0b,0x0a,0x0c,0x0d,0x04,0x03,0x01,0x07,0x06,0x02,0x05,0x0e);

			offset += (i & 0x078000);
			offset += sec[(i & 0xf80000) >> 19] << 19;
			memcpy(&src[i << 1], &dst[offset << 1], 2);
		}
#ifndef PSP_SLIM
		free(dst);
#endif

		mem16[0x9e90/2] = 0x000f;
		mem16[0x9e92/2] = 0xc9c0;
		mem16[0xa10c/2] = 0x4eb9;
		mem16[0xa10e/2] = 0x000e;
		mem16[0xa110/2] = 0x9750;

		return 1;
	}
	return 0;
}

int kf2k3bl_px_decrypt(void)
{
	const UINT8 sec[] = { 0x07, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	UINT32 i;
	UINT8 *rom = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *buf = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *buf = (UINT8 *)malloc(0x800000);
#endif

	if (buf)
	{
		memcpy(buf, rom, 0x800000);

		for (i = 0; i < 8; i++)
		{
			memcpy(&rom[i * 0x100000], &buf[sec[i] * 0x100000], 0x100000);
		}
#ifndef PSP_SLIM
		free(buf);
#endif
		return 1;
	}
	return 0;
}

int kf2k3pl_px_decrypt(void)
{
	UINT32 i, j;
#ifdef PSP_SLIM
	UINT16 *buf = (UINT16 *)psp2k_mem_offset;
#else
	UINT16 *buf = (UINT16 *)malloc(0x100000);
#endif
	UINT16 *rom = (UINT16*)memory_region_cpu1;

	if (buf)
	{
		for (i = 0; i < 0x700000/2; i += 0x100000/2)
		{
			memcpy(buf, &rom[i], 0x100000);
			for (j = 0; j < 0x100000/2; j++)
				rom[i + j] = buf[BITSWAP24(j,23,22,21,20,19,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18)];
		}
#ifndef PSP_SLIM
		free(buf);
#endif

		rom[0xf38ac/2] = 0x4e75;
		return 1;
	}
	return 0;
}

void kf2k3upl_px_decrypt(void)
{
	UINT32 i, offset;
	UINT8 *rom = memory_region_cpu1;
	UINT8 *buf;

	memmove(rom + 0x100000, rom, 0x600000);
	memmove(rom, rom + 0x700000, 0x100000);

	rom = memory_region_cpu1 + 0xfe000;
	buf = memory_region_cpu1 + 0xd0610;

	for (i = 0; i < 0x2000 / 2; i++)
	{
		offset = (i & 0xff00) + BITSWAP8((i & 0x00ff),7,6,0,4,3,2,1,5);
		memcpy(&rom[i << 1], &buf[offset << 1], 2);
	}
}

int samsho5b_px_decrypt(void)
{
	UINT32 i, offset;
	UINT8 *rom = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *buf = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *buf = (UINT8 *)malloc(memory_length_cpu1);
#endif

	if (buf)
	{
		memcpy(buf, rom, memory_length_cpu1);

		for (i = 0; i < memory_length_cpu1 / 2; i++)
		{
			offset = BITSWAP8((i & 0x000ff),7,6,5,4,3,0,1,2);
			offset += (i & 0xfffff00);
			offset ^= 0x060005;

			memcpy(&rom[i << 1], &buf[offset << 1], 2);
		}

		memcpy(buf, rom, memory_length_cpu1);

		memcpy(&rom[0x000000], &buf[0x700000], 0x100000);
		memcpy(&rom[0x100000], &buf[0x000000], 0x700000);

#ifndef PSP_SLIM
		free(buf);
#endif
		return 1;
	}
	return 0;
}

void kof96ep_px_decrypt(void)
{
	UINT32 i, j;
	UINT8 *rom = memory_region_cpu1;

	for (i = 0; i < 0x080000; i++)
	{
		j = i + 0x300000;
		if (rom[j] - rom[i] == 8) rom[j] = rom[i];
	}

	memcpy(rom, rom + 0x300000, 0x080000);
}

int cthd2k3a_px_decrypt(void)
{
	UINT32 i;
	UINT8 bank[] =
	{
		0x06, 0x02, 0x04, 0x05, 0x01, 0x03, 0x00, 0x07,
		0x27, 0x0e, 0x1c, 0x15, 0x1b, 0x17, 0x0a, 0x0f,
		0x16, 0x14, 0x23, 0x0b, 0x22, 0x26, 0x08, 0x24,
		0x21, 0x13, 0x1a, 0x0c, 0x19, 0x1d, 0x25, 0x10,
		0x09, 0x20, 0x18, 0x1f, 0x1e, 0x12, 0x0d, 0x11
	};
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x500000);
#endif

	if (dst)
	{
		for (i = 0; i < 0x500000 / 0x20000; i++)
		{
			memcpy(dst + i * 0x20000, src + bank[i] * 0x20000, 0x20000);
		}
		memcpy(src, dst, 0x500000);
#ifndef PSP_SLIM
		free(dst);
#endif
		return 1;
	}
	return 0;
}

int kf2k4pls_px_decrypt(void)
{
	UINT8 *src = memory_region_cpu1;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = (UINT8 *)malloc(0x200000);
#endif

	if (dst)
	{
		memcpy(dst, src, 0x100000);
		memcpy(dst + 0x100000, src + 0x500000, 0x100000);
		memcpy(src + 0x000000, dst + 0x1a0000, 0x020000);
		memcpy(src + 0x020000, dst + 0x080000, 0x020000);
		memcpy(src + 0x040000, dst + 0x140000, 0x020000);
		memcpy(src + 0x060000, dst + 0x000000, 0x020000);
		memcpy(src + 0x080000, dst + 0x180000, 0x020000);
		memcpy(src + 0x0a0000, dst + 0x0a0000, 0x020000);
		memcpy(src + 0x0c0000, dst + 0x100000, 0x020000);
		memcpy(src + 0x0e0000, dst + 0x040000, 0x020000);
#ifndef PSP_SLIM
		free(dst);
#endif
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------
	あらかじめ復号処理を行う必要が無いもの
-----------------------------------------------------*/

void cthd2003_sx_decrypt(void)
{
	UINT8 *rom = memory_region_gfx2;
	UINT8 buf[8*32*128];

	memcpy(buf, rom + 8*64*128, 8*32*128);
	memcpy(rom + 8*64*128, rom + 8*32*128, 8*32*128);
	memcpy(rom + 8*32*128, buf, 8*32*128);
}

void cthd2003_mx_decrypt(void)
{
	UINT8 *rom = memory_region_cpu2 + 0x10000;
	UINT8 buf[8*32*128];

	memcpy(buf, rom + 8*64*128, 8*32*128);
	memcpy(rom + 8*64*128, rom + 8*32*128, 8*32*128);
	memcpy(rom + 8*32*128, buf, 8*32*128);
}

void kf2k5uni_sx_decrypt(void)
{
	UINT32 i;
	UINT8 *rom = memory_region_gfx2;

	for (i = 0; i < 0x20000; i++)
		rom[i] = BITSWAP8(rom[i],4,5,6,7,0,1,2,3);
}

void kf2k5uni_mx_decrypt(void)
{
	UINT32 i;
	UINT8 *rom = memory_region_cpu2;

	for (i = 0; i < 0x30000; i++)
		rom[i] = BITSWAP8(rom[i],4,5,6,7,0,1,2,3);
}

void kf10thep_sx_decrypt(void)
{
	UINT32 i, j;
	UINT8 *rom = memory_region_gfx2;
	UINT8 buf[16];

	for (i = 0; i < 0x20000; i += 16)
	{
		memcpy(buf, &rom[i], 16);

		for (j = 0; j < 16; j++)
		{
			rom[i + j] = buf[j ^ 0x08];
		}
	}
}

void kf2k1pa_sx_decrypt(void)
{
	UINT32 i;
	UINT8 *rom = memory_region_gfx2;

	for (i = 0; i < 0x20000; i++)
		rom[i] = BITSWAP8(rom[i], 3, 2, 4, 5, 1, 6, 0, 7);
}

#define MATRIMBLFIX(i) (i^(BITSWAP8(i&0x3,4,3,1,2,0,7,6,5)<<8))

void matrimbl_mx_decrypt(void)
{
	UINT8 *src = memory_region_cpu2 + 0x10000;
#ifdef PSP_SLIM
	UINT8 *dst = (UINT8 *)psp2k_mem_offset;
#else
	UINT8 *dst = malloc(0x20000);
#endif
	UINT32 i, j = 0;

	if (dst)
	{
		memcpy(dst, src, 0x20000);

		for (i = 0x00000; i < 0x20000; i++)
		{
			if (i & 0x10000)
			{
				if (i & 0x800)
				{
					j = MATRIMBLFIX(i);
					j = j ^ 0x10000;
				}
				else
				{
					j = MATRIMBLFIX((i ^ 0x01));
				}
			}
			else
			{
				if (i & 0x800)
				{
					j = MATRIMBLFIX((i ^ 0x01));
					j = j ^ 0x10000;
				}
				else
				{
					j = MATRIMBLFIX(i);
				}
			}
			src[j] = dst[i];
		}
#ifndef PSP_SLIM
		free(dst);
#endif
	}
}

/*-----------------------------------------------------
	パッチ
-----------------------------------------------------*/

void patch_cthd2003(void)
{
	UINT32 i;
	UINT16 *mem16 = (UINT16 *)memory_region_cpu1;

	mem16[0xf415a/2] = 0x4ef9;
	mem16[0xf415c/2] = 0x000f;
	mem16[0xf415e/2] = 0x4cf2;

	for (i = 0x1ae290/2; i < 0x1ae8d0/2; i++)
	{
		mem16[i] = 0x0000;
	}

	for (i = 0x1f8ef0/2; i < 0x1fa1f0/2; i += 2)
	{
		mem16[i + 0] -= 0x7000;
		mem16[i + 1] -= 0x0010;
	}

	for (i = 0xac500/2; i < 0xac520/2; i++)
	{
		mem16[i] = 0xffff;
	}

	mem16[0x991d0/2] = 0xdd03;
	mem16[0x99306/2] = 0xdd03;
	mem16[0x99354/2] = 0xdd03;
	mem16[0x9943e/2] = 0xdd03;
}

void patch_kof97pla(void)
{
	UINT8 *mem8 = memory_region_cpu1;

	mem8[0x1394a] = 0x6c;
	mem8[0x1394b] = 0xd1;
	mem8[0x1394c] = 0x3a;
	mem8[0x1394d] = 0x1;
	mem8[0x1394e] = 0x6c;
	mem8[0x1394f] = 0x91;
}

void patch_fr2ch(void)
{
	UINT16 *src = (UINT16 *)memory_region_cpu1;
	UINT8 *rom = memory_region_cpu1;
	UINT8 *dst = memory_region_gfx1;
	int i;
	UINT8 data[16] =
	{
		0x49, 0x46, 0x41, 0x4E, 0x20, 0x4C, 0x4F, 0x52,
		0x41, 0x4D, 0x43, 0x4E, 0x20, 0x45, 0x20, 0x32
	};

	// change jsr to C004DA
	src[0x01AF8 >> 1] = 0x04DA; // C00552 (Not used?)
	src[0x01BF6 >> 1] = 0x04DA; // C0056A (fixes crash)
	src[0x01ED8 >> 1] = 0x04DA; // C00570 (Not used?)
	src[0x1C384 >> 1] = 0x04DA; // C00552 (fixes crash)

	// 0x001C06 - this routine can cause a loop/freeze
	src[0x01C06 >> 1] = 0x4E75;

	// can cause bugs
	// Move text for credit + coin info (Thanks to Kanyero)
	memcpy(dst, dst + 0x600, 0x140);

	// Patch out neogeo intro (Moving S causes garbage)
	src[0x00112 >> 1] = 0x0180;
	src[0x00114 >> 1] = 0x0180;

	// optional
	// Hack in the proper identification (see setup menu [F2])
	for (i = 0; i < 0x10; i++)
	{
		rom[0x3A6 + i] = rom[0x61E + i] = rom[0x896 + i] = data[i];
	}
	for (i = 0; i < 0x20; i += 4)
	{
		src[(0x40 + i + 2) >> 1] = 0x0426;
	}

	// Album Fix
	src[0x1C382 >> 1] = 0x0008; // C00552
	src[0x1C384 >> 1] = 0x0000;
	src[0x80000 >> 1] = 0x33FC;
	src[0x80002 >> 1] = 0x0001;
	src[0x80004 >> 1] = 0x0020;
	src[0x80006 >> 1] = 0x0002;
	src[0x80008 >> 1] = 0x4E75;
}

#endif /* RELEASE */

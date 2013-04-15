/******************************************************************************

	loadrom.c

	ROMイメージファイルロード関数

******************************************************************************/

#ifndef LOAD_ROM_H
#define LOAD_ROM_H

enum
{
	ROM_LOAD = 0,
	ROM_CONTINUE,
	ROM_WORDSWAP,
	MAP_MAX
};

struct rom_t
{
	UINT32 type;
	UINT32 offset;
	UINT32 length;
	UINT32 crc;
	int group;
	int skip;
	char name[32];
};

#if (EMU_SYSTEM != NCDZ)
int file_open(const char *fname1, const char *fname2, const UINT32 crc, char *fname);
void file_close(void);
int file_read(void *buf, size_t length);
int file_getc(void);
int rom_load(struct rom_t *rom, UINT8 *mem, int idx, int max);
#endif

#if USE_CACHE && (EMU_SYSTEM == MVS)
enum
{
	CACHE_INFO = 0,
	CACHE_CROM,
	CACHE_SROM,
	CACHE_VROM
};

int cachefile_open(int type);
#endif

void error_memory(const char *mem_name);
void error_crc(const char *rom_name);
void error_file(const char *rom_name);

#endif /* LOAD_ROM_H */

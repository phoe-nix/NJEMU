/******************************************************************************

	zfile.c

	ZIPƒtƒ@ƒCƒ‹‘€ìŠÖ”

******************************************************************************/

#ifndef ZFILE_H
#define ZFILE_H

struct zip_find_t
{
	char name[MAX_PATH];
	UINT32  length;
	UINT32  crc32;
};

int zip_open(const char *path);
void zip_close(void);

int zip_findfirst(struct zip_find_t *file);
int zip_findnext(struct zip_find_t *file);

int zopen(const char *filename);
int zread(int fd, void *buf, unsigned size);
int zgetc(int fd);
int zclose(int fd);
int zsize(int fd);
#if (EMU_SYSTEM == NCDZ)
int zlength(const char *filename);
#endif

#endif /* ZFILE_H */

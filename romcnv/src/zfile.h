#ifndef ZFILE_H
#define ZFILE_H

#ifndef MAX_PATH
#define MAX_PATH	512
#endif

struct zip_find_t
{
	char name[MAX_PATH];
	uint32_t length;
	uint32_t crc32;
};

int zip_open(const char *path, const char *mode);
void zip_close(void);

int zip_findfirst(struct zip_find_t *file);
int zip_findnext(struct zip_find_t *file);

int zopen(const char *filename);
int zread(int fd, void *buf, unsigned size);
int zwrite(int fd, void *buf, unsigned size);
int zgetc(int fd);
int zclose(int fd);
int zsize(int fd);
int zcrc(int fd);

#endif /* ZFILEH */

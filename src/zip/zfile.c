/******************************************************************************

	zfile.c

	ZIPファイル操作関数

******************************************************************************/

#include "emumain.h"
#include "zip/unzip.h"


/******************************************************************************
	ローカル変数
******************************************************************************/

static unzFile unzfile = NULL;

static char basedir[MAX_PATH];
static char *basedirend;
static char zip_cache[4096];
static int  zip_cached_len;
static int  zip_filepos;


/******************************************************************************
	グローバル関数
******************************************************************************/

/*------------------------------------------------------
	ZIPファイルを開く
------------------------------------------------------*/

int zip_open(const char *path)
{
	if (unzfile != NULL) zip_close();

	if ((unzfile = unzOpen(path)) != NULL)
		return (int)unzfile;

	strcpy(basedir, path);
	strcat(basedir, "/");
	basedirend = strrchr(basedir, '/') + 1;

	return -1;
}


/*------------------------------------------------------
	ZIPファイルを閉じる
------------------------------------------------------*/

void zip_close(void)
{
	if (unzfile)
	{
		unzClose(unzfile);
		unzfile = NULL;
	}
}


/*------------------------------------------------------
	ZIPファイル内のファイルを検索 (初回)
------------------------------------------------------*/

int zip_findfirst(struct zip_find_t *file)
{
	if (unzfile)
	{
		if (unzGoToFirstFile(unzfile) == UNZ_OK)
		{
			unz_file_info info;

			unzGetCurrentFileInfo(unzfile, &info, file->name, MAX_PATH);
			file->length = info.uncompressed_size;
			file->crc32 = info.crc;
			return 1;
		}
	}
	return 0;
}


/*------------------------------------------------------
	ZIPファイル内のファイルを検索 (2回目以降)
------------------------------------------------------*/

int zip_findnext(struct zip_find_t *file)
{
	if (unzfile)
	{
		if (unzGoToNextFile(unzfile) == UNZ_OK)
		{
			unz_file_info info;

			unzGetCurrentFileInfo(unzfile, &info, file->name, MAX_PATH);
			file->length = info.uncompressed_size;
			file->crc32 = info.crc;
			return 1;
		}
	}
	return 0;
}


/*------------------------------------------------------
	ZIPファイル内のファイルを開く
------------------------------------------------------*/

int zopen(const char *filename)
{
	zip_cached_len = 0;

	if (unzfile == NULL)
	{
		SceUID fd;

		strcpy(basedirend, filename);
		fd = sceIoOpen(basedir, PSP_O_RDONLY, 0777);
		return (fd < 0) ? -1 : (int)fd;
	}

	if (unzLocateFile(unzfile, filename) == UNZ_OK)
		if (unzOpenCurrentFile(unzfile) == UNZ_OK)
			return (int)unzfile;

	return -1;
}


/*------------------------------------------------------
	ZIPファイル内のファイルを閉じる
------------------------------------------------------*/

int zclose(int fd)
{
	zip_cached_len = 0;

	if (unzfile == NULL)
	{
		if (fd != -1) sceIoClose((SceUID)fd);
		return 0;
	}
	return unzCloseCurrentFile(unzfile);
}


/*------------------------------------------------------
	ZIPファイル内のファイルを読み込む
------------------------------------------------------*/

int zread(int fd, void *buf, unsigned size)
{
	if (unzfile == NULL)
		return sceIoRead((SceUID)fd, buf, size);

	return unzReadCurrentFile(unzfile, buf, size);
}


/*------------------------------------------------------
	ZIPファイル内のファイルから1バイト読み込む
------------------------------------------------------*/

int zgetc(int fd)
{
	if (zip_cached_len == 0)
	{
		if (unzfile == NULL)
			zip_cached_len = sceIoRead((SceUID)fd, zip_cache, 4096);
		else
			zip_cached_len = unzReadCurrentFile(unzfile, zip_cache, 4096);
		if (zip_cached_len == 0) return EOF;
		zip_filepos = 0;
	}
	zip_cached_len--;
	return zip_cache[zip_filepos++] & 0xff;
}


/*------------------------------------------------------
	ZIPファイル内のファイルのサイズを取得
------------------------------------------------------*/

int zsize(int fd)
{
	unz_file_info info;

	if (unzfile == NULL)
	{
		int len, pos = sceIoLseek(fd, 0, SEEK_CUR);

		len = sceIoLseek(fd, 0, SEEK_END);
		sceIoLseek(fd, pos, SEEK_CUR);

		return len;
	}

	unzGetCurrentFileInfo(unzfile, &info, NULL, 0);

	return info.uncompressed_size;
}


/*------------------------------------------------------
	ZIPファイル内のファイルのサイズを取得
	(ZIPファイルを開かずにファイル名指定で取得)
------------------------------------------------------*/

#if (EMU_SYSTEM == NCDZ)
int zlength(const char *filename)
{
	int fd, length;

	if ((fd = zopen(filename)) != -1)
	{
		length = zsize(fd);
		zclose(fd);
		return length;
	}
	return -1;
}
#endif

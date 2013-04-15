#include "romcnv.h"
#include "unzip.h"
#include "zip.h"
#include <time.h>


#define ZIP_NOTOPEN		0
#define ZIP_READOPEN	1
#define ZIP_WRITEOPEN	2


static zipFile zipfile = NULL;

static char basedir[MAX_PATH];
static char *basedirend;
static char zip_cache[4096];
static int  zip_cached_len;
static int  zip_filepos;
static int  zip_mode = ZIP_NOTOPEN;
static int  zip_fd = -1;


int zip_open(const char *path, const char *mode)
{
	if (zipfile) zip_close();

	if (!strcmp(mode, "rb"))
	{
		zip_mode = ZIP_READOPEN;

		if ((zipfile = unzOpen(path)) != NULL)
			return (int)zipfile;

		strcpy(basedir, path);

		basedirend = basedir + strlen(basedir);
		*basedirend++ = '/';
	}
	else if (!strcmp(mode, "wb"))
	{
		if ((zipfile = zipOpen(path, 0)) != NULL)
		{
			zip_mode = ZIP_WRITEOPEN;
			return (int)zipfile;
		}
	}

	return -1;
}


void zip_close(void)
{
	if (zip_fd != -1) zclose(zip_fd);

	if (zipfile)
	{
		if (zip_mode == ZIP_READOPEN)
			unzClose(zipfile);
		else
			zipClose(zipfile, NULL);
		zipfile = NULL;
	}

	zip_mode = ZIP_NOTOPEN;
}


int zip_findfirst(struct zip_find_t *file)
{
	if (zipfile && zip_mode == ZIP_READOPEN)
	{
		if (unzGoToFirstFile(zipfile) == UNZ_OK)
		{
			unz_file_info info;

			unzGetCurrentFileInfo(zipfile, &info, file->name, MAX_PATH, NULL, 0, NULL, 0);
			file->length = info.uncompressed_size;
			file->crc32 = info.crc;
			return 1;
		}
	}
	return 0;
}


int zip_findnext(struct zip_find_t *file)
{
	if (zipfile && zip_mode == ZIP_READOPEN)
	{
		if (unzGoToNextFile(zipfile) == UNZ_OK)
		{
			unz_file_info info;

			unzGetCurrentFileInfo(zipfile, &info, file->name, MAX_PATH, NULL, 0, NULL, 0);
			file->length = info.uncompressed_size;
			file->crc32 = info.crc;
			return 1;
		}
	}
	return 0;
}


int zopen(const char *filename)
{
	zip_cached_len = 0;

	if (zip_mode == ZIP_READOPEN)
	{
		if (!zipfile)
		{
			FILE *fp;

			strcpy(basedirend, filename);
			if ((fp = fopen(basedir, "rb")) != NULL)
			{
				zip_fd = (int)fp;
				return zip_fd;
			}
			return -1;
		}

		if (unzLocateFile(zipfile, filename, 0) == UNZ_OK)
			if (unzOpenCurrentFile(zipfile) == UNZ_OK)
				return (int)zipfile;
	}
	else
	{
		zip_fileinfo zip_info;
		struct tm *nowtime;
		time_t ltime;

		memset(&zip_info, 0, sizeof(zip_info));

		time(&ltime);
		nowtime = localtime(&ltime);

		zip_info.tmz_date.tm_sec  = nowtime->tm_sec;
		zip_info.tmz_date.tm_min  = nowtime->tm_min;
		zip_info.tmz_date.tm_hour = nowtime->tm_hour;
		zip_info.tmz_date.tm_mday = nowtime->tm_mday;
		zip_info.tmz_date.tm_mon  = nowtime->tm_mon ;
		zip_info.tmz_date.tm_year = nowtime->tm_year;

		if (zipOpenNewFileInZip(zipfile, filename, &zip_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9) == ZIP_OK)
			return (int)zipfile;
	}
	return -1;
}


int zclose(int fd)
{
	zip_cached_len = 0;

	if (zip_mode == ZIP_READOPEN)
	{
		if (!zipfile)
		{
			if (fd != -1) fclose((FILE *)fd);
			zip_fd = -1;
			return 0;
		}
		return unzCloseCurrentFile(zipfile);
	}
	else
	{
		return zipCloseFileInZip(zipfile);
	}
}


int zread(int fd, void *buf, unsigned size)
{
	if (zip_mode == ZIP_READOPEN)
	{
		if (!zipfile)
			return fread(buf, 1, size, (FILE *)fd);
		else
			return unzReadCurrentFile(zipfile, buf, size);
	}
	return 0;
}


int zwrite(int fd, void *buf, unsigned size)
{
	if (zip_mode == ZIP_WRITEOPEN)
	{
		return zipWriteInFileInZip(zipfile, buf, size);
	}
	return 0;
}


int zgetc(int fd)
{
	if (zip_mode == ZIP_READOPEN)
	{
		if (!zipfile) return fgetc((FILE *)fd);

		if (zip_cached_len == 0)
		{
			zip_cached_len = unzReadCurrentFile(zipfile, zip_cache, 4096);
			if (zip_cached_len == 0) return EOF;
			zip_filepos = 0;
		}
		zip_cached_len--;
		return zip_cache[zip_filepos++] & 0xff;
	}
	return EOF;
}


int zsize(int fd)
{
	if (zip_mode == ZIP_READOPEN)
	{
		unz_file_info info;

		if (zipfile == NULL)
		{
			FILE *fp = (FILE *)fd;
			int len, pos = ftell(fp);

			fseek(fp, 0, SEEK_END);
			len = ftell(fp);
			fseek(fp, pos, SEEK_SET);

			return len;
		}

		unzGetCurrentFileInfo(zipfile, &info, NULL, 0, NULL, 0, NULL, 0);

		return info.uncompressed_size;
	}
	return 0;
}


int zcrc(int fd)
{
	if (zipfile && zip_mode == ZIP_READOPEN)
	{
		unz_file_info info;

		unzGetCurrentFileInfo(zipfile, &info, NULL, 0, NULL, 0, NULL, 0);

		return info.crc;
	}
	return 0;
}

/* unzip.c -- IO on .zip files using zlib
   Version 0.15 beta, Mar 19th, 1998,

   Read unzip.h for more info
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "zlib.h"
#include "unzip.h"

#ifdef STDC
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#endif
#ifdef NO_ERRNO_H
	extern int errno;
#else
#include <errno.h>
#endif


#if !defined(unix) && !defined(CASESENSITIVITYDEFAULT_YES) && !defined(CASESENSITIVITYDEFAULT_NO)
#define CASESENSITIVITYDEFAULT_NO
#endif

#ifndef UNZ_BUFSIZE
#define UNZ_BUFSIZE (16384)
#endif

#ifndef UNZ_MAXFILENAMEINZIP
#define UNZ_MAXFILENAMEINZIP (256)
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)


/* I've found an old Unix (a SunOS 4.1.3_U1) without all SEEK_* defined.... */

#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif

#ifndef SEEK_END
#define SEEK_END    2
#endif

#ifndef SEEK_SET
#define SEEK_SET    0
#endif

const char unz_copyright[] = " unzip 0.15 Copyright 1998 Gilles Vollant ";

/* unz_file_info_interntal contain internal info about a file in zipfile*/
typedef struct unz_file_info_internal_s
{
	uLong offset_curfile;/* relative offset of static header 4 bytes */
} unz_file_info_internal;


/* zip_read_info_s contain internal information about a file in zipfile,
    when reading and decompress it */
typedef struct
{
	char read_buffer[UNZ_BUFSIZE];  /* internal buffer for compressed data */
	z_stream stream;            /* zLib stream structure for inflate */

	uLong pos_in_zipfile;       /* position in byte on the zipfile, for fseek*/
	uLong stream_initialised;   /* flag set if stream structure is initialised*/

	uLong offset_static_extrafield;/* offset of the static extra field */
	uInt  size_static_extrafield;/* size of the static extra field */
	uLong pos_static_extrafield;   /* position in the static extra field in read*/

	uLong crc32;                /* crc32 of all data uncompressed */
	uLong crc32_wait;           /* crc32 we must obtain after decompress all */
	uLong rest_read_compressed; /* number of byte to be decompressed */
	uLong rest_read_uncompressed;/*number of byte to be obtained after decomp*/
	FILE* file;                 /* io structore of the zipfile */
	uLong compression_method;   /* compression method (0==store) */
	uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
} zip_read_info_s;


/* unz_s contain internal information about the zipfile
*/
typedef struct
{
	FILE *file;                 /* io structore of the zipfile */
	unz_global_info gi;       /* public global information */
	uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
	uLong num_file;             /* number of the current file in the zipfile*/
	uLong pos_in_central_dir;   /* pos of the current file in the central dir*/
	uLong current_file_ok;      /* flag about the usability of the current file*/
	uLong central_pos;          /* position of the beginning of the central dir*/

	uLong size_central_dir;     /* size of the central directory  */
	uLong offset_central_dir;   /* offset of start of central directory with
								   respect to the starting disk number */

	unz_file_info cur_file_info; /* public info about the current file in zip*/
	unz_file_info_internal cur_file_info_internal; /* private info about it*/
    zip_read_info_s *pzip_read_info; /* structure about the current
	                                    file if we are decompressing it */
} unz_s;


static zip_read_info_s zip_read_info;
static unz_s unz;

/* ===========================================================================
     Read a byte from a gz_stream; update next_in and avail_in. Return EOF
   for end of file.
   IN assertion: the stream s has been sucessfully opened for reading.
*/


static int unzstatic_getByte(FILE *fin, int *pi)
{
	unsigned char c;
	int err = fread(&c, 1, 1, fin);

	if (err == 1)
	{
		*pi = (int)c;
		return UNZ_OK;
	}
	else
	{
		if (ferror(fin))
			return UNZ_ERRNO;
		else
			return UNZ_EOF;
	}
}


/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets
*/
static int unzstatic_getShort(FILE *fin, uLong *pX)
{
	uLong x;
	int i = 0;
	int err;

	err = unzstatic_getByte(fin, &i);
	x = (uLong)i;

	if (err == UNZ_OK)
		err = unzstatic_getByte(fin, &i);
	x += ((uLong)i) << 8;

	*pX = (err == UNZ_OK) ? x : 0;

	return err;
}

static int unzstatic_getLong(FILE *fin, uLong *pX)
{
	uLong x;
	int i = 0;
	int err;

	err = unzstatic_getByte(fin, &i);
	x = (uLong)i;

	if (err == UNZ_OK)
		err = unzstatic_getByte(fin, &i);
	x += ((uLong)i) << 8;

	if (err ==UNZ_OK)
		err = unzstatic_getByte(fin, &i);
	x += ((uLong)i) << 16;

	if (err ==UNZ_OK)
		err = unzstatic_getByte(fin, &i);
	x += ((uLong)i) << 24;

	*pX = (err == UNZ_OK) ? x : 0;

	return err;
}


/* My own strcmpi / strcasecmp */
static int strcmpcasenosensitive_internal(const char *fileName1, const char *fileName2)
{
	while (1)
	{
		char c1 = *fileName1++;
		char c2 = *fileName2++;

		if ((c1 >= 'a') && (c1 <= 'z'))
			c1 -= 0x20;
		if ((c2 >= 'a') && (c2 <= 'z'))
			c2 -= 0x20;
		if (c1 == '\0')
			return ((c2 == '\0') ? 0 : -1);
		if (c2 == '\0')
			return 1;
		if (c1 < c2)
			return -1;
		if (c1 > c2)
			return 1;
	}
}


#ifdef  CASESENSITIVITYDEFAULT_NO
#define CASESENSITIVITYDEFAULTVALUE 2
#else
#define CASESENSITIVITYDEFAULTVALUE 1
#endif

#ifndef STRCMPCASENOSENTIVEFUNCTION
#define STRCMPCASENOSENTIVEFUNCTION strcmpcasenosensitive_internal
#endif

/*
   Compare two filename (fileName1,fileName2).
   If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
   If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi
                                                                or strcasecmp)
   If iCaseSenisivity = 0, case sensitivity is defaut of your operating system
        (like 1 on Unix, 2 on Windows)

*/
int unzStringFileNameCompare(const char *fileName1, const char *fileName2, int iCaseSensitivity)
{
	if (iCaseSensitivity == 0)
		iCaseSensitivity=CASESENSITIVITYDEFAULTVALUE;

	if (iCaseSensitivity == 1)
		return strcmp(fileName1, fileName2);

	return STRCMPCASENOSENTIVEFUNCTION(fileName1, fileName2);
}

#define BUFREADCOMMENT (0x400)

/*
  Locate the Central directory of a zipfile (at the end, just before
  the global comment)
*/
static uLong unzstatic_SearchCentralDir(FILE *fin)
{
	unsigned char *buf;
	uLong uSizeFile;
	uLong uBackRead;
	uLong uMaxBack = 0xffff; /* maximum size of global comment */
	uLong uPosFound = 0;

	if (fseek(fin, 0, SEEK_END) != 0)
		return 0;

	uSizeFile = ftell(fin);

	if (uMaxBack > uSizeFile)
		uMaxBack = uSizeFile;

	buf = (unsigned char *)malloc(BUFREADCOMMENT + 4);
	if (buf == NULL)
		return 0;

	uBackRead = 4;
	while (uBackRead < uMaxBack)
	{
		uLong uReadSize, uReadPos;
		int i;

		if (uBackRead+BUFREADCOMMENT > uMaxBack)
			uBackRead = uMaxBack;
		else
			uBackRead += BUFREADCOMMENT;
		uReadPos = uSizeFile - uBackRead;

		uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ?  (BUFREADCOMMENT + 4) : (uSizeFile - uReadPos);
		if (fseek(fin, uReadPos, SEEK_SET) != 0)
			break;

		if (fread(buf, (uInt)uReadSize, 1, fin) != 1)
			break;

		for (i = (int)uReadSize - 3; (i--) > 0;)
		{
			if (((*(buf + i + 0)) == 0x50) && ((*(buf + i + 1)) == 0x4b) &&
				((*(buf + i + 2)) == 0x05) && ((*(buf + i + 3)) == 0x06))
			{
				uPosFound = uReadPos + i;
				break;
			}
		}

		if (uPosFound != 0)
			break;
	}

	free(buf);

	return uPosFound;
}

/*
  Open a Zip file. path contain the full pathname (by example,
	 on a Windows NT computer "c:\\test\\zlib109.zip" or on an Unix computer
	 "zlib/zlib109.zip".
	 If the zipfile cannot be opened (file don't exist or in not valid), the
	   return value is NULL.
	 Else, the return value is a unzFile Handle, usable with other function
	   of this unzip package.
*/
unzFile unzOpen(const char *path)
{
	unz_s us;
	unz_s *s;
	uLong central_pos,uL;
	FILE *fin;

	uLong number_disk;		  /* number of the current dist, used for
								   spaning ZIP, unsupported, always 0*/
	uLong number_disk_with_CD;  /* number the the disk with central dir, used
								   for spaning ZIP, unsupported, always 0*/
	uLong number_entry_CD;	  /* total number of entries in
								   the central dir
								   (same than number_entry on nospan) */

	int err = UNZ_OK;

	if (unz_copyright[0] != ' ')
		return NULL;

	fin = fopen(path, "rb");
	if (fin == NULL)
		return NULL;

	central_pos = unzstatic_SearchCentralDir(fin);
	if (central_pos == 0)
		err = UNZ_ERRNO;

	if (fseek(fin, central_pos, SEEK_SET) != 0)
		err = UNZ_ERRNO;

	/* the signature, already checked */
	if (unzstatic_getLong(fin, &uL) != UNZ_OK)
		err = UNZ_ERRNO;

	/* number of this disk */
	if (unzstatic_getShort(fin, &number_disk) != UNZ_OK)
		err = UNZ_ERRNO;

	/* number of the disk with the start of the central directory */
	if (unzstatic_getShort(fin, &number_disk_with_CD) != UNZ_OK)
		err = UNZ_ERRNO;

	/* total number of entries in the central dir on this disk */
	if (unzstatic_getShort(fin, &us.gi.number_entry) != UNZ_OK)
		err = UNZ_ERRNO;

	/* total number of entries in the central dir */
	if (unzstatic_getShort(fin, &number_entry_CD) != UNZ_OK)
		err = UNZ_ERRNO;

	if ((number_entry_CD != us.gi.number_entry) || (number_disk_with_CD != 0) || (number_disk != 0))
		err = UNZ_BADZIPFILE;

	/* size of the central directory */
	if (unzstatic_getLong(fin, &us.size_central_dir) != UNZ_OK)
		err = UNZ_ERRNO;

	/* offset of start of central directory with respect to the
		  starting disk number */
	if (unzstatic_getLong(fin, &us.offset_central_dir) != UNZ_OK)
		err = UNZ_ERRNO;

	/* zipfile comment length */
	if (unzstatic_getShort(fin, &us.gi.size_comment) != UNZ_OK)
		err = UNZ_ERRNO;

	if ((central_pos < us.offset_central_dir + us.size_central_dir) && (err == UNZ_OK))
		err = UNZ_BADZIPFILE;

	if (err != UNZ_OK)
	{
		fclose(fin);
		return NULL;
	}

	us.file = fin;
	us.byte_before_the_zipfile = central_pos - (us.offset_central_dir + us.size_central_dir);
	us.central_pos = central_pos;
	us.pzip_read_info = NULL;

	s = &unz;
	*s = us;
	unzGoToFirstFile((unzFile)s);

	return (unzFile)s;
}


/*
  Close a ZipFile opened with unzipOpen.
  If there is files inside the .Zip opened with unzipOpenCurrentFile (see later),
	these files MUST be closed with unzipCloseCurrentFile before call unzipClose.
  return UNZ_OK if there is no problem. */
int unzClose(unzFile file)
{
	unz_s *s;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;

	if (s->pzip_read_info != NULL)
		unzCloseCurrentFile(file);

	fclose(s->file);

	return UNZ_OK;
}


/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem. */
int unzGetGlobalInfo(unzFile file, unz_global_info *pglobal_info)
{
	unz_s *s;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	*pglobal_info = s->gi;
	return UNZ_OK;
}


/*
   Translate date/time from Dos format to tm_unz (readable more easilty)
*/
static void unzstatic_DosDateToTmuDate(uLong ulDosDate, tm_unz *ptm)
{
	uLong uDate;

	uDate = (uLong)(ulDosDate >> 16);
	ptm->tm_mday = (uInt)(uDate & 0x1f);
	ptm->tm_mon =  (uInt)((((uDate) & 0x1E0) / 0x20) - 1);
	ptm->tm_year = (uInt)(((uDate&0x0FE00)/0x0200)+1980);

	ptm->tm_hour = (uInt)((ulDosDate & 0xF800) / 0x800);
	ptm->tm_min =  (uInt)((ulDosDate & 0x7E0) / 0x20);
	ptm->tm_sec =  (uInt)(2 * (ulDosDate & 0x1f));
}

/*
  Get Info about the current file in the zipfile, with internal only info
*/
static int unzstatic_GetCurrentFileInfoInternal(
	unzFile file,
	unz_file_info *pfile_info,
	unz_file_info_internal *pfile_info_internal,
	char *szFileName,
	uLong fileNameBufferSize,
	void *extraField,
	uLong extraFieldBufferSize,
	char *szComment,
	uLong commentBufferSize
)
{
	unz_s *s;
	unz_file_info file_info;
	unz_file_info_internal file_info_internal;
	int err = UNZ_OK;
	uLong uMagic;
	long lSeek = 0;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	if (fseek(s->file, s->pos_in_central_dir + s->byte_before_the_zipfile, SEEK_SET) != 0)
		err = UNZ_ERRNO;

	/* we check the magic */
	if (err == UNZ_OK)
	{
		if (unzstatic_getLong(s->file, &uMagic) != UNZ_OK)
			err = UNZ_ERRNO;
		else if (uMagic != 0x02014b50)
			err = UNZ_BADZIPFILE;
	}

	if (unzstatic_getShort(s->file, &file_info.version) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &file_info.version_needed) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &file_info.flag) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &file_info.compression_method) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getLong(s->file, &file_info.dosDate) != UNZ_OK)
		err = UNZ_ERRNO;

	unzstatic_DosDateToTmuDate(file_info.dosDate, &file_info.tmu_date);

	if (unzstatic_getLong(s->file, &file_info.crc) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getLong(s->file, &file_info.compressed_size) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getLong(s->file, &file_info.uncompressed_size) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &file_info.size_filename) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &file_info.size_file_extra) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &file_info.size_file_comment) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &file_info.disk_num_start) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &file_info.internal_fa) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getLong(s->file, &file_info.external_fa) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getLong(s->file, &file_info_internal.offset_curfile) != UNZ_OK)
		err = UNZ_ERRNO;

	lSeek += file_info.size_filename;
	if ((err == UNZ_OK) && (szFileName != NULL))
	{
		uLong uSizeRead;

		if (file_info.size_filename < fileNameBufferSize)
		{
			*(szFileName + file_info.size_filename) = '\0';
			uSizeRead = file_info.size_filename;
		}
		else
			uSizeRead = fileNameBufferSize;

		if ((file_info.size_filename > 0) && (fileNameBufferSize > 0))
			if (fread(szFileName, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		lSeek -= uSizeRead;
	}

	if ((err == UNZ_OK) && (extraField != NULL))
	{
		uLong uSizeRead;
		if (file_info.size_file_extra<extraFieldBufferSize)
			uSizeRead = file_info.size_file_extra;
		else
			uSizeRead = extraFieldBufferSize;

		if (lSeek != 0)
		{
			if (fseek(s->file, lSeek, SEEK_CUR) == 0)
				lSeek = 0;
			else
				err = UNZ_ERRNO;
		}

		if ((file_info.size_file_extra > 0) && (extraFieldBufferSize > 0))
			if (fread(extraField, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		lSeek += file_info.size_file_extra - uSizeRead;
	}
	else
		lSeek += file_info.size_file_extra;


	if ((err == UNZ_OK) && (szComment != NULL))
	{
		uLong uSizeRead;

		if (file_info.size_file_comment < commentBufferSize)
		{
			*(szComment+file_info.size_file_comment) = '\0';
			uSizeRead = file_info.size_file_comment;
		}
		else
			uSizeRead = commentBufferSize;

		if (lSeek != 0)
		{
			if (fseek(s->file, lSeek, SEEK_CUR) == 0)
				lSeek = 0;
			else
				err = UNZ_ERRNO;
		}

		if ((file_info.size_file_comment > 0) && (commentBufferSize > 0))
			if (fread(szComment, (uInt)uSizeRead, 1, s->file) != 1)
				err = UNZ_ERRNO;
		lSeek += file_info.size_file_comment - uSizeRead;
	}
	else
		lSeek += file_info.size_file_comment;

	if ((err == UNZ_OK) && (pfile_info != NULL))
		*pfile_info = file_info;

	if ((err == UNZ_OK) && (pfile_info_internal != NULL))
		*pfile_info_internal = file_info_internal;

	return err;
}



/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem.
*/
int unzGetCurrentFileInfo(
	unzFile file,
	unz_file_info *pfile_info,
	char *szFileName,
	uLong fileNameBufferSize,
	void *extraField,
	uLong extraFieldBufferSize,
	char *szComment,
	uLong commentBufferSize
)
{
	return unzstatic_GetCurrentFileInfoInternal(file, pfile_info, NULL,
												szFileName, fileNameBufferSize,
												extraField, extraFieldBufferSize,
												szComment, commentBufferSize);
}

/*
  Set the current file of the zipfile to the first file.
  return UNZ_OK if there is no problem
*/
int unzGoToFirstFile(unzFile file)
{
	int err = UNZ_OK;
	unz_s *s;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	s->pos_in_central_dir = s->offset_central_dir;
	s->num_file = 0;
	err = unzstatic_GetCurrentFileInfoInternal(file, &s->cur_file_info,
											 &s->cur_file_info_internal,
											 NULL, 0, NULL, 0, NULL, 0);
	s->current_file_ok = (err == UNZ_OK);

	return err;
}


/*
  Set the current file of the zipfile to the next file.
  return UNZ_OK if there is no problem
  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
*/
int unzGoToNextFile(unzFile file)
{
	unz_s *s;
	int err;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	if (!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;
	if (s->num_file + 1 == s->gi.number_entry)
		return UNZ_END_OF_LIST_OF_FILE;

	s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename +
			s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment;
	s->num_file++;
	err = unzstatic_GetCurrentFileInfoInternal(file, &s->cur_file_info,
											   &s->cur_file_info_internal,
											   NULL, 0, NULL, 0, NULL, 0);
	s->current_file_ok = (err == UNZ_OK);

	return err;
}


/*
  Try locate the file szFileName in the zipfile.
  For the iCaseSensitivity signification, see unzipStringFileNameCompare

  return value :
  UNZ_OK if the file is found. It becomes the current file.
  UNZ_END_OF_LIST_OF_FILE if the file is not found
*/
int unzLocateFile(unzFile file, const char *szFileName, int iCaseSensitivity)
{
	unz_s* s;
	int err;
	uLong num_fileSaved;
	uLong pos_in_central_dirSaved;

	if (file == NULL)
		return UNZ_PARAMERROR;

	if (strlen(szFileName) >= UNZ_MAXFILENAMEINZIP)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	if (!s->current_file_ok)
		return UNZ_END_OF_LIST_OF_FILE;

	num_fileSaved = s->num_file;
	pos_in_central_dirSaved = s->pos_in_central_dir;

	err = unzGoToFirstFile(file);

	while (err == UNZ_OK)
	{
		char szCurrentFileName[UNZ_MAXFILENAMEINZIP + 1];

		unzGetCurrentFileInfo(file, NULL,
								szCurrentFileName, sizeof(szCurrentFileName) - 1,
								NULL, 0, NULL, 0);

		if (unzStringFileNameCompare(szCurrentFileName, szFileName, iCaseSensitivity) == 0)
			return UNZ_OK;

		err = unzGoToNextFile(file);
	}

	s->num_file = num_fileSaved;
	s->pos_in_central_dir = pos_in_central_dirSaved;
	return err;
}


/*
  Read the static header of the current zipfile
  Check the coherency of the static header and info in the end of central
		directory about this file
  store in *piSizeVar the size of extra info in static header
		(filename and size of extra field data)
*/
static int unzstatic_CheckCurrentFileCoherencyHeader(
	unz_s *s,
	uInt *piSizeVar,
	uLong *poffset_static_extrafield,
	uInt  *psize_static_extrafield
)
{
	uLong uMagic, uData,uFlags;
	uLong size_filename;
	uLong size_extra_field;
	int err = UNZ_OK;

	*piSizeVar = 0;
	*poffset_static_extrafield = 0;
	*psize_static_extrafield = 0;

	if (fseek(s->file, s->cur_file_info_internal.offset_curfile + s->byte_before_the_zipfile, SEEK_SET) != 0)
		return UNZ_ERRNO;

	if (err == UNZ_OK)
	{
		if (unzstatic_getLong(s->file, &uMagic) != UNZ_OK)
			err = UNZ_ERRNO;
		else if (uMagic != 0x04034b50)
			err = UNZ_BADZIPFILE;
	}

	if (unzstatic_getShort(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
/*
	else if ((err ==UNZ_OK) && (uData!=s->cur_file_info.wVersion))
		err = UNZ_BADZIPFILE;
*/
	if (unzstatic_getShort(s->file, &uFlags) != UNZ_OK)
		err = UNZ_ERRNO;

	if (unzstatic_getShort(s->file, &uData) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err ==UNZ_OK) && (uData != s->cur_file_info.compression_method))
		err = UNZ_BADZIPFILE;

	if ((err == UNZ_OK) && (s->cur_file_info.compression_method != 0) &&
						 (s->cur_file_info.compression_method != Z_DEFLATED))
		err = UNZ_BADZIPFILE;

	if (unzstatic_getLong(s->file, &uData) != UNZ_OK) /* date/time */
		err = UNZ_ERRNO;

	if (unzstatic_getLong(s->file, &uData) != UNZ_OK) /* crc */
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.crc) && ((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unzstatic_getLong(s->file,&uData) != UNZ_OK) /* size compr */
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.compressed_size) && ((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unzstatic_getLong(s->file,&uData) != UNZ_OK) /* size uncompr */
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (uData != s->cur_file_info.uncompressed_size) && ((uFlags & 8) == 0))
		err = UNZ_BADZIPFILE;

	if (unzstatic_getShort(s->file, &size_filename) != UNZ_OK)
		err = UNZ_ERRNO;
	else if ((err == UNZ_OK) && (size_filename != s->cur_file_info.size_filename))
		err = UNZ_BADZIPFILE;

	*piSizeVar += (uInt)size_filename;

	if (unzstatic_getShort(s->file, &size_extra_field) != UNZ_OK)
		err = UNZ_ERRNO;
	*poffset_static_extrafield = s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER + size_filename;
	*psize_static_extrafield = (uInt)size_extra_field;

	*piSizeVar += (uInt)size_extra_field;

	return err;
}

/*
  Open for reading data the current file in the zipfile.
  If there is no error and the file is opened, the return value is UNZ_OK.
*/
int unzOpenCurrentFile(unzFile file)
{
	int err = UNZ_OK;
	int Store;
	uInt iSizeVar;
	unz_s* s;
	zip_read_info_s *pzip_read_info;
	uLong offset_static_extrafield;  /* offset of the static extra field */
	uInt size_static_extrafield;	/* size of the static extra field */

	if (!file) return UNZ_PARAMERROR;

	s = (unz_s *)file;
	if (!s->current_file_ok) return UNZ_PARAMERROR;

	if (s->pzip_read_info != NULL)
		unzCloseCurrentFile(file);

	if (unzstatic_CheckCurrentFileCoherencyHeader(s,&iSizeVar,
				&offset_static_extrafield,&size_static_extrafield) != UNZ_OK)
		return UNZ_BADZIPFILE;

	pzip_read_info = &zip_read_info;
	pzip_read_info->offset_static_extrafield = offset_static_extrafield;
	pzip_read_info->size_static_extrafield = size_static_extrafield;
	pzip_read_info->pos_static_extrafield = 0;
	pzip_read_info->stream_initialised = 0;

	if ((s->cur_file_info.compression_method != 0)
	&&	(s->cur_file_info.compression_method != Z_DEFLATED))
		err = UNZ_BADZIPFILE;

	Store = s->cur_file_info.compression_method == 0;

	pzip_read_info->crc32_wait = s->cur_file_info.crc;
	pzip_read_info->crc32 = 0;
	pzip_read_info->compression_method = s->cur_file_info.compression_method;
	pzip_read_info->file = s->file;
	pzip_read_info->byte_before_the_zipfile = s->byte_before_the_zipfile;

	pzip_read_info->stream.total_out = 0;

	if (!Store)
	{
		pzip_read_info->stream.zalloc = (alloc_func)0;
		pzip_read_info->stream.zfree = (free_func)0;
		pzip_read_info->stream.opaque = (voidpf)0;

		err = inflateInit2(&pzip_read_info->stream, -MAX_WBITS);
		if (err == Z_OK)
			pzip_read_info->stream_initialised = 1;
		/* windowBits is passed < 0 to tell that there is no zlib header.
		 * Note that in this case inflate *requires* an extra "dummy" byte
		 * after the compressed stream in order to complete decompression and
		 * return Z_STREAM_END.
		 * In unzip, i don't wait absolutely Z_STREAM_END because I known the
		 * size of both compressed and uncompressed data
		 */
	}
	pzip_read_info->rest_read_compressed = s->cur_file_info.compressed_size;
	pzip_read_info->rest_read_uncompressed = s->cur_file_info.uncompressed_size;

	pzip_read_info->pos_in_zipfile =
			s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER + iSizeVar;

	pzip_read_info->stream.avail_in = (uInt)0;

	s->pzip_read_info = pzip_read_info;

	return UNZ_OK;
}


/*
  Read bytes from the current file.
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if somes bytes are copied
  return 0 if the end of file was reached
  return <0 with error code if there is an error
	(UNZ_ERRNO for IO error, or zLib error for uncompress error)
*/
int unzReadCurrentFile(unzFile file, void *buf, unsigned len)
{
	int err = UNZ_OK;
	uInt iRead = 0;
	unz_s *s;
	zip_read_info_s *pzip_read_info;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	pzip_read_info = s->pzip_read_info;

	if (pzip_read_info == NULL)
		return UNZ_PARAMERROR;

	if (len == 0)
		return 0;

	pzip_read_info->stream.next_out = (Bytef *)buf;

	pzip_read_info->stream.avail_out = (uInt)len;

	if (len>pzip_read_info->rest_read_uncompressed)
		pzip_read_info->stream.avail_out = (uInt)pzip_read_info->rest_read_uncompressed;

	while (pzip_read_info->stream.avail_out > 0)
	{
		if ((pzip_read_info->stream.avail_in == 0) &&
			(pzip_read_info->rest_read_compressed > 0))
		{
			uInt uReadThis = UNZ_BUFSIZE;

			if (pzip_read_info->rest_read_compressed<uReadThis)
				uReadThis = (uInt)pzip_read_info->rest_read_compressed;

			if (uReadThis == 0)
				return UNZ_EOF;

			if (fseek(pzip_read_info->file,
					pzip_read_info->pos_in_zipfile +
					pzip_read_info->byte_before_the_zipfile, SEEK_SET) != 0)
				return UNZ_ERRNO;

			if (fread(pzip_read_info->read_buffer, uReadThis, 1, pzip_read_info->file) != 1)
				return UNZ_ERRNO;

			pzip_read_info->pos_in_zipfile += uReadThis;

			pzip_read_info->rest_read_compressed -= uReadThis;

			pzip_read_info->stream.next_in = (Bytef *)pzip_read_info->read_buffer;
			pzip_read_info->stream.avail_in = (uInt)uReadThis;
		}

		if (pzip_read_info->compression_method == 0)
		{
			uInt uDoCopy,i;

			if (pzip_read_info->stream.avail_out < pzip_read_info->stream.avail_in)
				uDoCopy = pzip_read_info->stream.avail_out;
			else
				uDoCopy = pzip_read_info->stream.avail_in;

			for (i = 0; i < uDoCopy; i++)
				*(pzip_read_info->stream.next_out + i) = *(pzip_read_info->stream.next_in + i);

			pzip_read_info->crc32 = crc32(pzip_read_info->crc32,
								pzip_read_info->stream.next_out,
								uDoCopy);
			pzip_read_info->rest_read_uncompressed -= uDoCopy;
			pzip_read_info->stream.avail_in -= uDoCopy;
			pzip_read_info->stream.avail_out -= uDoCopy;
			pzip_read_info->stream.next_out += uDoCopy;
			pzip_read_info->stream.next_in += uDoCopy;
			pzip_read_info->stream.total_out += uDoCopy;
			iRead += uDoCopy;
		}
		else
		{
			uLong uTotalOutBefore, uTotalOutAfter;
			const Bytef *bufBefore;
			uLong uOutThis;
			int flush = Z_SYNC_FLUSH;

			uTotalOutBefore = pzip_read_info->stream.total_out;
			bufBefore = pzip_read_info->stream.next_out;

			/*
			if ((pzip_read_info->rest_read_uncompressed ==
					 pzip_read_info->stream.avail_out) &&
				(pzip_read_info->rest_read_compressed == 0))
				flush = Z_FINISH;
			*/
			err = inflate(&pzip_read_info->stream, flush);

			uTotalOutAfter = pzip_read_info->stream.total_out;
			uOutThis = uTotalOutAfter-uTotalOutBefore;

			pzip_read_info->crc32 = crc32(pzip_read_info->crc32, bufBefore, (uInt)(uOutThis));

			pzip_read_info->rest_read_uncompressed -= uOutThis;

			iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

			if (err == Z_STREAM_END)
				return (iRead == 0) ? UNZ_EOF : iRead;

			if (err != Z_OK)
				break;
		}
	}

	if (err == Z_OK)
		return iRead;
	return err;
}


#if 0
/*
  Give the current position in uncompressed data
*/
z_off_t unztell(unzFile file)
{
	unz_s *s;
	zip_read_info_s *pzip_read_info;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	pzip_read_info = s->pzip_read_info;

	if (pzip_read_info == NULL)
		return UNZ_PARAMERROR;

	return (z_off_t)pzip_read_info->stream.total_out;
}


/*
  return 1 if the end of file was reached, 0 elsewhere
*/
int unzeof(unzFile file)
{
	unz_s *s;
	zip_read_info_s *pzip_read_info;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	pzip_read_info = s->pzip_read_info;

	if (pzip_read_info == NULL)
		return UNZ_PARAMERROR;

	if (pzip_read_info->rest_read_uncompressed == 0)
		return 1;
	else
		return 0;
}


/*
  Read extra field from the current file (opened by unzOpenCurrentFile)
  This is the static-header version of the extra field (sometimes, there is
	more info in the static-header version than in the central-header)

  if buf==NULL, it return the size of the static extra field that can be read

  if buf!=NULL, len is the size of the buffer, the extra header is copied in
	buf.
  the return value is the number of bytes copied in buf, or (if <0)
	the error code
*/
int unzGetLocalExtrafield(unzFile file, void *buf, unsigned len)
{
	unz_s *s;
	zip_read_info_s *pzip_read_info;
	uInt read_now;
	uLong size_to_read;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	pzip_read_info = s->pzip_read_info;

	if (pzip_read_info == NULL)
		return UNZ_PARAMERROR;

	size_to_read = (pzip_read_info->size_static_extrafield - pzip_read_info->pos_static_extrafield);

	if (buf == NULL)
		return (int)size_to_read;

	if (len > size_to_read)
		read_now = (uInt)size_to_read;
	else
		read_now = (uInt)len;

	if (read_now == 0)
		return 0;

	if (fseek(pzip_read_info->file,
			  pzip_read_info->offset_static_extrafield +
			  pzip_read_info->pos_static_extrafield, SEEK_SET) != 0)
		return UNZ_ERRNO;

	if (fread(buf, (uInt)size_to_read, 1, pzip_read_info->file) != 1)
		return UNZ_ERRNO;

	return (int)read_now;
}
#endif

/*
  Close the file in zip opened with unzipOpenCurrentFile
  Return UNZ_CRCERROR if all the file was read but the CRC is not good
*/
int unzCloseCurrentFile(unzFile file)
{
	int err = UNZ_OK;
	unz_s *s;
	zip_read_info_s *pzip_read_info;

	if (file == NULL)
		return UNZ_PARAMERROR;

	s = (unz_s *)file;
	pzip_read_info = s->pzip_read_info;

	if (pzip_read_info == NULL)
		return UNZ_PARAMERROR;

	if (pzip_read_info->rest_read_uncompressed == 0)
		if (pzip_read_info->crc32 != pzip_read_info->crc32_wait)
			err = UNZ_CRCERROR;

	if (pzip_read_info->stream_initialised)
		inflateEnd(&pzip_read_info->stream);

	pzip_read_info->stream_initialised = 0;

	s->pzip_read_info = NULL;

	return err;
}


#if 0
/*
  Get the global comment string of the ZipFile, in the szComment buffer.
  uSizeBuf is the size of the szComment buffer.
  return the number of byte copied or an error code <0
*/
int unzGetGlobalComment(unzFile file, char *szComment, uLong uSizeBuf)
{
	unz_s *s;
	uLong uReadThis;

	if (file == NULL)
		return UNZ_PARAMERROR;
	s = (unz_s *)file;

	uReadThis = uSizeBuf;
	if (uReadThis > s->gi.size_comment)
		uReadThis = s->gi.size_comment;

	if (fseek(s->file, s->central_pos + 22, SEEK_SET) != 0)
		return UNZ_ERRNO;

	if (uReadThis > 0)
	{
		*szComment = '\0';
		if (fread(szComment, (uInt)uReadThis, 1, s->file) != 1)
			return UNZ_ERRNO;
	}

	if ((szComment != NULL) && (uSizeBuf > s->gi.size_comment))
		*(szComment + s->gi.size_comment) = '\0';

	return (int)uReadThis;
}
#endif

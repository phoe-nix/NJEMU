/******************************************************************************

	osd_cpu.h

	Compatible for MAME.

******************************************************************************/

#ifndef OSD_CPU_H
#define OSD_CPU_H

#ifdef PSP

typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
typedef SceULong64		UINT64;

typedef signed char		INT8;
typedef signed short	INT16;
typedef signed int		INT32;
typedef SceLong64		INT64;

#define offs_t		UINT32
#define data8_t		UINT8
#define data16_t	UINT16
#define data32_t	UINT32

#else

typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned __int64    UINT64;
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed int          INT32;
typedef signed __int64      INT64;

#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/* Useful macros to deal with bit shuffling encryptions */
#define BIT(x,n) (((x)>>(n))&1)

#define BITSWAP8(val,B7,B6,B5,B4,B3,B2,B1,B0) \
		((BIT(val,B7) << 7) | \
		 (BIT(val,B6) << 6) | \
		 (BIT(val,B5) << 5) | \
		 (BIT(val,B4) << 4) | \
		 (BIT(val,B3) << 3) | \
		 (BIT(val,B2) << 2) | \
		 (BIT(val,B1) << 1) | \
		 (BIT(val,B0) << 0))

#define BITSWAP16(val,B15,B14,B13,B12,B11,B10,B9,B8,B7,B6,B5,B4,B3,B2,B1,B0) \
		((BIT(val,B15) << 15) | \
		 (BIT(val,B14) << 14) | \
		 (BIT(val,B13) << 13) | \
		 (BIT(val,B12) << 12) | \
		 (BIT(val,B11) << 11) | \
		 (BIT(val,B10) << 10) | \
		 (BIT(val, B9) <<  9) | \
		 (BIT(val, B8) <<  8) | \
		 (BIT(val, B7) <<  7) | \
		 (BIT(val, B6) <<  6) | \
		 (BIT(val, B5) <<  5) | \
		 (BIT(val, B4) <<  4) | \
		 (BIT(val, B3) <<  3) | \
		 (BIT(val, B2) <<  2) | \
		 (BIT(val, B1) <<  1) | \
		 (BIT(val, B0) <<  0))

#define BITSWAP24(val,B23,B22,B21,B20,B19,B18,B17,B16,B15,B14,B13,B12,B11,B10,B9,B8,B7,B6,B5,B4,B3,B2,B1,B0) \
		((BIT(val,B23) << 23) | \
		 (BIT(val,B22) << 22) | \
		 (BIT(val,B21) << 21) | \
		 (BIT(val,B20) << 20) | \
		 (BIT(val,B19) << 19) | \
		 (BIT(val,B18) << 18) | \
		 (BIT(val,B17) << 17) | \
		 (BIT(val,B16) << 16) | \
		 (BIT(val,B15) << 15) | \
		 (BIT(val,B14) << 14) | \
		 (BIT(val,B13) << 13) | \
		 (BIT(val,B12) << 12) | \
		 (BIT(val,B11) << 11) | \
		 (BIT(val,B10) << 10) | \
		 (BIT(val, B9) <<  9) | \
		 (BIT(val, B8) <<  8) | \
		 (BIT(val, B7) <<  7) | \
		 (BIT(val, B6) <<  6) | \
		 (BIT(val, B5) <<  5) | \
		 (BIT(val, B4) <<  4) | \
		 (BIT(val, B3) <<  3) | \
		 (BIT(val, B2) <<  2) | \
		 (BIT(val, B1) <<  1) | \
		 (BIT(val, B0) <<  0))

#define BITSWAP32(val,B31,B30,B29,B28,B27,B26,B25,B24,B23,B22,B21,B20,B19,B18,B17,B16,B15,B14,B13,B12,B11,B10,B9,B8,B7,B6,B5,B4,B3,B2,B1,B0) \
		((BIT(val,B31) << 31) | \
		 (BIT(val,B30) << 30) | \
		 (BIT(val,B29) << 29) | \
		 (BIT(val,B28) << 28) | \
		 (BIT(val,B27) << 27) | \
		 (BIT(val,B26) << 26) | \
		 (BIT(val,B25) << 25) | \
		 (BIT(val,B24) << 24) | \
		 (BIT(val,B23) << 23) | \
		 (BIT(val,B22) << 22) | \
		 (BIT(val,B21) << 21) | \
		 (BIT(val,B20) << 20) | \
		 (BIT(val,B19) << 19) | \
		 (BIT(val,B18) << 18) | \
		 (BIT(val,B17) << 17) | \
		 (BIT(val,B16) << 16) | \
		 (BIT(val,B15) << 15) | \
		 (BIT(val,B14) << 14) | \
		 (BIT(val,B13) << 13) | \
		 (BIT(val,B12) << 12) | \
		 (BIT(val,B11) << 11) | \
		 (BIT(val,B10) << 10) | \
		 (BIT(val, B9) <<  9) | \
		 (BIT(val, B8) <<  8) | \
		 (BIT(val, B7) <<  7) | \
		 (BIT(val, B6) <<  6) | \
		 (BIT(val, B5) <<  5) | \
		 (BIT(val, B4) <<  4) | \
		 (BIT(val, B3) <<  3) | \
		 (BIT(val, B2) <<  2) | \
		 (BIT(val, B1) <<  1) | \
		 (BIT(val, B0) <<  0))

#endif /* OSD_CPU_H */

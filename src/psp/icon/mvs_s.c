/******************************************************************************

	icon/mvs_s.c

	MVSPSP用アイコンデータ(小) (24x18 pixels)

******************************************************************************/

#include "psp/psp.h"

#define NUM_FONTS	0x11


/******************************************************************************
	ローカル変数
******************************************************************************/

/*------------------------------------------------------
	gryph data
------------------------------------------------------*/

static const uint8_t ALIGN_DATA icon_s[] = {
	0x00,0x00,0x21,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x82,0xdc,0x5b,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xed,0xee,0x07,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x10,0x53,0xeb,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xe5,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0xe3,
	0xbe,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x71,0x58,0x33,0xeb,0xee,0x6d,0x00,0x00,
	0x00,0x00,0x00,0x00,0x40,0xee,0xdd,0xee,0xee,0xfe,0x4c,0x00,0x00,0x00,0x00,0x00,
	0x00,0xc5,0xfe,0xce,0xfe,0xee,0xef,0x3a,0x30,0x55,0x01,0x00,0x00,0x10,0x65,0x04,
	0xb3,0xff,0xee,0xef,0xdb,0xee,0x5c,0x00,0x00,0x00,0x00,0x00,0x00,0xc5,0xef,0xee,
	0xee,0xdd,0xee,0x05,0x00,0x00,0x00,0x00,0x00,0x10,0xd7,0xee,0xce,0x34,0x96,0x18,
	0x00,0x00,0x00,0x00,0x00,0x00,0x10,0xea,0x4e,0x00,0x00,0x10,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xf6,0x5e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd3,
	0xbe,0x35,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0xee,0xde,0x8d,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xb6,0xcd,0x3a,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x10,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x82,0x04,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x71,0x3d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x87,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x98,0x00,0x00,0x00,0x00,0x00,0x00,
	0x61,0x57,0x24,0x00,0x54,0x10,0x43,0x86,0x17,0x00,0x10,0xfb,0xff,0xde,0xdd,0xcc,
	0xdd,0xee,0xce,0xae,0x00,0x60,0xce,0x67,0xfc,0xee,0xff,0xee,0xdc,0x08,0xe8,0x04,
	0xb0,0x4e,0x64,0xc2,0xee,0xab,0xbf,0x40,0x18,0xf8,0x0a,0xe3,0x2b,0xfe,0x84,0xef,
	0xc9,0xcf,0x51,0xce,0x76,0x2d,0xe6,0x1c,0xca,0x92,0xef,0xde,0xee,0xad,0xb7,0x00,
	0x5d,0xe7,0x8e,0x11,0xe5,0xee,0x98,0xee,0x4e,0x80,0x88,0x6e,0xe7,0xed,0xbc,0xee,
	0xee,0xed,0xee,0xae,0xc6,0xde,0x5d,0xe5,0xdd,0xee,0xee,0xee,0xee,0xee,0xee,0xde,
	0xdd,0x3d,0x80,0xdd,0xdd,0xcc,0xbb,0xbb,0xbb,0xcb,0xdd,0xcd,0x06,0x00,0x42,0x24,
	0x01,0x00,0x00,0x00,0x00,0x32,0x13,0x00,0x00,0x53,0x55,0x25,0x00,0x00,0x00,0x00,
	0x00,0x00,0x30,0xbc,0xbb,0xcb,0x01,0x00,0x00,0x00,0x00,0x00,0xb2,0x04,0x00,0x70,
	0x1a,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0xb9,0xaa,0xaa,0xaa,0x4b,0x00,
	0x58,0x00,0x00,0x00,0x30,0x33,0x22,0x22,0x79,0x00,0x58,0x90,0xab,0xba,0xaa,0xaa,
	0xaa,0xaa,0xcc,0x4a,0x48,0xe2,0xee,0xfe,0xff,0xff,0xee,0xee,0xdd,0x3e,0x48,0xe5,
	0xee,0xee,0xee,0xee,0xee,0xde,0xdd,0x1c,0x58,0xe8,0xee,0xee,0xee,0xee,0xee,0xde,
	0xed,0x0a,0x68,0xeb,0xee,0xee,0xee,0xee,0xee,0xde,0xed,0x07,0x88,0xed,0xee,0xee,
	0xee,0xee,0xee,0xde,0xdd,0x04,0xc8,0xdd,0xed,0xee,0xee,0xee,0xee,0xdd,0xcd,0x01,
	0xe8,0xdd,0xdd,0xee,0xee,0xee,0xde,0xdd,0xae,0x00,0xe8,0xed,0xee,0xee,0xee,0xee,
	0xee,0xee,0x8e,0x00,0xb6,0xbb,0xbb,0xbb,0xbb,0xbb,0xbb,0xbb,0x4b,0x00,0x00,0x00,
	0x20,0x97,0xbb,0x7a,0x03,0x00,0x00,0x00,0x10,0xd7,0xee,0xcd,0xed,0x9e,0x01,0x00,
	0x00,0xb2,0xbf,0x4c,0x52,0xcd,0xc8,0x3c,0x00,0x10,0xeb,0x6e,0x0c,0xb6,0x6d,0x00,
	0xd7,0x02,0x70,0xdb,0x3d,0x0c,0x42,0x08,0x70,0xc2,0x0a,0xd2,0x22,0x17,0x0c,0xd5,
	0x2c,0x24,0x60,0x3e,0xe7,0x22,0x10,0x2c,0xbd,0x7e,0x00,0x31,0x9d,0xd9,0xd4,0x16,
	0x0c,0x61,0xce,0x40,0x34,0xbe,0xbb,0xe8,0xad,0x4c,0xdb,0xff,0x07,0x80,0xce,0xcb,
	0x59,0x64,0xfc,0x3c,0xd8,0x9d,0xc7,0xcd,0xea,0x67,0x7c,0xe9,0x55,0xa6,0x6c,0x35,
	0xca,0xe7,0xb5,0xce,0x8d,0x52,0xce,0xa5,0x24,0x9a,0xd3,0x96,0x67,0x4b,0x77,0x6a,
	0x42,0x56,0x59,0x90,0x58,0x6c,0x4a,0xfa,0x59,0x24,0x54,0x08,0x10,0x6b,0x55,0xba,
	0xd5,0x98,0x7c,0xa3,0x04,0x00,0xc3,0x49,0xf8,0xcb,0x28,0x76,0x59,0x00,0x00,0x20,
	0xda,0xed,0xee,0x9d,0x78,0x03,0x00,0x00,0x00,0x40,0xb9,0xdc,0x9b,0x05,0x00,0x00,
	0x00,0x86,0x88,0x88,0x88,0x88,0x88,0x88,0x36,0x00,0x00,0x00,0xeb,0xee,0xee,0xee,
	0xee,0xee,0xee,0xde,0x18,0x00,0x00,0xda,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xed,0xae,
	0x00,0x00,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0xb4,0xed,0x06,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x20,0xec,0x0b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xe9,0x2d,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0xe8,0x2d,0x00,0x00,0x89,
	0x00,0x00,0x00,0x00,0x00,0x10,0xec,0x0c,0x00,0xa2,0x8f,0x00,0x00,0x00,0x00,0x00,
	0xa2,0xee,0x07,0x40,0xec,0xde,0xbb,0xbb,0xbb,0xbb,0xbb,0xed,0xbe,0x01,0xb1,0xde,
	0xed,0xff,0xff,0xff,0xff,0xff,0xee,0x2a,0x00,0x10,0xea,0xce,0xaa,0xaa,0xaa,0xaa,
	0xaa,0x48,0x00,0x00,0x00,0x80,0x8f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x87,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x44,0x44,0x55,0x55,0x45,0x44,0x44,0x03,0x00,
	0x10,0xda,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xad,0x01,0x90,0xee,0x06,0x00,0x00,
	0x00,0x00,0x00,0x40,0xed,0x09,0xd3,0xed,0x05,0x00,0x00,0x00,0x00,0x00,0x30,0xdd,
	0x4d,0xe7,0xed,0x06,0x00,0x00,0x00,0x00,0x00,0x30,0xdd,0x7e,0xe9,0xed,0x06,0x00,
	0x00,0x00,0x00,0x00,0x30,0xdd,0x9d,0xe9,0xed,0x06,0x00,0x00,0x00,0x00,0x00,0x30,
	0xdd,0xad,0xe8,0xed,0x06,0x00,0x00,0x00,0x00,0x00,0x30,0xdd,0x9e,0xe6,0xed,0x06,
	0x00,0x00,0x00,0x00,0x00,0x30,0xdd,0x6e,0xc2,0xed,0x05,0x00,0x00,0x00,0x00,0x00,
	0x30,0xdd,0x2d,0x60,0xee,0x48,0x44,0x44,0x44,0x44,0x44,0x74,0xee,0x06,0x00,0xb5,
	0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0x6b,0x00,0xcb,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,
	0xcc,0xcc,0xac,0xdd,0xcc,0xed,0xdd,0xed,0xde,0xdc,0xde,0xcc,0xcd,0x8d,0x66,0xd7,
	0x13,0xb1,0x6b,0x67,0x7d,0x67,0xc9,0x8d,0xdb,0xd8,0x02,0xb0,0xba,0x8e,0x8c,0xbd,
	0xc9,0x8d,0xcb,0xd8,0x02,0xb0,0xab,0x8d,0x8c,0xac,0xc9,0x8d,0x00,0xe5,0x02,0xb0,
	0x0a,0x30,0x5e,0x00,0xc8,0x8d,0x00,0xe5,0x02,0xb0,0x0b,0x30,0x4e,0x00,0xc8,0x8d,
	0x00,0xe5,0x02,0xb0,0x0b,0x30,0x4e,0x00,0xc8,0x8d,0x00,0xe5,0x02,0xb0,0x0a,0x30,
	0x4e,0x00,0xc8,0x8d,0x00,0xd5,0xb7,0xb8,0x0a,0x30,0x4e,0x00,0xc8,0x7d,0x00,0xd5,
	0xf9,0xbb,0x0a,0x30,0x4e,0x00,0xc8,0x8d,0x00,0xd6,0x96,0xb6,0x0a,0x30,0x5e,0x00,
	0xc9,0xdd,0xcc,0xed,0xbc,0xdc,0xcd,0xcc,0xcd,0xcc,0xcd,0xdc,0xdd,0xdd,0xdd,0xdd,
	0xdd,0xdd,0xdd,0xdd,0xbd,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x00,
	0x51,0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0xac,0xca,0x05,0x00,0x75,0x66,
	0x26,0x00,0x00,0xb1,0x66,0x57,0x2d,0x00,0xfc,0xee,0x4e,0x00,0x00,0xd3,0xc9,0x66,
	0x5e,0x42,0xec,0xee,0x7e,0x14,0x00,0xd2,0xae,0xe8,0x3e,0xd3,0xee,0xee,0xee,0x08,
	0x00,0x80,0xaf,0xf9,0x0b,0x40,0xed,0xee,0x8e,0x00,0x00,0x00,0xb7,0x9c,0x01,0x00,
	0xd4,0xfe,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x8d,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x20,0x54,0x69,0x34,0x01,0x00,0x00,0x00,0x00,0x50,0xeb,0xee,0xee,
	0xde,0x9c,0x02,0x00,0x00,0x00,0xf4,0xef,0xee,0xee,0xee,0xee,0x0b,0x00,0x00,0x00,
	0xb2,0xfe,0xef,0xee,0xee,0xde,0x07,0x00,0x00,0x30,0x06,0x73,0xb9,0xbc,0x8a,0x15,
	0x70,0x00,0x00,0x30,0x6c,0x01,0x00,0x00,0x00,0x30,0x99,0x00,0x00,0x00,0xa4,0xbc,
	0x79,0x77,0xa8,0xbb,0x18,0x00,0x00,0x00,0x00,0x31,0x65,0x67,0x45,0x02,0x00,0x00,
	0x90,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe6,0x3d,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x40,0xdd,0xce,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xd3,0xde,0xfe,
	0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0xa8,0xec,0xad,0xab,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xf7,0x1d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf7,0x1d,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xf7,0x1d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xf7,0x2d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe5,0x6e,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xd2,0xde,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xee,
	0xce,0xbb,0xbb,0x1a,0xa7,0x72,0x32,0x00,0x00,0xe8,0xee,0xee,0xee,0x2e,0xd9,0xa3,
	0x53,0x00,0x00,0x40,0xca,0xdd,0xdd,0x2c,0xc8,0x92,0x43,0x73,0x76,0x77,0x77,0x77,
	0x77,0x77,0x67,0x66,0x06,0xf8,0xee,0xee,0xff,0xff,0xff,0xee,0xee,0xee,0x1d,0xe7,
	0xdd,0xed,0xee,0xee,0xee,0xde,0xdd,0xdd,0x1c,0xe7,0xdd,0xee,0xee,0xee,0xee,0xee,
	0xdd,0xdd,0x1c,0xe7,0xdd,0xee,0xee,0xee,0xee,0xee,0xde,0xdd,0x1c,0xe7,0xed,0xee,
	0xee,0xee,0xee,0xee,0xde,0xdd,0x1c,0xe7,0xed,0xee,0xee,0xee,0xee,0xee,0xde,0xdd,
	0x1c,0xe7,0x8d,0xed,0xee,0xee,0xee,0xee,0xde,0xdd,0x1c,0xf8,0x0a,0xec,0xee,0xee,
	0xee,0xee,0xde,0xdd,0x1c,0xe4,0x8d,0xec,0xee,0xee,0xee,0xee,0xdd,0xdd,0x1c,0x50,
	0xee,0xee,0xee,0xee,0xee,0xee,0xdd,0xdd,0x1d,0x00,0xd5,0xdd,0xed,0xee,0xee,0xdd,
	0xdd,0xdd,0x1c,0x00,0x20,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x03,0x70,0xbc,0xcc,
	0xcc,0xcc,0xcc,0xac,0x66,0x56,0x00,0xb4,0xaa,0xaa,0xaa,0xaa,0xaa,0xdb,0xde,0xde,
	0x00,0x00,0x20,0x22,0x22,0x22,0x22,0x20,0x95,0x48,0x00,0x00,0xe9,0xee,0xee,0xee,
	0xee,0x04,0xd1,0x0a,0x00,0x00,0xa5,0xaa,0xaa,0xaa,0x9a,0x01,0xe2,0x0a,0x00,0x00,
	0xd9,0xdd,0xdd,0xdd,0xdd,0x04,0xe2,0x0a,0x00,0x00,0xa5,0xaa,0xaa,0xaa,0x9a,0x01,
	0xe2,0x0a,0x00,0x00,0xe9,0xdd,0xee,0xee,0xee,0x04,0xe2,0x0a,0x00,0x00,0x52,0x55,
	0x55,0x55,0x55,0x10,0x83,0x27,0x00,0x95,0x89,0x88,0x99,0x99,0x99,0xd9,0xde,0xcd,
	0x00,0xa1,0xde,0xed,0xee,0xee,0xee,0xce,0x98,0x69,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xb0,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x86,0x58,0x04,0x00,
	0x00,0x00,0x00,0x00,0x00,0xe1,0xde,0xed,0x0b,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
	0x44,0x44,0x03,0x00,0x76,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x16,0x10,0xed,
	0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xed,0x2d,0x10,0x9d,0x01,0x00,0x11,0x00,0x10,
	0x01,0x00,0x71,0x2d,0x30,0x8d,0x60,0x76,0x02,0x66,0x06,0x72,0x56,0x60,0x2d,0xd6,
	0x8e,0xe0,0xff,0x25,0xff,0x2f,0xf6,0xdf,0x60,0x2d,0xe7,0x8e,0xd0,0xee,0x25,0xee,
	0x2e,0xe5,0xce,0x60,0x2d,0xe6,0x8e,0xd0,0xee,0x25,0xee,0x2e,0xe5,0xce,0x60,0x2d,
	0xe6,0x8e,0xd0,0xee,0x25,0xee,0x2e,0xe5,0xce,0x60,0x2d,0xe7,0x8e,0xd0,0xee,0x25,
	0xee,0x2e,0xe5,0xce,0x60,0x2d,0xd6,0x8e,0xd0,0xff,0x25,0xff,0x2e,0xf6,0xce,0x60,
	0x2d,0x30,0x8d,0x50,0x55,0x01,0x55,0x05,0x62,0x45,0x50,0x2d,0x10,0x9d,0x12,0x11,
	0x22,0x11,0x21,0x12,0x11,0x72,0x2d,0x10,0xed,0xee,0xee,0xee,0xee,0xee,0xee,0xee,
	0xed,0x2d,0x00,0x65,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x15,0x00,0x76,0x77,
	0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x16,0x10,0xed,0xee,0xee,0xee,0xee,0xee,0xee,
	0xee,0xed,0x2d,0x10,0x9d,0x11,0x11,0x11,0x00,0x10,0x01,0x00,0x71,0x2d,0x30,0x8d,
	0x00,0x00,0x10,0x66,0x06,0x72,0x56,0x60,0x2d,0xd6,0x8e,0x00,0x00,0x30,0xff,0x2f,
	0xf6,0xdf,0x60,0x2d,0xe7,0x8e,0x00,0x00,0x30,0xee,0x2e,0xe5,0xce,0x60,0x2d,0xe6,
	0x8e,0x00,0x00,0x30,0xee,0x2e,0xe5,0xce,0x60,0x2d,0xe6,0x8e,0x00,0x00,0x30,0xee,
	0x2e,0xe5,0xce,0x60,0x2d,0xe7,0x8e,0x00,0x00,0x30,0xee,0x2e,0xe5,0xce,0x60,0x2d,
	0xd6,0x8e,0x00,0x00,0x30,0xff,0x2e,0xf6,0xce,0x60,0x2d,0x30,0x8d,0x00,0x00,0x00,
	0x55,0x05,0x62,0x45,0x60,0x2d,0x10,0x9d,0x22,0x22,0x22,0x11,0x21,0x12,0x11,0x72,
	0x2d,0x10,0xed,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xed,0x2d,0x00,0x65,0x66,0x66,
	0x66,0x66,0x66,0x66,0x66,0x66,0x15,0x00,0x76,0x77,0x77,0x77,0x77,0x77,0x77,0x77,
	0x77,0x16,0x10,0xed,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xed,0x2d,0x10,0x9d,0x11,
	0x11,0x11,0x11,0x11,0x01,0x00,0x71,0x2d,0x30,0x8d,0x00,0x00,0x00,0x00,0x00,0x72,
	0x56,0x60,0x2d,0xd6,0x8e,0x00,0x00,0x00,0x00,0x00,0xf6,0xdf,0x60,0x2d,0xe7,0x8e,
	0x00,0x00,0x00,0x00,0x00,0xe6,0xce,0x60,0x2d,0xe6,0x8e,0x00,0x00,0x00,0x00,0x00,
	0xe6,0xce,0x60,0x2d,0xe6,0x8e,0x00,0x00,0x00,0x00,0x00,0xe6,0xce,0x60,0x2d,0xe7,
	0x8e,0x00,0x00,0x00,0x00,0x00,0xe6,0xce,0x60,0x2d,0xd6,0x8e,0x00,0x00,0x00,0x00,
	0x00,0xf6,0xce,0x60,0x2d,0x30,0x8d,0x00,0x00,0x00,0x00,0x00,0x62,0x45,0x60,0x2d,
	0x10,0x9d,0x22,0x22,0x22,0x22,0x22,0x12,0x11,0x72,0x2d,0x10,0xed,0xee,0xee,0xee,
	0xee,0xee,0xee,0xee,0xed,0x2d,0x00,0x65,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,
	0x15,0x00,0x76,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x16,0x10,0xed,0xee,0xee,
	0xee,0xee,0xee,0xee,0xee,0xed,0x2d,0x10,0x9d,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
	0x71,0x2d,0x30,0x8d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x2d,0xd6,0x8e,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x2d,0xe7,0x8e,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x60,0x2d,0xe6,0x8e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x2d,0xe6,0x8e,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x2d,0xe7,0x8e,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x60,0x2d,0xd6,0x8e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x2d,0x30,
	0x8d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x2d,0x10,0x9d,0x22,0x22,0x22,0x22,
	0x22,0x22,0x22,0x72,0x2d,0x10,0xed,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xed,0x2d,
	0x00,0x65,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x15,0x00,0x20,0xa5,0x99,0x99,
	0x99,0xa9,0x07,0x00,0xb5,0xf7,0xee,0xee,0xee,0xee,0x0a,0x40,0xbd,0xe7,0xee,0xee,
	0xdd,0xed,0x09,0x60,0x56,0xe8,0xee,0xee,0xde,0xed,0x09,0xc1,0xdd,0xee,0x45,0x44,
	0xed,0xed,0x09,0xd1,0xee,0xbd,0x01,0x00,0xda,0xee,0x09,0xd1,0xee,0x3d,0x00,0x00,
	0xc2,0xee,0x09,0xd1,0xee,0xde,0x03,0x20,0xec,0xee,0x09,0xd1,0xee,0xfe,0x4e,0xd3,
	0xef,0xee,0x09,0xd1,0xee,0xbe,0x69,0x85,0xda,0xee,0x09,0xd1,0xae,0x02,0x00,0x00,
	0x10,0xe8,0x0a,0xd1,0x9e,0x00,0x00,0x00,0x00,0xe6,0x09,0xd1,0x9b,0x8c,0x46,0x54,
	0xb8,0xaa,0x0a,0xd1,0x4c,0x96,0xbb,0xbb,0x79,0xb4,0x0a,0xd1,0xee,0x7a,0x67,0x76,
	0x97,0xfd,0x0a,0x71,0x88,0x98,0x89,0x88,0x99,0x88,0x05,0x00,0x00,0x00,0x20,0x32,
	0x44,0x22,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0xa7,0xaa,0x48,0x11,0x00,0x00,
	0x00,0x00,0x00,0x10,0x93,0xaa,0xaa,0xaa,0x16,0x00,0x00,0x00,0x00,0x00,0x21,0xa8,
	0xaa,0xaa,0xaa,0x3a,0x01,0x00,0x00,0x00,0x00,0x41,0xaa,0xaa,0xaa,0xaa,0x7a,0x01,
	0x00,0x00,0x00,0x00,0x51,0xaa,0xaa,0xaa,0xaa,0x8a,0x12,0x00,0x00,0x00,0x10,0x61,
	0xab,0xaa,0xaa,0xaa,0x8a,0x11,0x00,0x00,0x00,0x21,0xc7,0xef,0xbe,0xaa,0xbb,0xab,
	0x36,0x01,0x00,0x10,0xa3,0xcd,0xfe,0xff,0xbc,0xbb,0x9b,0x89,0x13,0x00,0x11,0xd9,
	0xcc,0xed,0xff,0xcf,0xbb,0x89,0x98,0x27,0x01,0x31,0xcc,0xcc,0xcc,0xfe,0xef,0x9a,
	0x88,0x88,0x49,0x01,0x51,0xcd,0xcc,0xcc,0xdc,0xde,0x89,0x88,0x88,0x59,0x01,0x51,
	0xcd,0xcc,0xcc,0xdc,0xcd,0x89,0x88,0x88,0x69,0x01,0x41,0xcc,0xcc,0xcc,0xdc,0xcd,
	0x88,0x88,0x88,0x49,0x01,0x11,0xd9,0xcc,0xcc,0xcc,0xae,0x88,0x88,0x98,0x28,0x01,
	0x10,0xb3,0xcd,0xcc,0xdd,0x89,0x89,0x98,0x89,0x14,0x00,0x00,0x31,0xc8,0xcc,0x6b,
	0x31,0x87,0x88,0x37,0x01,0x00,0x00,0x20,0x32,0x45,0x23,0x22,0x32,0x34,0x22,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const int ALIGN_DATA icon_s_pos[NUM_FONTS] = {
	0x000000,0x0000d8,0x000188,0x00021e,0x0002c0,0x000365,0x0003e9,0x00047f,0x00051f,0x0005ab,0x00062d,0x0006c3,0x00075d,0x0007f7,0x000891,0x00092b,
	0x0009ab
};

static const int8_t ALIGN_DATA icon_s_width[NUM_FONTS] = {
	24,22,20,18,22,22,20,20,20,20,20,22,22,22,22,16,
	22
};

static const int8_t ALIGN_DATA icon_s_height[NUM_FONTS] = {
	18,16,15,18,15,12,15,16,14,13,15,14,14,14,14,16,
	18
};

static const int8_t ALIGN_DATA icon_s_skipx[NUM_FONTS] = {
	 0, 1, 2, 3, 1, 1, 2, 1, 2, 2, 3, 1, 1, 1, 1, 4,
	 1
};

static const int8_t ALIGN_DATA icon_s_skipy[NUM_FONTS] = {
	 0, 1, 1, 0, 2, 3, 2, 1, 2, 3, 2, 2, 2, 2, 2, 1,
	 0
};


/*------------------------------------------------------
	light data
------------------------------------------------------*/

static const uint8_t ALIGN_DATA icon_s_light[] = {
	0x00,0x00,0x10,0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x32,0x44,0x34,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x64,0x88,
	0x68,0x24,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x52,0x97,0xbb,0x9b,0x47,
	0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x63,0xb9,0xdc,0xcd,0x6a,0x13,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x74,0xa9,0xcb,0xdd,0x8b,0x25,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x21,0x64,0x87,0xa9,0xdc,0xad,0x47,0x12,0x00,0x00,0x00,0x00,
	0x00,0x00,0x31,0x75,0x88,0xb9,0xed,0xce,0x7a,0x35,0x12,0x01,0x00,0x00,0x00,0x00,
	0x31,0x86,0xaa,0xcb,0xee,0xef,0xbc,0x79,0x35,0x23,0x22,0x11,0x00,0x00,0x21,0x75,
	0xba,0xdc,0xee,0xff,0xde,0xac,0x79,0x56,0x55,0x24,0x01,0x00,0x10,0x53,0xa8,0xcc,
	0xdd,0xee,0xee,0xde,0xbc,0xaa,0x99,0x57,0x13,0x00,0x00,0x31,0x75,0x99,0xaa,0xcb,
	0xed,0xee,0xee,0xdd,0xcc,0x8a,0x35,0x01,0x00,0x10,0x42,0x55,0x65,0x97,0xca,0xed,
	0xff,0xee,0xcd,0xab,0x57,0x12,0x00,0x00,0x11,0x22,0x32,0x53,0x97,0xcb,0xfe,0xee,
	0xbc,0xaa,0x68,0x13,0x00,0x00,0x00,0x00,0x10,0x21,0x53,0xa7,0xec,0xde,0x9b,0x88,
	0x57,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x74,0xda,0xcd,0x9b,0x88,0x47,0x12,
	0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x52,0xb8,0xdd,0xbc,0x9a,0x47,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x31,0xa6,0xdc,0xcd,0x9b,0x47,0x01,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x20,0x74,0xb9,0xbb,0x8a,0x36,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x10,0x42,0x86,0x88,0x56,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x21,0x43,0x44,0x23,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,
	0x11,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x10,0x33,0x23,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x21,0x65,0x46,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x31,
	0x75,0x68,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x31,0x75,0x88,0x25,
	0x01,0x00,0x00,0x00,0x00,0x00,0x10,0x21,0x22,0x32,0x75,0x99,0x46,0x23,0x22,0x12,
	0x01,0x00,0x00,0x21,0x54,0x55,0x65,0x97,0xaa,0x78,0x56,0x55,0x45,0x12,0x00,0x10,
	0x53,0x97,0xa9,0xaa,0xcb,0xdd,0xbc,0xaa,0x9a,0x79,0x35,0x01,0x20,0x84,0xca,0xdd,
	0xdd,0xed,0xee,0xde,0xdd,0xdd,0xac,0x48,0x02,0x31,0xa6,0xed,0xff,0xff,0xff,0xff,
	0xff,0xff,0xef,0xce,0x6a,0x13,0x41,0xc8,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xde,0x8b,0x14,0x52,0xc9,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xee,0x9c,0x25,
	0x52,0xda,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xee,0xee,0x9c,0x25,0x52,0xda,0xff,
	0xff,0xff,0xff,0xff,0xff,0xef,0xee,0xee,0x9c,0x25,0x52,0xd9,0xfe,0xff,0xff,0xff,
	0xff,0xff,0xff,0xee,0xee,0x9c,0x25,0x42,0xb8,0xed,0xff,0xff,0xff,0xff,0xff,0xef,
	0xee,0xde,0x8b,0x24,0x31,0x97,0xcb,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xbc,0x79,
	0x13,0x20,0x64,0x98,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x89,0x46,0x02,0x10,0x32,
	0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x45,0x23,0x01,0x00,0x10,0x21,0x22,0x22,
	0x22,0x22,0x22,0x22,0x22,0x12,0x01,0x00,0x00,0x10,0x21,0x22,0x12,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x21,0x54,0x55,0x45,0x13,0x01,0x00,0x00,0x00,0x00,0x00,
	0x10,0x53,0x87,0x99,0x78,0x45,0x23,0x22,0x22,0x12,0x00,0x00,0x21,0x85,0xa9,0xbb,
	0xab,0x79,0x56,0x55,0x55,0x34,0x12,0x00,0x41,0x97,0xba,0xbb,0xbb,0xab,0xaa,0xaa,
	0x9a,0x79,0x24,0x01,0x42,0xa8,0xaa,0xaa,0xbb,0xdc,0xdd,0xdd,0xdd,0xac,0x47,0x02,
	0x52,0xa8,0xaa,0xbb,0xcc,0xed,0xfe,0xff,0xef,0xce,0x7a,0x13,0x52,0xb9,0xcc,0xdd,
	0xdd,0xfe,0xff,0xff,0xff,0xef,0x8c,0x24,0x52,0xc9,0xed,0xee,0xff,0xff,0xff,0xff,
	0xff,0xef,0x9c,0x25,0x52,0xda,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x9c,0x25,
	0x52,0xda,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x8c,0x14,0x52,0xda,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xef,0x7b,0x13,0x52,0xda,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xdf,0x6a,0x12,0x52,0xda,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xde,0x59,0x02,
	0x52,0xd9,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xce,0x48,0x01,0x52,0xc9,0xfe,0xff,
	0xff,0xff,0xff,0xff,0xef,0xad,0x36,0x01,0x41,0xa7,0xdc,0xdd,0xdd,0xdd,0xdd,0xdd,
	0xcd,0x8a,0x24,0x00,0x21,0x75,0x99,0xaa,0xaa,0xaa,0xaa,0xaa,0x99,0x57,0x13,0x00,
	0x10,0x42,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x24,0x01,0x00,0x00,0x11,0x22,0x22,
	0x22,0x22,0x22,0x22,0x22,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x10,0x22,0x33,0x33,0x33,0x22,0x01,0x00,0x00,0x00,0x10,
	0x32,0x65,0x77,0x77,0x77,0x56,0x23,0x01,0x00,0x00,0x31,0x64,0xa8,0xbb,0xbb,0xbb,
	0x8a,0x57,0x13,0x00,0x10,0x53,0xa7,0xcb,0xdd,0xdd,0xdd,0xbc,0x8a,0x35,0x01,0x20,
	0x74,0xca,0xdd,0xdd,0xdd,0xde,0xcc,0xab,0x58,0x02,0x31,0x96,0xdc,0xee,0xcd,0xdc,
	0xcd,0xcc,0xcc,0x7a,0x13,0x52,0xb8,0xdd,0xdd,0xcc,0xdc,0xcd,0xbb,0xcc,0x8b,0x25,
	0x62,0xca,0xdd,0xdd,0xcc,0xdc,0xcd,0xbb,0xcc,0xac,0x26,0x73,0xda,0xdd,0xcd,0xcc,
	0xdd,0xcd,0xbb,0xcb,0xbc,0x37,0x73,0xdb,0xdd,0xdd,0xdd,0xed,0xde,0xbc,0xdc,0xbd,
	0x37,0x73,0xdb,0xee,0xdd,0xdd,0xed,0xde,0xcc,0xdc,0xbd,0x37,0x73,0xdb,0xee,0xee,
	0xee,0xee,0xee,0xdd,0xdd,0xbd,0x37,0x73,0xdb,0xee,0xee,0xee,0xde,0xee,0xde,0xdd,
	0xbd,0x37,0x73,0xdb,0xee,0xee,0xee,0xdd,0xee,0xdd,0xdd,0xad,0x37,0x62,0xca,0xed,
	0xee,0xde,0xed,0xde,0xdd,0xdd,0xac,0x26,0x52,0xb8,0xdd,0xed,0xee,0xee,0xde,0xdd,
	0xcd,0x8b,0x25,0x31,0xa7,0xdc,0xdd,0xee,0xee,0xde,0xdd,0xcd,0x7a,0x13,0x20,0x85,
	0xca,0xdd,0xee,0xee,0xde,0xdd,0xac,0x58,0x02,0x10,0x53,0xa8,0xcb,0xdd,0xee,0xdd,
	0xbc,0x8a,0x35,0x01,0x00,0x31,0x75,0xa8,0xbb,0xbb,0xab,0x8a,0x57,0x13,0x00,0x00,
	0x10,0x32,0x65,0x77,0x77,0x77,0x56,0x23,0x01,0x00,0x00,0x00,0x10,0x22,0x33,0x33,
	0x33,0x22,0x01,0x00,0x00,0x00,0x10,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x12,0x01,
	0x00,0x00,0x00,0x32,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x45,0x23,0x01,0x00,0x10,
	0x63,0x98,0xaa,0xaa,0xaa,0xaa,0xaa,0x9a,0x89,0x46,0x13,0x00,0x20,0x84,0xca,0xcc,
	0xcc,0xcc,0xcc,0xcc,0xcc,0xbc,0x8a,0x35,0x01,0x20,0x84,0xca,0xcc,0xcc,0xcc,0xcc,
	0xcc,0xcc,0xdd,0xbc,0x48,0x02,0x10,0x63,0x98,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xcb,
	0xcd,0x6a,0x13,0x00,0x32,0x65,0x77,0x66,0x55,0x55,0x55,0x66,0xa8,0xdc,0x8b,0x14,
	0x00,0x21,0x53,0x55,0x44,0x33,0x33,0x33,0x43,0x96,0xdc,0x9c,0x25,0x00,0x21,0x64,
	0x77,0x45,0x33,0x33,0x33,0x43,0x96,0xdc,0x9c,0x25,0x00,0x31,0x86,0xaa,0x78,0x56,
	0x55,0x55,0x66,0xa8,0xdc,0x8b,0x14,0x10,0x63,0xb9,0xcd,0xab,0xaa,0xaa,0xaa,0xaa,
	0xcb,0xcd,0x6a,0x13,0x20,0x85,0xdb,0xee,0xdd,0xcc,0xcc,0xcc,0xdc,0xdd,0xbd,0x48,
	0x02,0x21,0x95,0xec,0xee,0xdd,0xcc,0xcc,0xcc,0xcc,0xcc,0x8b,0x35,0x01,0x20,0x84,
	0xcb,0xdd,0xac,0xaa,0xaa,0xaa,0xaa,0x99,0x57,0x13,0x00,0x10,0x53,0xa8,0xab,0x78,
	0x56,0x55,0x55,0x55,0x55,0x24,0x01,0x00,0x00,0x31,0x65,0x77,0x45,0x23,0x22,0x22,
	0x22,0x22,0x11,0x00,0x00,0x00,0x10,0x32,0x44,0x23,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x10,0x22,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x10,0x21,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x12,0x01,0x00,0x00,0x21,0x54,0x55,
	0x55,0x55,0x55,0x55,0x55,0x55,0x45,0x12,0x00,0x10,0x53,0x97,0x99,0x99,0x99,0x99,
	0x99,0x99,0x99,0x79,0x35,0x01,0x21,0x85,0xcb,0xbc,0xbb,0xab,0xaa,0xba,0xbb,0xcb,
	0xbc,0x58,0x12,0x41,0xb7,0xdd,0xbd,0x9a,0x99,0x99,0x99,0xa9,0xdb,0xdd,0x7b,0x14,
	0x52,0xc9,0xde,0x9c,0x67,0x55,0x55,0x55,0x76,0xc9,0xed,0x9c,0x25,0x52,0xd9,0xde,
	0x7b,0x34,0x22,0x22,0x22,0x43,0xb7,0xed,0x9d,0x25,0x52,0xda,0xde,0x6a,0x13,0x00,
	0x00,0x00,0x31,0xa6,0xed,0xad,0x25,0x52,0xda,0xde,0x5a,0x12,0x00,0x00,0x00,0x21,
	0xa5,0xed,0xad,0x25,0x52,0xda,0xde,0x6a,0x13,0x00,0x00,0x00,0x31,0xa6,0xed,0xad,
	0x25,0x52,0xc9,0xde,0x7b,0x34,0x22,0x22,0x22,0x43,0xb7,0xed,0x9c,0x25,0x41,0xb8,
	0xdd,0x9c,0x67,0x55,0x55,0x55,0x76,0xc9,0xdd,0x8b,0x14,0x31,0xa6,0xdc,0xbc,0x9a,
	0x99,0x99,0x99,0xa9,0xcb,0xcd,0x6a,0x13,0x20,0x74,0xb9,0xbc,0xbb,0xab,0xaa,0xba,
	0xbb,0xcb,0x9b,0x47,0x02,0x10,0x42,0x86,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x68,
	0x24,0x01,0x00,0x21,0x43,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x34,0x12,0x00,0x00,
	0x00,0x11,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x11,0x00,0x00,0x21,0x32,0x33,0x33,
	0x33,0x33,0x33,0x33,0x33,0x33,0x23,0x12,0x42,0x76,0x77,0x77,0x77,0x77,0x77,0x77,
	0x77,0x77,0x67,0x24,0x62,0xb9,0xbb,0xbb,0xbb,0xbb,0xbb,0xbb,0xbb,0xbb,0x9b,0x26,
	0x73,0xdb,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xde,0xdd,0xbd,0x37,0x73,0xdb,0xee,0xee,
	0xcd,0xdc,0xee,0xee,0xee,0xee,0xbd,0x37,0x73,0xdb,0xee,0xde,0xbc,0xcb,0xed,0xee,
	0xee,0xee,0xbd,0x37,0x73,0xdb,0xdd,0xdd,0xac,0xcb,0xdd,0xdd,0xdd,0xdd,0xbd,0x37,
	0x73,0xcb,0xcc,0xcc,0xab,0xca,0xcc,0xcc,0xcd,0xcc,0xbc,0x37,0x73,0xba,0xbb,0xcc,
	0xab,0xba,0xbb,0xcb,0xcc,0xbb,0xab,0x37,0x73,0xba,0xaa,0xcb,0xac,0xbb,0xab,0xba,
	0xbc,0xaa,0xab,0x37,0x73,0xba,0xaa,0xdb,0xbc,0xcc,0xab,0xba,0xbc,0xaa,0xab,0x37,
	0x73,0xba,0xaa,0xdc,0xdd,0xdd,0xac,0xba,0xbc,0xaa,0xab,0x37,0x73,0xba,0xbb,0xdc,
	0xee,0xde,0xbc,0xcb,0xcc,0xbb,0xab,0x37,0x73,0xcb,0xcc,0xed,0xee,0xee,0xcd,0xcc,
	0xcd,0xcc,0xbc,0x37,0x73,0xdb,0xdd,0xed,0xee,0xee,0xdd,0xdd,0xdd,0xdd,0xbc,0x37,
	0x73,0xca,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xac,0x37,0x52,0xa8,0xaa,0xaa,
	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x8a,0x25,0x31,0x65,0x66,0x77,0x77,0x77,0x77,0x77,
	0x77,0x66,0x56,0x13,0x10,0x32,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x23,0x01,
	0x00,0x10,0x21,0x12,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x44,
	0x44,0x13,0x11,0x22,0x22,0x12,0x00,0x00,0x00,0x00,0x10,0x53,0x97,0x89,0x46,0x33,
	0x54,0x55,0x34,0x12,0x00,0x00,0x00,0x21,0x85,0xcb,0xbc,0x79,0x76,0x98,0x99,0x79,
	0x24,0x01,0x00,0x00,0x41,0xb7,0xed,0xde,0xac,0xa9,0xcb,0xdd,0xac,0x47,0x02,0x00,
	0x00,0x52,0xc9,0xfe,0xef,0xcd,0xcb,0xed,0xee,0xce,0x69,0x13,0x00,0x00,0x42,0xc9,
	0xfe,0xef,0xcd,0xcb,0xed,0xff,0xce,0x6a,0x13,0x00,0x00,0x41,0xa7,0xed,0xde,0xac,
	0xba,0xec,0xff,0xcd,0x59,0x12,0x00,0x00,0x21,0x85,0xba,0xab,0x89,0x98,0xdb,0xee,
	0xac,0x47,0x02,0x00,0x00,0x10,0x43,0x76,0x78,0x77,0x97,0xdb,0xde,0xac,0x57,0x12,
	0x00,0x00,0x00,0x21,0x43,0x55,0x86,0xb9,0xec,0xee,0xbd,0x79,0x35,0x01,0x00,0x00,
	0x00,0x21,0x53,0xa8,0xdc,0xfe,0xff,0xde,0xac,0x58,0x13,0x00,0x00,0x00,0x10,0x63,
	0xc9,0xed,0xfe,0xff,0xee,0xde,0x8b,0x24,0x00,0x00,0x00,0x10,0x74,0xca,0xed,0xee,
	0xee,0xee,0xee,0xac,0x36,0x01,0x00,0x00,0x10,0x84,0xca,0xdd,0xcc,0xcc,0xdd,0xee,
	0xbd,0x37,0x01,0x00,0x00,0x10,0x73,0xb9,0xbc,0xab,0xaa,0xcb,0xdd,0xac,0x37,0x01,
	0x00,0x00,0x10,0x52,0x97,0xaa,0xaa,0xaa,0xba,0xbb,0x8a,0x25,0x01,0x00,0x00,0x00,
	0x21,0x64,0x87,0x88,0x88,0x88,0x88,0x46,0x12,0x00,0x00,0x00,0x00,0x10,0x32,0x44,
	0x55,0x55,0x55,0x45,0x23,0x01,0x00,0x00,0x00,0x00,0x00,0x10,0x21,0x22,0x22,0x22,
	0x12,0x01,0x00,0x00,0x00,0x00,0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x10,0x32,0x23,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0x75,0x57,
	0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x63,0xa8,0x8a,0x36,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x31,0x85,0xdb,0xbd,0x58,0x12,0x00,0x00,0x00,0x00,0x00,0x00,
	0x42,0xa7,0xed,0xce,0x6a,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x42,0xa7,0xed,0xce,
	0x69,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0x85,0xdb,0xbd,0x58,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x20,0x74,0xda,0xad,0x37,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
	0x10,0x63,0xca,0xac,0x36,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x52,0xc9,0xbd,
	0x47,0x23,0x22,0x22,0x22,0x22,0x12,0x00,0x00,0x52,0xc9,0xcd,0x79,0x56,0x55,0x55,
	0x55,0x55,0x34,0x02,0x00,0x41,0xb8,0xdd,0xbc,0xaa,0xaa,0xaa,0xaa,0x9a,0x68,0x13,
	0x00,0x31,0x96,0xdc,0xdd,0xdd,0xdd,0xdd,0xdd,0xcd,0x8b,0x24,0x00,0x20,0x74,0xba,
	0xdd,0xee,0xee,0xee,0xee,0xde,0x9c,0x25,0x00,0x10,0x42,0x86,0xba,0xdc,0xdd,0xdd,
	0xdd,0xcd,0x8b,0x24,0x00,0x00,0x21,0x53,0x86,0xa9,0xaa,0xaa,0xaa,0x9a,0x68,0x13,
	0x00,0x00,0x00,0x21,0x43,0x55,0x55,0x55,0x55,0x55,0x34,0x02,0x00,0x00,0x00,0x00,
	0x11,0x22,0x22,0x22,0x22,0x22,0x12,0x00,0x00,0x21,0x22,0x22,0x22,0x22,0x22,0x22,
	0x22,0x22,0x12,0x00,0x20,0x43,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x34,0x02,
	0x31,0x86,0xa9,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x9a,0x68,0x13,0x42,0xb8,0xdc,0xdd,
	0xdd,0xdd,0xdd,0xdd,0xdd,0xcd,0x8b,0x24,0x52,0xc9,0xfe,0xff,0xff,0xff,0xff,0xff,
	0xff,0xef,0x9c,0x25,0x52,0xda,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xad,0x25,
	0x52,0xda,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xad,0x25,0x52,0xda,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xad,0x25,0x52,0xda,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xad,0x25,0x52,0xd9,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xad,0x25,
	0x52,0xc9,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xad,0x25,0x41,0xb8,0xfd,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xad,0x25,0x21,0x95,0xdb,0xfe,0xff,0xff,0xff,0xff,
	0xff,0xef,0x9c,0x25,0x10,0x63,0xb9,0xdc,0xdd,0xdd,0xdd,0xdd,0xdd,0xcd,0x8b,0x24,
	0x00,0x31,0x85,0xa9,0xaa,0xaa,0xaa,0xaa,0xaa,0x9a,0x68,0x13,0x00,0x10,0x42,0x55,
	0x55,0x55,0x55,0x55,0x55,0x55,0x34,0x02,0x00,0x00,0x11,0x22,0x22,0x22,0x22,0x22,
	0x22,0x22,0x12,0x00,0x00,0x10,0x21,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x11,0x00,
	0x00,0x00,0x21,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x24,0x01,0x00,0x10,0x53,
	0x97,0xa9,0xaa,0xaa,0xaa,0xaa,0xaa,0x99,0x57,0x12,0x00,0x20,0x84,0xca,0xdd,0xdd,
	0xdd,0xdd,0xdd,0xdd,0xcd,0x7a,0x14,0x00,0x21,0x95,0xec,0xfe,0xff,0xff,0xff,0xee,
	0xee,0xde,0x8b,0x14,0x00,0x20,0x95,0xec,0xff,0xff,0xff,0xff,0xef,0xee,0xde,0x7b,
	0x14,0x00,0x10,0x74,0xeb,0xff,0xff,0xff,0xff,0xde,0xdd,0xcd,0x69,0x13,0x00,0x10,
	0x63,0xda,0xff,0xff,0xff,0xff,0xde,0xcc,0xcd,0x59,0x02,0x00,0x00,0x62,0xda,0xff,
	0xff,0xff,0xff,0xde,0xcb,0xcd,0x59,0x02,0x00,0x10,0x63,0xda,0xff,0xff,0xff,0xff,
	0xde,0xcc,0xcd,0x59,0x02,0x00,0x10,0x73,0xda,0xff,0xff,0xff,0xff,0xde,0xdd,0xcd,
	0x69,0x13,0x00,0x20,0x84,0xdb,0xff,0xff,0xff,0xff,0xef,0xee,0xde,0x7b,0x14,0x00,
	0x21,0x85,0xdb,0xfe,0xff,0xff,0xff,0xef,0xee,0xde,0x8b,0x24,0x00,0x20,0x74,0xca,
	0xdd,0xdd,0xdd,0xdd,0xed,0xee,0xde,0x8b,0x25,0x00,0x10,0x53,0x97,0xa9,0xaa,0xaa,
	0xaa,0xcb,0xed,0xde,0x9b,0x25,0x01,0x00,0x21,0x54,0x55,0x55,0x55,0x76,0xb8,0xdc,
	0xde,0xac,0x37,0x01,0x00,0x10,0x21,0x22,0x22,0x22,0x43,0x96,0xcb,0xcc,0xab,0x47,
	0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x64,0x98,0x9a,0x89,0x36,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x10,0x32,0x54,0x55,0x45,0x23,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x10,0x22,0x22,0x22,0x01,0x00,0x00,0x10,0x22,0x22,0x22,0x22,0x22,0x22,
	0x22,0x22,0x22,0x12,0x00,0x00,0x32,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x34,0x02,0x10,0x63,0x98,0xa9,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x99,0x68,0x13,0x20,
	0x95,0xcb,0xdc,0xdd,0xdd,0xdd,0xdd,0xdd,0xcd,0xcc,0x8b,0x24,0x31,0xa7,0xed,0xee,
	0xfe,0xff,0xff,0xff,0xef,0xee,0xdd,0x9c,0x25,0x41,0xc8,0xed,0xee,0xff,0xff,0xff,
	0xff,0xff,0xde,0xdc,0x9c,0x25,0x52,0xc9,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xde,
	0xcc,0x9b,0x25,0x52,0xd9,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xce,0xcb,0x9b,0x25,
	0x52,0xda,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xce,0xbb,0x9b,0x25,0x52,0xda,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xce,0xbb,0x9b,0x25,0x52,0xd9,0xfe,0xff,0xff,0xff,
	0xff,0xff,0xff,0xce,0xcb,0x9b,0x25,0x52,0xc9,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,
	0xde,0xcc,0x9b,0x25,0x41,0xc8,0xed,0xee,0xff,0xff,0xff,0xff,0xff,0xde,0xdc,0x9c,
	0x25,0x31,0xa7,0xed,0xee,0xfe,0xff,0xff,0xff,0xef,0xee,0xdd,0x9c,0x25,0x20,0x95,
	0xcb,0xdc,0xdd,0xdd,0xdd,0xdd,0xdd,0xcd,0xcc,0x8b,0x24,0x10,0x63,0x98,0xa9,0xaa,
	0xaa,0xaa,0xaa,0xaa,0xaa,0x99,0x68,0x13,0x00,0x32,0x54,0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x34,0x02,0x00,0x10,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
	0x12,0x00,0x00,0x10,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x12,0x00,0x00,
	0x32,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x34,0x02,0x10,0x63,0x98,0x99,
	0x99,0x99,0xaa,0xaa,0xaa,0xaa,0x99,0x68,0x13,0x20,0x95,0xcb,0xbb,0xbb,0xcb,0xdd,
	0xdd,0xdd,0xcd,0xcc,0x8b,0x24,0x31,0xa7,0xcc,0xab,0xaa,0xdb,0xee,0xff,0xef,0xee,
	0xdd,0x9c,0x25,0x41,0xb8,0xbc,0x79,0x76,0xc9,0xfe,0xff,0xff,0xde,0xdc,0x9c,0x25,
	0x52,0xc9,0xac,0x47,0x43,0xb7,0xfd,0xff,0xff,0xde,0xcc,0x9b,0x25,0x52,0xc9,0xac,
	0x36,0x32,0xa6,0xfd,0xff,0xff,0xce,0xcb,0x9b,0x25,0x52,0xca,0xac,0x25,0x21,0xa5,
	0xfd,0xff,0xff,0xce,0xbb,0x9b,0x25,0x52,0xca,0xac,0x25,0x21,0xa5,0xfd,0xff,0xff,
	0xce,0xbb,0x9b,0x25,0x52,0xc9,0xac,0x36,0x32,0xa6,0xfd,0xff,0xff,0xce,0xcb,0x9b,
	0x25,0x52,0xc9,0xac,0x47,0x43,0xb7,0xfd,0xff,0xff,0xde,0xcc,0x9b,0x25,0x41,0xb8,
	0xbc,0x79,0x76,0xc9,0xfe,0xff,0xff,0xde,0xdc,0x9c,0x25,0x31,0xa7,0xcc,0xab,0xaa,
	0xdb,0xee,0xff,0xef,0xee,0xdd,0x9c,0x25,0x20,0x95,0xcb,0xbb,0xbb,0xcb,0xdd,0xdd,
	0xdd,0xcd,0xcc,0x8b,0x24,0x10,0x63,0x98,0x99,0x99,0x99,0xaa,0xaa,0xaa,0xaa,0x99,
	0x68,0x13,0x00,0x32,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x34,0x02,0x00,
	0x10,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x12,0x00,0x00,0x10,0x22,0x22,
	0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x12,0x00,0x00,0x32,0x54,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x34,0x02,0x10,0x63,0x98,0x99,0x99,0x99,0x99,0x99,0xa9,0xaa,
	0x99,0x68,0x13,0x20,0x95,0xcb,0xbb,0xbb,0xaa,0xbb,0xbb,0xcc,0xcd,0xcc,0x8b,0x24,
	0x31,0xa7,0xcc,0xab,0x99,0x99,0x99,0xba,0xed,0xee,0xdd,0x9c,0x25,0x41,0xb8,0xbc,
	0x79,0x56,0x55,0x65,0x97,0xdc,0xde,0xdc,0x9c,0x25,0x52,0xc9,0xac,0x47,0x23,0x22,
	0x32,0x74,0xdb,0xdd,0xcc,0x9b,0x25,0x52,0xc9,0xac,0x36,0x01,0x00,0x10,0x63,0xca,
	0xcd,0xcb,0x9b,0x25,0x52,0xca,0xac,0x25,0x00,0x00,0x00,0x52,0xca,0xcd,0xbb,0x9b,
	0x25,0x52,0xca,0xac,0x25,0x00,0x00,0x00,0x52,0xca,0xcd,0xbb,0x9b,0x25,0x52,0xc9,
	0xac,0x36,0x01,0x00,0x10,0x63,0xca,0xcd,0xcb,0x9b,0x25,0x52,0xc9,0xac,0x47,0x23,
	0x22,0x32,0x74,0xdb,0xdd,0xcc,0x9b,0x25,0x41,0xb8,0xbc,0x79,0x56,0x55,0x65,0x97,
	0xdc,0xde,0xdc,0x9c,0x25,0x31,0xa7,0xcc,0xab,0x99,0x99,0x99,0xba,0xed,0xee,0xdd,
	0x9c,0x25,0x20,0x95,0xcb,0xbb,0xbb,0xaa,0xbb,0xbb,0xcc,0xcd,0xcc,0x8b,0x24,0x10,
	0x63,0x98,0x99,0x99,0x99,0x99,0x99,0xa9,0xaa,0x99,0x68,0x13,0x00,0x32,0x54,0x55,
	0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x34,0x02,0x00,0x10,0x22,0x22,0x22,0x22,0x22,
	0x22,0x22,0x22,0x22,0x12,0x00,0x00,0x10,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
	0x22,0x12,0x00,0x00,0x32,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x34,0x02,
	0x10,0x63,0x98,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x68,0x13,0x20,0x95,0xcb,
	0xbb,0xbb,0xaa,0xaa,0xaa,0xba,0xbb,0xbb,0x8a,0x24,0x31,0xa7,0xcc,0xab,0x99,0x99,
	0x99,0x99,0x99,0xa9,0xcb,0x9b,0x25,0x41,0xb8,0xbc,0x79,0x56,0x55,0x55,0x55,0x55,
	0x76,0xb9,0x9b,0x25,0x52,0xc9,0xac,0x47,0x23,0x22,0x22,0x22,0x22,0x43,0xa7,0x9b,
	0x25,0x52,0xc9,0xac,0x36,0x01,0x00,0x00,0x00,0x00,0x31,0x96,0x9b,0x25,0x52,0xca,
	0xac,0x25,0x00,0x00,0x00,0x00,0x00,0x20,0x95,0x9a,0x25,0x52,0xca,0xac,0x25,0x00,
	0x00,0x00,0x00,0x00,0x20,0x95,0x9a,0x25,0x52,0xc9,0xac,0x36,0x01,0x00,0x00,0x00,
	0x00,0x31,0x96,0x9b,0x25,0x52,0xc9,0xac,0x47,0x23,0x22,0x22,0x22,0x22,0x43,0xa7,
	0x9b,0x25,0x41,0xb8,0xbc,0x79,0x56,0x55,0x55,0x55,0x55,0x76,0xb9,0x9b,0x25,0x31,
	0xa7,0xcc,0xab,0x99,0x99,0x99,0x99,0x99,0xa9,0xcb,0x9b,0x25,0x20,0x95,0xcb,0xbb,
	0xbb,0xaa,0xaa,0xaa,0xba,0xbb,0xbb,0x8a,0x24,0x10,0x63,0x98,0x99,0x99,0x99,0x99,
	0x99,0x99,0x99,0x99,0x68,0x13,0x00,0x32,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x55,0x34,0x02,0x00,0x10,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x12,0x00,
	0x00,0x00,0x11,0x22,0x22,0x22,0x22,0x22,0x01,0x00,0x00,0x10,0x42,0x55,0x55,0x55,
	0x55,0x45,0x23,0x00,0x00,0x31,0x85,0xa9,0xaa,0xaa,0xaa,0x89,0x36,0x01,0x10,0x63,
	0xb9,0xdc,0xdd,0xdd,0xdd,0xbc,0x48,0x02,0x21,0x95,0xdb,0xee,0xee,0xee,0xff,0xce,
	0x59,0x02,0x41,0xb8,0xfd,0xff,0xee,0xee,0xff,0xdf,0x5a,0x02,0x52,0xc9,0xfe,0xef,
	0xcd,0xdc,0xfe,0xdf,0x5a,0x02,0x52,0xd9,0xff,0xdf,0xab,0xba,0xfd,0xdf,0x5a,0x02,
	0x52,0xda,0xff,0xde,0x9b,0xb9,0xed,0xde,0x5a,0x02,0x52,0xda,0xff,0xdf,0x9b,0xb9,
	0xed,0xde,0x5a,0x02,0x52,0xda,0xff,0xdf,0xac,0xba,0xed,0xde,0x5a,0x02,0x52,0xda,
	0xff,0xde,0xab,0xa9,0xdb,0xde,0x5a,0x02,0x52,0xda,0xff,0xce,0x8a,0x88,0xca,0xcd,
	0x5a,0x02,0x52,0xda,0xee,0xbd,0x8a,0x88,0xba,0xcd,0x59,0x02,0x52,0xda,0xfe,0xde,
	0xbc,0xbb,0xdc,0xcd,0x59,0x02,0x52,0xc9,0xee,0xde,0xdd,0xdc,0xdd,0xcd,0x59,0x02,
	0x42,0xb8,0xdc,0xdd,0xcc,0xcc,0xcc,0xbc,0x48,0x02,0x31,0x86,0xa9,0xaa,0xaa,0xaa,
	0xaa,0x89,0x36,0x01,0x20,0x43,0x55,0x55,0x55,0x55,0x55,0x45,0x23,0x00,0x00,0x21,
	0x22,0x22,0x22,0x22,0x22,0x22,0x01,0x00,0x00,0x00,0x00,0x00,0x11,0x22,0x22,0x22,
	0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x42,0x55,0x55,0x55,0x24,0x01,0x00,
	0x00,0x00,0x00,0x00,0x00,0x31,0x85,0xa9,0xaa,0x99,0x57,0x13,0x00,0x00,0x00,0x00,
	0x00,0x10,0x63,0xb9,0xdc,0xdd,0xcd,0x8a,0x24,0x00,0x00,0x00,0x00,0x00,0x31,0x95,
	0xdb,0xfe,0xff,0xef,0xad,0x36,0x01,0x00,0x00,0x00,0x00,0x41,0xb8,0xfd,0xff,0xff,
	0xff,0xce,0x58,0x02,0x00,0x00,0x00,0x10,0x62,0xd9,0xfe,0xff,0xff,0xff,0xdf,0x7a,
	0x13,0x00,0x00,0x00,0x20,0x74,0xeb,0xff,0xff,0xff,0xff,0xef,0x9c,0x25,0x01,0x00,
	0x00,0x31,0xa6,0xec,0xff,0xff,0xff,0xff,0xff,0xad,0x47,0x02,0x00,0x10,0x63,0xc9,
	0xfe,0xff,0xff,0xff,0xff,0xff,0xce,0x69,0x13,0x00,0x21,0x95,0xec,0xff,0xff,0xff,
	0xff,0xff,0xff,0xef,0x9c,0x25,0x01,0x41,0xb8,0xfd,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xbd,0x48,0x01,0x52,0xc9,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xce,0x59,
	0x02,0x52,0xd9,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdf,0x59,0x02,0x52,0xd9,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdf,0x59,0x02,0x52,0xc9,0xfe,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xce,0x59,0x02,0x41,0xb8,0xfd,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xbd,0x48,0x01,0x21,0x95,0xdb,0xfe,0xff,0xff,0xff,0xff,0xff,0xde,0x9b,
	0x25,0x01,0x10,0x63,0xb9,0xdc,0xdd,0xdd,0xdd,0xdd,0xdd,0xbc,0x69,0x13,0x00,0x00,
	0x31,0x85,0xa9,0xaa,0xaa,0xaa,0xaa,0xaa,0x89,0x35,0x01,0x00,0x00,0x10,0x42,0x55,
	0x55,0x55,0x55,0x55,0x55,0x45,0x12,0x00,0x00,0x00,0x00,0x11,0x22,0x22,0x22,0x22,
	0x22,0x22,0x12,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const int ALIGN_DATA icon_s_light_pos[NUM_FONTS] = {
	0x000000,0x000134,0x000238,0x000328,0x000425,0x00050f,0x0005ec,0x0006d0,0x0007d4,0x0008b8,0x000984,0x000a88,0x000b72,0x000c5c,0x000d46,0x000e30,
	0x000ef8
};

static const int8_t ALIGN_DATA icon_s_light_width[NUM_FONTS] = {
	28,26,24,22,26,26,24,26,24,24,26,26,26,26,26,20,
	26
};

static const int8_t ALIGN_DATA icon_s_light_height[NUM_FONTS] = {
	22,20,20,23,18,17,19,20,19,17,20,18,18,18,18,20,
	22
};

static const int8_t ALIGN_DATA icon_s_light_skipx[NUM_FONTS] = {
	 2, 3, 4, 5, 3, 3, 4, 3, 4, 4, 4, 3, 3, 3, 3, 6,
	 3
};

static const int8_t ALIGN_DATA icon_s_light_skipy[NUM_FONTS] = {
	 2, 3, 3, 1, 4, 5, 4, 3, 4, 5, 3, 4, 4, 4, 4, 3,
	 2
};


/*------------------------------------------------------
	functions
------------------------------------------------------*/

int icon_s_get_gryph(struct font_t *font, uint16_t code)
{
	if (code < NUM_FONTS)
	{
		font->data   = &icon_s[icon_s_pos[code]];
		font->width  = icon_s_width[code];
		font->height = icon_s_height[code];
		font->pitch  = 32;
		font->skipx  = icon_s_skipx[code];
		font->skipy  = icon_s_skipy[code];
		return 1;
	}
	return 0;
}

int icon_s_get_light(struct font_t *font, uint16_t code)
{
	if (code < NUM_FONTS)
	{
		font->data   = &icon_s_light[icon_s_light_pos[code]];
		font->width  = icon_s_light_width[code];
		font->height = icon_s_light_height[code];
		font->pitch  = 32;
		font->skipx  = icon_s_light_skipx[code];
		font->skipy  = icon_s_light_skipy[code];
		return 1;
	}
	return 0;
}

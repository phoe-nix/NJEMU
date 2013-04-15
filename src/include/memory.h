/******************************************************************************

	memory.h

	Core memory interface definitions. (compatible for M.A.M.E.)

******************************************************************************/

#ifndef MEMORY_H
#define MEMORY_H

#define READ8_HANDLER(name) 	UINT8 name(UINT32 offset)
#define WRITE8_HANDLER(name) 	void name(UINT32 offset, UINT8 data)
#define READ16_HANDLER(name)	UINT16 name(UINT32 offset, UINT16 mem_mask)
#define WRITE16_HANDLER(name)	void name(UINT32 offset, UINT16 data, UINT16 mem_mask)
#define READ32_HANDLER(name)	UINT32 name(UINT32 offset)
#define WRITE32_HANDLER(name)	void name(UINT32 offset, UINT32 data)

/* ----- 16/32-bit memory accessing ----- */
#define COMBINE_DATA(varptr)		(*(varptr) = (*(varptr) & mem_mask) | (data & ~mem_mask))
#define COMBINE_SWABDATA(varptr)	(*(varptr) = (*(varptr) & (mem_mask << 8)) | (data & ~(mem_mask >> 8)))

/* ----- 16-bit memory accessing ----- */
#define ACCESSING_LSB			((mem_mask & 0x00ff) == 0)
#define ACCESSING_MSB			((mem_mask & 0xff00) == 0)

#endif /* MEMORY_H */

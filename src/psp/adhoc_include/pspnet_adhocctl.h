/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * pspnet_adhocctl.h - PSP Adhoc control networking libraries.
 *
 * Copyright (c) 2006 James F.
 *
 * Based on the adhoc code in SMS Plus
 * 
 * $Id: pspnet_adhocctl.h 1836 2006-03-16 20:58:05Z tyranid $
 */
#ifndef __PSPNETCTL_ADHOC_H__
#define __PSPNETCTL_ADHOC_H__

#ifdef __cplusplus
extern "C" {
#endif

/** Product structure */
struct productStruct
{
	/** Unknown, set to 0 */
	int unknown;
	/** The product ID string */
	unsigned char product[9];
	unsigned char filler[3];
} productStruct;

/**
 * Initialise the Adhoc control library
 *
 * @param unk1 - Set to 0x2000
 * @param unk2 - Set to 0x30
 * @param product - Pass a filled in ::productStruct
 *
 * @return 0 on success, < 0 on error
 */
int sceNetAdhocctlInit(SceSize unk1, int unk2, struct productStruct *product);

/**
 * Terminate the Adhoc control library
 *
 * @return 0 on success, < on error.
 */
int sceNetAdhocctlTerm(void);

/**
 * Connect to the Adhoc control
 *
 * @param unk1 - Pass ""
 *
 * @return 0 on success, < 0 on error.
 */
int sceNetAdhocctlConnect(const void *unk1);

/**
 * Disconnect from the Adhoc control
 *
 * @return 0 on success, < 0 on error
 */
int sceNetAdhocctlDisconnect(void);

/**
 * Get the state of the Adhoc control
 *
 * @param event - Pointer to an integer to receive the status. Can continue when it becomes 1.
 *
 * @return 0 on success, < 0 on error
 */
int sceNetAdhocctlGetState(int *event);


#ifdef __cplusplus
}
#endif

#endif

/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * pspnet_adhoc.h - PSP Adhoc networking libraries.
 *
 * Copyright (c) 2006 James F.
 *
 * Based on the adhoc code in SMS Plus
 * 
 * $Id: pspnet_adhoc.h 2252 2007-06-07 04:27:31Z oopo $
 */
#ifndef __PSPNET_ADHOC_H__
#define __PSPNET_ADHOC_H__

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * Initialise the adhoc library.
 *
 * @return 0 on success, < 0 on error
 */
int sceNetAdhocInit( void );

/**
 * Terminate the adhoc library
 *
 * @return 0 on success, < 0 on error
 */
int sceNetAdhocTerm( void );

/**
 * Create a PDP object.
 *
 * @param mac - Your MAC address (from sceWlanGetEtherAddr)
 * @param port - Port to use, lumines uses 0x309
 * @param unk2 - Unknown, lumines sets to 0x400
 * @param unk3 - Unknown, lumines sets to 0
 *
 * @return The ID of the PDP object (< 0 on error)
 */
int sceNetAdhocPdpCreate(unsigned char *mac, unsigned short port, unsigned int unk2, int unk3);

/**
 * Delete a PDP object.
 *
 * @param id - The ID returned from ::sceNetAdhocPdpCreate
 * @param unk1 - Unknown, set to 0
 *
 * @return 0 on success, < 0 on error
 */
int sceNetAdhocPdpDelete(int id, int unk1); 

/**
 * Set a PDP packet to a destination
 *
 * @param id - The ID as returned by ::sceNetAdhocPdpCreate
 * @param destMacAddr - The destination MAC address, can be set to all 0xFF for broadcast
 * @param port - The port to send to
 * @param data - The data to send
 * @param len - The length of the data.
 * @param unk6 - Unknown, set to 0
 * @param unk7 - Unknown, set to 0
 *
 * @return Bytes sent, < 0 on error
 */
int sceNetAdhocPdpSend(int id, unsigned char *destMacAddr, unsigned short port, void *data, unsigned int len, int unk6, int unk7); 

/**
 * Receive a PDP packet
 *
 * @param id - The ID of the PDP object, as returned by ::sceNetAdhocPdpCreate
 * @param srcMacAddr - Buffer to hold the source mac address of the sender
 * @param port - Buffer to hold the port number of he received data
 * @param data - Data buffer
 * @param dataLength - The length of the data buffer
 * @param unk6 - Set to 0
 * @param unk7 - Set to 0
 *
 * @return Number of bytes received, < 0 on error.
 */
int sceNetAdhocPdpRecv(int id, unsigned char *srcMacAddr, unsigned short *port, void *data, void *dataLength, int unk6, int unk7);

/**
 * PDP status structure
 */
typedef struct pdpStatStruct
{
	/** Pointer to next PDP structure in list */
	struct pdpStatStruct *next;
	/** pdp ID */
	int pdpId;
	/** MAC address */
	unsigned char mac[6];
	/** Port */
	unsigned short port;
	/** Bytes received */
	unsigned int rcvdData;
} pdpStatStruct;

/**
 * Get the status of all PDP objects
 *
 * @param size - Pointer to the size of the stat array (e.g 20 for one structure)
 * @param stat - Pointer to a list of ::pspStatStruct structures.
 *
 * @return 0 on success, < 0 on error
 */
int sceNetAdhocGetPdpStat(int *size, pdpStatStruct *stat);

#ifdef __cplusplus
}
#endif

#endif

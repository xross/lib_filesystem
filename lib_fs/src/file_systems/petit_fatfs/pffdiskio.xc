/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "pffdiskio.h"


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS pff_disk_initialize() {
	DSTATUS stat;

	// Put your code here

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

/**
 *  \param buff   Pointer to the destination object
 *  \param sector Sector number (LBA)
 *  \param offset Offset in the sector
 *  \param count  Byte count (bit15:destination)
 */
DRESULT pff_disk_readp(BYTE* buff, DWORD sector, UINT offset, UINT count) {
	DRESULT res;

	// Put your code here

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

/**
 *  \param buff Pointer to the data to be written, NULL:Initiate/Finalize write operation
 *  \param sc   Sector number (LBA) or Number of bytes to send
 */
DRESULT pff_disk_writep(const BYTE* buff, DWORD sc) {
	DRESULT res;


	if (!buff) {
		if (sc) {

			// Initiate write process

		} else {

			// Finalize write process

		}
	} else {

		// Send data to the disk

	}

	return res;
}


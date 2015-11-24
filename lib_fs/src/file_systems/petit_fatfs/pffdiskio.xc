/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "pffdiskio.h"
#include "fs.h"

extern unsafe client interface fs_storage_media_if i_media;

DSTATUS pff_disk_initialize() {
  DSTATUS stat;

  unsafe {
    stat = i_media.initialise();
  }
  return stat;
}

/**
 *  \param buff   Pointer to the destination object
 *  \param sector Sector number (LBA)
 *  \param offset Offset in the sector
 *  \param count  Byte count (bit15:destination)
 */
DRESULT pff_disk_readp(BYTE* buff, DWORD sector, UINT offset, UINT count) {
  DRESULT res;

  unsafe {
    res = i_media.read(buff, sector, offset, count);
  }
  return res;
}

uint32_t sector_to_write = -1;

/**
 *  \param buff Pointer to the data to be written, NULL:Initiate/Finalize write operation
 *  \param sc   Sector number (LBA) or Number of bytes to send
 */
DRESULT pff_disk_writep(const BYTE* buff, DWORD sc) {
  DRESULT res;

  if (!buff) {
    if (sc) {
      // Initiate write process
      sector_to_write = sc;
    } else {
      // Finalize write process
      sector_to_write = -1;
    }
  } else {
    // Send data to the disk
    unsafe {
      res = i_media.write(buff, sector_to_write, sc);
    }
    sector_to_write++;
  }
  return res;
}

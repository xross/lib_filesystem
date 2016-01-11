/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/

// This implementation allows FatFs to use a file as a disk image.

#include "pffdiskio.h"
#include <stdio.h>

#ifndef BYTES_PER_SECTOR
#error BYTES_PER_SECTOR must be defined!
#endif

#ifndef SECTOR_COUNT
#error SECTOR_COUNT must be defined!
#endif

#ifndef BLOCK_SIZE
#error BLOCK_SIZE must be defined!
#endif

#ifndef DISKIO_INFO
#define DISKIO_INFO 0
#endif

#if DISKIO_INFO
#define DISKIO_I_PRINTF(...) printf("  ");printf(__VA_ARGS__)
#else
#define DISKIO_I_PRINTF(...)
#endif

#ifndef DISKIO_DEBUG
#define DISKIO_DEBUG 0
#endif

#if DISKIO_DEBUG
#define DISKIO_D_PRINTF(...) printf("    ");printf(__VA_ARGS__)
#else
#define DISKIO_D_PRINTF(...)
#endif

// The following globals are declared in make_fat.c
extern FILE *disk_image;
extern char *disk_image_filename;

DSTATUS pff_disk_initialize() {
  // Create the output file truncating to zero if it already exists
  if (!disk_image) { // Check if the disk_image is already open
    if (_USE_WRITE) {
      disk_image = fopen(disk_image_filename, "r+");
    } else
      disk_image = fopen(disk_image_filename, "r");
    }
  }
  if (disk_image) {
    DISKIO_I_PRINTF("Disk initialised successfully\n");
    DISKIO_D_PRINTF("disk_initialize() returning %d\n", RES_OK);
    return RES_OK;
  }
  DISKIO_D_PRINTF("Unable to create or open file for disk image!\n");
  DISKIO_D_PRINTF("disk_initialize() returning %d\n", STA_NOINIT);
  return STA_NOINIT;
}

/**
 *  \param buff   Pointer to the destination object
 *  \param sector Sector number (LBA)
 *  \param offset Offset in the sector
 *  \param count  Byte count (bit15:destination)
 */
DRESULT pff_disk_readp(BYTE* buff, DWORD sector, UINT offset, UINT count) {
  DISKIO_I_PRINTF("Reading %d sector(s), starting from %u\n", count, sector);

  // Move to the required position in the file
  if (fseek(disk_image, (sector * BYTES_PER_SECTOR) + offset, SEEK_SET)) {
    DISKIO_D_PRINTF("Failed to seek to required position in disk image!\n");
    DISKIO_D_PRINTF("disk_read() returning %d\n", RES_PARERR);
    return RES_PARERR;
  }
  // Read the data from the file
  size_t bytes_read = fread(buff, sizeof(BYTE), (count * BYTES_PER_SECTOR),
                            disk_image);
  if ((count * BYTES_PER_SECTOR) != bytes_read) {
    DISKIO_D_PRINTF("Failed to read from disk image, only read %zd byte(s)!\n",
                    bytes_read);
    DISKIO_D_PRINTF("disk_read() returning %d\n", RES_PARERR);
    return RES_PARERR;
  }
  DISKIO_D_PRINTF("disk_read() returning %d having read %zd bytes\n",
                  RES_OK, bytes_read);
  return RES_OK;
}

#if 0
#if _USE_WRITE
// TODO: Complete file based implementation of pff_disk_writep()
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
    // Write the data to the file
    DISKIO_I_PRINTF("Writing %d sector(s)\n", sc);
    size_t bytes_written = fwrite(buff, sizeof(BYTE),
                                    (count * BYTES_PER_SECTOR), disk_image);
    sector_to_write++;
  }
  return res;
}
#endif
#endif

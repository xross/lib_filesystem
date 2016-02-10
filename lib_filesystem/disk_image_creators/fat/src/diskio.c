/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/

// This implementation allows FatFs to use a file as a disk image.

#include "ffdiskio.h"
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

/**
 *  \param pdrv Physical drive number to identify the drive
 */
DSTATUS ff_disk_status(BYTE pdrv) {
  switch (pdrv) {
    case DISK_IMAGE:
      // Check the file handle is pointing to something
      if (disk_image) {
        DISKIO_I_PRINTF("Status for disk %d read successfully\n", pdrv);
        DISKIO_D_PRINTF("disk_status() returning %d\n", RES_OK);
        return RES_OK;
      }
      DISKIO_D_PRINTF("disk_image file handle is NULL in disk_status()!\n");
      DISKIO_D_PRINTF("disk_status() returning %d\n", STA_NOINIT);
      return STA_NOINIT;

    default:
      DISKIO_D_PRINTF("Unrecognised physical drive number passed to disk_status()!\n");
      break;
  }
  DISKIO_D_PRINTF("disk_status() returning %d\n", STA_NOINIT);
  return STA_NOINIT;
}

/**
 *  \param pdrv Physical drive number to identify the drive
 */
DSTATUS ff_disk_initialize(BYTE pdrv) {
  switch (pdrv) {
    case DISK_IMAGE:
      // Create the output file truncating to zero if it already exists
      if (!disk_image) { // Check if the disk_image is already open
        disk_image = fopen(disk_image_filename, "w+");
      }
      if (disk_image) {
        DISKIO_I_PRINTF("Disk %d initialised successfully\n", pdrv);
        DISKIO_D_PRINTF("disk_initialize() returning %d\n", RES_OK);
        return RES_OK;
      }
      DISKIO_D_PRINTF("Unable to create or open file for disk image!\n");
      DISKIO_D_PRINTF("disk_initialize() returning %d\n", STA_NOINIT);
      return STA_NOINIT;

    default:
      DISKIO_D_PRINTF("Unrecognised physical drive number passed to disk_initialize()!\n");
      break;
  }
  DISKIO_D_PRINTF("disk_initialize() returning %d\n", STA_NOINIT);
  return STA_NOINIT;
}

/**
 *  \param pdrv   Physical drive number to identify the drive
 *  \param buff   Data buffer to store read data
 *  \param sector Sector address in LBA
 *  \param count  Number of sectors to read
 */
DRESULT ff_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
  DISKIO_I_PRINTF("Reading %d sector(s), starting from %lu\n", count, sector);
  switch (pdrv) {
    case DISK_IMAGE:
      // Move to the required position in the file
      if (fseek(disk_image, (sector * BYTES_PER_SECTOR), SEEK_SET)) {
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

    default:
      DISKIO_D_PRINTF("Unrecognised physical drive number passed to disk_read()!\n");
      break;
  }
  DISKIO_D_PRINTF("disk_read() returning %d\n", RES_PARERR);
  return RES_PARERR;
}

#if _USE_WRITE
/**
 *  \param pdrv   Physical drive number to identify the drive
 *  \param buff   Data to be written
 *  \param sector Sector address in LBA
 *  \param count  Number of sectors to write
 */
DRESULT ff_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
  DISKIO_I_PRINTF("Writing %d sector(s), starting from %lu\n", count, sector);
  switch (pdrv) {
    case DISK_IMAGE:
      // Move to the required position in the file
      if (fseek(disk_image, (sector * BYTES_PER_SECTOR), SEEK_SET)) {
        DISKIO_D_PRINTF("Failed to seek to required position in disk image!\n");
        DISKIO_D_PRINTF("disk_write() returning %d\n", RES_PARERR);
        return RES_PARERR;
      }
      // Write the data to the file
      size_t bytes_written = fwrite(buff, sizeof(BYTE),
                                    (count * BYTES_PER_SECTOR), disk_image);
      if ((count * BYTES_PER_SECTOR) != bytes_written) {
        DISKIO_D_PRINTF("Failed to write to disk image, only wrote %zd byte(s)!\n",
                        bytes_written);
        DISKIO_D_PRINTF("disk_write() returning %d\n", RES_PARERR);
        return RES_PARERR;
      }
      DISKIO_D_PRINTF("disk_write() returning %d having written %zd bytes\n",
                      RES_OK, bytes_written);
      return RES_OK;

    default:
      DISKIO_D_PRINTF("Unrecognised physical drive number passed to disk_write()!\n");
      break;
  }
  DISKIO_D_PRINTF("disk_write() returning %d\n", RES_PARERR);
  return RES_PARERR;
}
#endif

#if _USE_IOCTL
/**
 *  \param pdrv Physical drive number to identify the drive
 *  \param cmd  Control code
 *  \param buff Buffer to send/receive control data
 */
DRESULT ff_disk_ioctl(BYTE pdrv, BYTE cmd, WORD *buff) {
  switch (pdrv) {
    case DISK_IMAGE:
      switch (cmd) {
        case CTRL_SYNC:
          // Complete pending write process (needed at _FS_READONLY == 0)
          if (fflush(disk_image)) {
            DISKIO_D_PRINTF("Failed to flush to disk image!\n");
            DISKIO_D_PRINTF("disk_ioctl() returning %d from CTRL_SYNC\n",
                            RES_PARERR);
            return RES_PARERR;
          }
          DISKIO_I_PRINTF("Flushed to disk successfully\n");
          DISKIO_D_PRINTF("disk_ioctl() returning %d from CTRL_SYNC\n", RES_OK);
          return RES_OK;

        case GET_SECTOR_COUNT:
          // Get media size (needed at _USE_MKFS == 1)
          *(DWORD*)buff = SECTOR_COUNT;
          DISKIO_I_PRINTF("Getting sector count\n");
          DISKIO_D_PRINTF("disk_ioctl() returning %d from GET_SECTOR_COUNT\n",
                          RES_OK);
          return RES_OK;

        case GET_SECTOR_SIZE:
          // Get sector size (needed at _MAX_SS != _MIN_SS)
          *(WORD*)buff = BYTES_PER_SECTOR;
          DISKIO_I_PRINTF("Getting sector size\n");
          DISKIO_D_PRINTF("disk_ioctl() returning %d from GET_SECTOR_SIZE\n",
                          RES_OK);
          return RES_OK;

        case GET_BLOCK_SIZE:
          // Get erase block size (needed at _USE_MKFS == 1)
          *(DWORD*)buff = BLOCK_SIZE;
          DISKIO_I_PRINTF("Getting block size\n");
          DISKIO_D_PRINTF("disk_ioctl() returning %d from GET_BLOCK_SIZE\n",
                          RES_OK);
          return RES_OK;

// NOTE: Erase support not implemented
#if 0
        case CTRL_TRIM:
          /* Inform device that the data on the block of sectors is no longer
           * used (needed at _USE_TRIM == 1)
           */
          return RES_OK;
#endif

        default:
          DISKIO_D_PRINTF("Unrecognised control code passed to disk_ioctl()!\n");
          break;
      }
      DISKIO_D_PRINTF("disk_ioctl() returning %d\n", RES_PARERR);
      return RES_PARERR;

    default:
        DISKIO_D_PRINTF("Unrecognised physical drive number passed to disk_ioctl()!\n");
        break;
  }
  DISKIO_D_PRINTF("disk_ioctl() returning %d\n", RES_PARERR);
  return RES_PARERR;
}
#endif

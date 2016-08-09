// Copyright (c) 2015-2016, XMOS Ltd, All rights reserved
#ifndef __filesystem_h__
#define __filesystem_h__

#include <stdint.h>
#include <stddef.h>

#ifndef MAX_ARRAY_SIZE
/** TODO: document */
// FIXME: prepend define name with "FS_"
#define MAX_ARRAY_SIZE 512
#endif

/** TODO: document */
typedef enum {
  FS_FORMAT_FAT12, ///< TODO: document
  FS_FORMAT_FAT16, ///< TODO: document
  FS_FORMAT_FAT32  ///< TODO: document
} fs_format_t;

/** TODO: document */
typedef enum {
  FS_MEDIA_SPI_FLASH,  ///< TODO: document
  FS_MEDIA_QSPI_FLASH, ///< TODO: document
  FS_MEDIA_SDCARD      ///< TODO: document
} fs_media_t;

/* XXX: Add as replacement for DSTATUS
    #define STA_NOINIT   0x01  // Drive not initialized
    #define STA_NODISK   0x02  // No medium in the drive
    #define STA_PROTECT  0x04  // Write protected
   */
/** TODO: document */
typedef enum {
  FS_MED_OK,             ///< TODO: document
  FS_MED_NOT_INIT,       ///< TODO: document
  FS_MED_NO_DISK,        ///< TODO: document
  FS_MED_WRITE_PROTECTED ///< TODO: document
} fs_media_status_t;

/** TODO: document */
typedef enum {
  FS_RES_OK,           ///< TODO: document
  FS_RES_DISK_ERR,     ///< TODO: document
  FS_RES_NOT_READY,    ///< TODO: document
  FS_RES_NO_FILE,      ///< TODO: document
  FS_RES_NOT_OPENED,   ///< TODO: document
  FS_RES_NOT_ENABLED,  ///< TODO: document
  FS_RES_NO_FILESYSTEM ///< TODO: document
} fs_result_t;

/** TODO: document */
typedef interface fs_storage_media_if {

  /** TODO: document */
  fs_media_status_t initialise();

  /** TODO: document */
  fs_media_status_t status();

  /** TODO: document */
  fs_result_t read(uint8_t *buf, uint32_t sector, uint32_t offset,
                   uint32_t count);

  /** TODO: document */
  fs_result_t write(const uint8_t *buf, uint32_t sector, uint32_t count);

  /** TODO: document */
  fs_result_t ioctl(uint8_t cmd, uint8_t *buf);

} fs_storage_media_if;

/** TODO: document */
typedef interface fs_basic_if {

  /** TODO: document */
  fs_result_t mount();

  /** TODO: document */
  fs_result_t open(char path[n], size_t n);

  /** TODO: document */
  fs_result_t read(uint8_t buf[n], size_t n, size_t bytes_to_read,
                   size_t &num_bytes_read);

  /** TODO: document */
  fs_result_t seek(size_t offset, int seek_from_sof);

  /** TODO: document */
  fs_result_t size(size_t &filesize_bytes);

} fs_basic_if;

#if 0
extends client interface fs_basic_if : {

  /** TODO: document */
  inline fs_result_t write() {
    return FS_RES_OK;
  }

  /** TODO: document */
  inline fs_result_t open_dir() {
    return FS_RES_OK;
  }

  /** TODO: document */
  inline fs_result_t read_dir() {
    return FS_RES_OK;
  }

}
#endif

/** TODO: document */
[[distributable]]
void filesystem_basic(server interface fs_basic_if i_fs[n_fs_clients],
                      size_t n_fs_clients,
                      fs_format_t fs_format,
                      client interface fs_storage_media_if i_storage_media);

/** TODO: document */
typedef interface fs_if {

  /* file access
    open
    close
    read
    write
    seek
    truncate
    sync
    forward
    gets
    putc
    puts
    printf
    tell
    eof
    size
    error
  */
  /** TODO: document */
  fs_result_t mount();

  /** TODO: document */
  fs_result_t open(char path[n], size_t n);

  /** TODO: document */
  fs_result_t read(uint8_t buf[n], size_t n, size_t bytes_to_read,
                   size_t &num_bytes_read);

  /** TODO: document */
  fs_result_t seek(size_t offset, int seek_from_sof);

  /** TODO: document */
  fs_result_t size(size_t &filesize_bytes);

  /* directory access
    open_dir
    close_dir
    read_dir
    find_first
    find_next
  */

  /* file/directory management
    stat
    unlink
    rename
    chmod
    utime
    mkdir
    chdir
    chdrive
    getcwd
  */

  /* volume management
    mount
    mkfs
    fdisk
    get_free
    get_label
    set_label
  */

} fs_if;

/** TODO: document */
[[distributable]]
void filesystem(server interface fs_if i_fs[n_fs_clients],
                size_t n_fs_clients,
                fs_format_t fs_format,
                client interface fs_storage_media_if i_storage_media);

#endif // __filesystem_h__

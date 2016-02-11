// Copyright (c) 2016, XMOS Ltd, All rights reserved
#include "filesystem.h"
#include "qspi_flash_storage_media.h"
#include <quadflash.h>
#include <QuadSpecMacros.h>
#include <platform.h>
#include "debug_print.h"
#include "xassert.h"
#include <stdlib.h>
#include <string.h> // Included for memset

#define BUFFER_SIZE      50
#define PARTIAL_READ_LEN 5
#define BUFFER_PATTERN   0xAAAAAAAA

fl_QSPIPorts qspi_flash_ports = {
  PORT_SQI_CS,
  PORT_SQI_SCLK,
  PORT_SQI_SIO,
  on tile[0]: XS1_CLKBLK_1
};

void application(client interface fs_basic_if i_fs) {
  fs_result_t result;

  debug_printf("Mounting filesystem...\n");
  result = i_fs.mount();
  if (result != FS_RES_OK) {
    debug_printf("result = %d\n", result);
    exit(1);
  }

  debug_printf("Opening file...\n");
  result = i_fs.open("HELLO.TXT");
  if (result != FS_RES_OK) {
    debug_printf("result = %d\n", result);
    exit(1);
  }

  debug_printf("Getting file size...\n");
  size_t file_size;
  result = i_fs.size(file_size);
  if (result != FS_RES_OK) {
    debug_printf("result = %d\n", result);
    exit(1);
  }

  uint8_t buf[BUFFER_SIZE];

  debug_printf("Reading part of file...\n");
  xassert((file_size > PARTIAL_READ_LEN)
          && msg("Partial read length exceeds file size!\n"));
  size_t bytes_to_read = 0;
  size_t num_bytes_read = 0;
  // Init buffer with pattern
  memset(buf, BUFFER_PATTERN, BUFFER_SIZE);
  bytes_to_read = PARTIAL_READ_LEN;
  result = i_fs.read(buf, bytes_to_read, &num_bytes_read);
  if (result != FS_RES_OK) {
    debug_printf("result = %d\n", result);
    exit(1);
  }

  debug_printf("Attempted to read %d byte(s) of %d byte file.\n"
               "Read %d byte(s) of file:\n",
               bytes_to_read, file_size, num_bytes_read);
  for (int i = 0; i < num_bytes_read; i++) {
    debug_printf("%c", buf[i]);
  }
  debug_printf("\n");
  if (bytes_to_read != num_bytes_read) {
    exit(1);
  }
  // Check end of buffer hasn't been overwritten
  for (int i = bytes_to_read; i < BUFFER_SIZE; i++) {
    if (buf[i] != (uint8_t)BUFFER_PATTERN) {
      debug_printf("Unexpected write to buffer at index %d, "
                   "found %x, expected %x!\n",
                   i, buf[i], (uint8_t)BUFFER_PATTERN);
    }
  }

  debug_printf("Seeking back to beginning of file...\n");
  result = i_fs.seek(0, 1);
  if (result != FS_RES_OK) {
    debug_printf("result = %d\n", result);
    exit(1);
  }

  debug_printf("Reading whole file...\n");
  xassert((file_size < BUFFER_SIZE)
          && msg("Buffer not large enough to read whole file in to!\n"));
  bytes_to_read = 0;
  num_bytes_read = 0;
  // Init buffer with pattern
  memset(buf, BUFFER_PATTERN, BUFFER_SIZE);
  result = i_fs.read(buf, file_size, &num_bytes_read);
  if (result != FS_RES_OK) {
    debug_printf("result = %d\n", result);
    exit(1);
  }

  debug_printf("Attempted to read all of %d byte file.\n"
               "Read %d byte(s) of file:\n",
               file_size, num_bytes_read);
  for (int i = 0; i < num_bytes_read; i++) {
    debug_printf("%c", buf[i]);
  }
  debug_printf("\n");
  if (file_size != num_bytes_read) {
    exit(1);
  }
  // Check end of buffer hasn't been overwritten
  for (int i = file_size; i < BUFFER_SIZE; i++) {
    if (buf[i] != (uint8_t)BUFFER_PATTERN) {
      debug_printf("Unexpected write to buffer at index %d, "
                   "found %x, expected %x!\n",
                   i, buf[i], (uint8_t)BUFFER_PATTERN);
    }
  }

  exit(0);
}

int main(void) {
  interface fs_basic_if i_fs[1];
  interface fs_storage_media_if i_media;
  fl_QuadDeviceSpec qspi_spec = FL_QUADDEVICE_SPANSION_S25FL116K;

  // TODO: must ensure the correct disk image for this test has been written to the flash

  par {
    qspi_flash_fs_media(i_media, qspi_flash_ports, qspi_spec, 512);
    filesystem_basic(i_fs, 1, FS_FORMAT_FAT12, i_media);
    application(i_fs[0]);
  }

  return 0;
}

#include "filesystem.h"
#include "qspi_flash_storage_media.h"
#include <quadflash.h>
#include <QuadSpecMacros.h>
#include <platform.h>
#include "debug_print.h"
#include <stdlib.h>

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
  result = i_fs.open("SMALL.TXT");
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

  debug_printf("Reading file...\n");
  size_t bytes_to_read;
  size_t num_bytes_read;
  uint8_t buf[1024];
  if (file_size > 1024) {
    bytes_to_read = 1024;
  } else {
    bytes_to_read = file_size;
  }
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

  exit(0);
}

int main(void) {
  interface fs_basic_if i_fs[1];
  interface fs_storage_media_if i_media;
  fl_QuadDeviceSpec qspi_spec = FL_QUADDEVICE_SPANSION_S25FL116K;

  par {
    qspi_flash_fs_media(i_media, qspi_flash_ports, qspi_spec, 512);
    filesystem_basic(i_fs, 1, FS_FORMAT_FAT12, i_media);
    application(i_fs[0]);
  }

  return 0;
}

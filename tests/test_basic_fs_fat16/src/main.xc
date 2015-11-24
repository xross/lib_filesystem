#include "fs.h"
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

  debug_printf("Getting filesize...\n");
  size_t file_size;
  // result = i_fs.size(file_size);
  if (result != FS_RES_OK) {
    debug_printf("result = %d\n", result);
    exit(1);
  }

  debug_printf("Reading file...\n");
  uint8_t buf[20];
  size_t num_bytes_read;
  result = i_fs.read(buf, 20, &num_bytes_read);
  if (result != FS_RES_OK) {
    debug_printf("result = %d\n", result);
    exit(1);
  }

  debug_printf("num_bytes_read = %d\nbuf=%s\n", num_bytes_read, buf);
  exit(0);
}

int main(void) {
  interface fs_basic_if i_fs[1];
  interface fs_storage_media_if i_media;
  fl_QuadDeviceSpec qspi_spec = FL_QUADDEVICE_SPANSION_S25FL116K;
  // static const size_t sector_size = (size_t)qspi_spec.sectorSizes.regularSectorSize;

  par {
    qspi_flash_fs_media(i_media, qspi_flash_ports, qspi_spec, 4096);
    fs_basic(i_fs, 1, FS_FORMAT_FAT16, i_media);
    application(i_fs[0]);
  }

  return 0;
}

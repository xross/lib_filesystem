// Copyright (c) 2015-2016, XMOS Ltd, All rights reserved
#include "filesystem.h"
#include "qspi_flash_storage_media.h"
#include <quadflash.h>
#include <QuadSpecMacros.h>
#include <platform.h>

fl_QSPIPorts qspi_flash_ports = {
  PORT_SQI_CS,
  PORT_SQI_SCLK,
  PORT_SQI_SIO,
  on tile[0]: XS1_CLKBLK_1
};

void application(client interface fs_if i_fs) {
  i_fs.mount();
  // TODO: add full set of tests
}

int main(void) {
  interface fs_if i_fs[1];
  interface fs_storage_media_if i_media;
  fl_QuadDeviceSpec qspi_spec = FL_QUADDEVICE_SPANSION_S25FL116K;

  // TODO: must ensure the correct disk image for this test has been written to the flash

  par {
    qspi_flash_fs_media(i_media, qspi_flash_ports, qspi_spec, 4096);
    filesystem(i_fs, 1, FS_FORMAT_FAT16, i_media);
    application(i_fs[0]);
  }

  return 0;
}

#include "fs.h"
#include "xassert.h"
#include <quadflash.h>
#include <stddef.h>
#include <xs1.h>

[[distributable]]
void qspi_flash_fs_media(server interface fs_storage_media_if i_media,
                         fl_QSPIPorts &qspi_flash_ports,
                         const fl_QuadDeviceSpec qspi_spec,
                         static const size_t sector_size) {
  xassert((qspi_spec.sectorLayout == SECTOR_LAYOUT_REGULAR)
          && msg("Only QSPI flash with regular sector layout are supported\n"));

  while(1) {
    select {
      case i_media.initialise() -> fs_media_status_t status:
        if (fl_connectToDevice(qspi_flash_ports, &qspi_spec, 1)) {
          status = FS_MED_NOT_INIT;
        } else {
          status = FS_MED_OK;
        }
        break;

      case i_media.status() -> fs_media_status_t status:
        // libflash does not provide status information
        status = FS_MED_OK;
        break;

      case i_media.read(uint8_t *buf, uint32_t sector, uint32_t count)
                          -> fs_result_t result:
        if (fl_readData(sector * sector_size, count, buf)) {
          result = FS_RES_DISK_ERR;
        } else {
          result = FS_RES_OK;
        }
        break;

      case i_media.write(const uint8_t *buf, uint32_t sector, uint32_t count)
                           -> fs_result_t result:
        uint8_t scratch[sector_size];
        if (fl_writeData(sector * sector_size, count, buf, scratch)) {
          result = FS_RES_DISK_ERR;
        } else {
          result = FS_RES_OK;
        }
        break;

      case i_media.ioctl(uint8_t cmd, uint8_t *buf) -> fs_result_t result:
        // Not handling commands on QSPI flash
        result = FS_RES_OK;
        break;
    }
  }
}

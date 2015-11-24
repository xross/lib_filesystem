#include "fs.h"
#include "xassert.h"
#include "pff.h"

unsafe client interface fs_storage_media_if i_media;

[[distributable]]
void fs_basic(server interface fs_basic_if i_fs[n_fs_clients],
              size_t n_fs_clients,
              fs_format_t fs_format,
              client interface fs_storage_media_if i_storage_media) {
  xassert(((fs_format == FS_FORMAT_FAT12) ||
           (fs_format == FS_FORMAT_FAT16) ||
           (fs_format == FS_FORMAT_FAT32))
          && msg("fs_basic only supports FAT16 and FAT32\n"));

  FATFS fatfs;

  unsafe {
    i_media = i_storage_media;
  }

  while (1) {
    select {
      case i_fs[int i].mount() -> fs_result_t result:
        result = (fs_result_t)pf_mount(&fatfs);
        break;

      case i_fs[int i].open(const char *path) -> fs_result_t result:
        result = pf_open(path);
        break;

      case i_fs[int i].read(uint8_t *buf, size_t bytes_to_read,
                            size_t *num_bytes_read) -> fs_result_t result:
        result = pf_read(buf, (UINT)num_bytes_read, num_bytes_read);
        break;

      case i_fs[int i].seek(size_t offset, int seek_from_sof)
                              -> fs_result_t result:
        DWORD ofs = offset;
        if (seek_from_sof) {
          ofs += fatfs.fptr;
        }
        result = pf_lseek(ofs);
        break;

      case i_fs[int i].size(size_t &filesize_bytes) -> fs_result_t result:

        break;
    }
  }
}

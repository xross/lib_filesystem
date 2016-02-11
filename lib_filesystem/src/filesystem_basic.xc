// Copyright (c) 2016, XMOS Ltd, All rights reserved
#include "filesystem.h"
#include "xassert.h"
#include "pff.h"
#include <string.h>

unsafe client interface fs_storage_media_if i_media;

[[distributable]]
void filesystem_basic(server interface fs_basic_if i_fs[n_fs_clients],
                      size_t n_fs_clients,
                      fs_format_t fs_format,
                      client interface fs_storage_media_if i_storage_media) {
  xassert(((fs_format == FS_FORMAT_FAT12) ||
           (fs_format == FS_FORMAT_FAT16) ||
           (fs_format == FS_FORMAT_FAT32))
          && msg("fs_basic only supports FAT\n"));

  FATFS fatfs;

  unsafe {
    i_media = i_storage_media;
  }

  while (1) {
    select {
      case i_fs[int i].mount() -> fs_result_t result:
        result = (fs_result_t)pf_mount(&fatfs);
        break;

      case i_fs[int i].open(char path[n], size_t n) -> fs_result_t result:
        // Copy the remote data to a local array
        char local_path[MAX_ARRAY_SIZE];
        xassert((n <= MAX_ARRAY_SIZE)
                && msg("Length of path exceeds MAX_ARRAY_SIZE\n"));
        memcpy(local_path, path, n*sizeof(char));
        result = pf_open(local_path);
        break;

      case i_fs[int i].read(uint8_t buf[n], size_t n,
                            size_t bytes_to_read,
                            size_t &num_bytes_read) -> fs_result_t result:
        uint8_t local_buf[MAX_ARRAY_SIZE];
        xassert((n <= MAX_ARRAY_SIZE)
                && msg("Length of buf exceeds MAX_ARRAY_SIZE\n"));
        result = pf_read(local_buf, (UINT)bytes_to_read, (UINT*)&num_bytes_read);
        // Copy the local data to the remote array
        memcpy(buf, local_buf, n*sizeof(uint8_t));
        break;

      case i_fs[int i].seek(size_t offset, int seek_from_sof)
                              -> fs_result_t result:
        DWORD ofs = offset;
        if (!seek_from_sof) {
          ofs += fatfs.fptr;
        }
        result = pf_lseek(ofs);
        break;

      case i_fs[int i].size(size_t &filesize_bytes) -> fs_result_t result:
        if (FA_OPENED == fatfs.flag) {
          filesize_bytes = fatfs.fsize;
          result = FS_RES_OK;
        } else {
          result = FS_RES_NOT_OPENED;
        }
        break;
    }
  }
}

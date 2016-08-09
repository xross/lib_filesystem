// Copyright (c) 2015-2016, XMOS Ltd, All rights reserved
#include "ff.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *disk_image = NULL;
const char *disk_image_filename = NULL;

int main(int argc, char const *argv[]) {
  /* arg1 : path to output binary (will create file)
   * arg2+: path to file to write into new filesystem (optional)
   */

  // Sanity check the arguments
  if (argc < 2) {
    printf("Must supply name for output file!\n");
    return 1;
  }

  /* Set the global filename for the disk image, it will be used by the disk IO
   * functions.
   */
  disk_image_filename = argv[1];

  // Set up the root path for the filesystem in the disk image
  TCHAR disk_image_root_path[4] = { '0'+DISK_IMAGE, ':', '/', '\0' };

  FATFS fs;
  unsigned int result = 0;

  /* Call f_mount() to create the output file. If the file already exists it
   * will be truncated to zero bytes.
   * The initialisation option is set 0 such that FatFs does not yet attempt to
   * mount a filesystem from the drive.
   */
  printf("Creating disk image...\n");
  result = f_mount(&fs, disk_image_root_path, 0);
  if (result) {
    printf("->f_mount() failed with result code %d\n", result);
    return 1;
  }

  /* Create a FAT filesystem in the disk image using the SFD partitioning rule
   * such that the FAT volume starts from the start of the drive, and does not
   * create a MBR or partition table.
   * The allocation unit argument is set to 0 such that the cluster size is set
   * automatically based on the volume size
   */
  printf("Creating FAT filesystem...\n");
  result = f_mkfs (disk_image_root_path, 1, 0);
  if (result) {
    printf("->f_mkfs() failed with result code %d\n", result);
    return 1;
  }

  /* Now that the filesystem has been created we can call f_mount() with the
   * initialisation option is set 1 such that FatFs attempts to mount the FAT
   * volume from the drive.
   */
  printf("Mounting filesystem...\n");
  result = f_mount(&fs, disk_image_root_path, 1);
  if (result) {
    printf("->f_mount() failed with result code %d\n", result);
    return 1;
  }

  // Write any files passed as arguments into the new filesystem
  for (int i = 2; i < argc; i++) {
    const char *path_on_host = argv[i];
    FIL file_on_disk_image;

    // Try to extract the filename from a Unix path
    const char *extracted_filename = strrchr(path_on_host, '/');
    if (!extracted_filename) { // No Unix path separators found
      // Try to extract the filename from a Windows path
      extracted_filename = strrchr(path_on_host, '\\');
    }
    if (!extracted_filename) { // No path separators found
      // File must be in same directory as this executable
      extracted_filename = path_on_host;
    } else { // Found a path separator in the path
      // Jump forward 1 char to get just the filename
      extracted_filename = extracted_filename+sizeof(char);
    }

    TCHAR path_on_disk_image[strlen(disk_image_root_path) +
                             strlen(extracted_filename)];
    strcpy(path_on_disk_image, disk_image_root_path);
    strcat(path_on_disk_image, extracted_filename);

    printf("Writing %s to filesystem as %s...\n", path_on_host,
           path_on_disk_image);

    result = f_open(&file_on_disk_image, path_on_disk_image,
                    FA_CREATE_NEW | FA_WRITE);
    if (result) {
      printf("->f_open() failed with the result code %d\n", result);
      return 1;
    }

    // Open the file on the host
    FILE* file_on_host = fopen(argv[i], "r");
    if (!file_on_host) {
      printf("->Failed to open file %s\n", path_on_host);
      return 1;
    }

    // Get the size of the file
    if (fseek(file_on_host, 0, SEEK_END)) {
      printf("->Failed to seek to end of file %s\n", path_on_host);
      return 1;
    }
    long bytes_in_file = ftell(file_on_host);
    if (-1 == bytes_in_file) {
      printf("->Failed to get size of file %s\n", path_on_host);
      return 1;
    }

    // Go back to the start of the file
    if (fseek(file_on_host, 0, SEEK_SET)) {
      printf("->Failed to seek back to start of file %s\n", path_on_host);
      return 1;
    }

    // Allocate a buffer to hold file contents
    char *file_contents = calloc(bytes_in_file, sizeof(char));
    if (!file_contents) {
      printf("->Failed to allocate buffer for contents of file %s\n",
             path_on_host);
      return 1;
    }

    // Copy the file into memory
    if (fread(file_contents, sizeof(char), bytes_in_file, file_on_host) !=
        (size_t)bytes_in_file) {
      printf("->Failed to read contents into buffer for file %s\n",
             path_on_host);
      return 1;
    }

    // Close the file on the host
    if (fclose(file_on_host)) {
      printf("->Failed to close file %s\n", argv[i]);
      return 1;
    }

    // Write from memory into the disk image
    UINT bytes_written = 0;
    result = f_write(&file_on_disk_image, file_contents, bytes_in_file,
                     &bytes_written);
    if (result) {
      printf("->f_write() failed with the result code %d\n", result);
      return 1;
    }

    // Close the file on the disk image
    result = f_close(&file_on_disk_image);
    if (result) {
      printf("->f_close() failed with the result code %d\n", result);
      return 1;
    }

    // Free the buffer
    free(file_contents);
  }

  // Finally unmount the disk image
  printf("Unmounting filesystem...\n");
  result = f_mount(NULL, disk_image_root_path, 0);
  if (result) {
    printf("->f_mount() failed with result code %d\n", result);
    return 1;
  }
  printf("Disk image created successfully\n");
  return 0;
}

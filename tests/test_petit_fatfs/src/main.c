#include "pff.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *disk_image = NULL;
const char *disk_image_filename = NULL;

int main(int argc, char const *argv[]) {

  // Sanity check the arguments
  if (argc < 2) {
    printf("Must supply path to disk image!\n");
    return 1;
  }

  /* Set the global filename for the disk image, it will be used by the disk IO
   * functions.
   */
  disk_image_filename = argv[1];

  // Set up the root path for the filesystem in the disk image
  char disk_image_root_path[4] = { '0'+DISK_IMAGE, ':', '/', '\0' };

  FRESULT result;
  FATFS fs;

  printf("Mounting filesystem...\n");
  result = pf_mount(&fs);
  if (result != FR_OK) {
    printf("result = %d\n", result);
    exit(1);
  }

  printf("Opening file...\n");
  result = pf_open("SMALL.TXT");
  if (result != FR_OK) {
    printf("result = %d\n", result);
    exit(1);
  }

  printf("Reading file...\n");
  uint8_t buf[20];
  uint32_t num_bytes_read;
  result = pf_read(buf, 20, &num_bytes_read);
  if (result != FR_OK) {
    printf("result = %d\n", result);
    exit(1);
  }

  // TODO: test write functionality

  printf("num_bytes_read = %d\nbuf=%s\n", num_bytes_read, buf);

  return 0;
}

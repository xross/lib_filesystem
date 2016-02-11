BYTES_PER_SECTOR ?= 512
SECTOR_COUNT ?= 3444 # Same as high density 3.5" floppy disk
BLOCK_SIZE ?= 128 # Erase block size

CC = gcc
CFLAGS = -Wall -Wextra

FAT_DIR = ../../src/file_systems/fatfs
FAT_SRC = $(FAT_DIR)/ff.c

IMAGE_BUILDER_SRC = src/make_fat.c \
										src/diskio.c

APP_NAME = fat_image_creator

BUILD_OPTIONS = -DBYTES_PER_SECTOR=$(BYTES_PER_SECTOR) \
								-DSECTOR_COUNT=$(SECTOR_COUNT) \
								-DBLOCK_SIZE=$(BLOCK_SIZE) \
								-DDISK_IMAGE=0 \
								-DDISKIO_INFO=0 \
								-DDISKIO_DEBUG=0 \
								-DFATFS_INFO=1

all:
	$(CC) $(CFLAGS) -I$(FAT_DIR) $(FAT_SRC) $(IMAGE_BUILDER_SRC) $(BUILD_OPTIONS) -o $(APP_NAME)

.PHONY: clean

clean:
	rm -f $(APP_NAME)

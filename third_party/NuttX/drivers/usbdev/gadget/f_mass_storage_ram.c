/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: LiteOS USB Driver Device Mass Protocol About Memory Media.
 * Author: huangjieliang
 * Create: 2019-11-26
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 * --------------------------------------------------------------------------- */

#include <disk.h>
#include <driver.h>
#include "gadget/f_mass_storage_ram.h"
#include "user_copy.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SECTOR_SIZE   512
#define RAM_DEV_NAME  "/dev/uram"

struct ram_storage_sc
{
  uint8_t *start_addr;
  uint32_t len;
  uint32_t sector_size;
};

static int ram_open(struct inode *filep)
{
  (void)filep;
  return 0;
}

static int ram_close(struct inode *filep)
{
  (void)filep;
  return 0;
}

static ssize_t ram_read(struct inode *ram_inode, uint8_t *buffer,
                        uint64_t start_sector, uint32_t nsectors)
{
  errno_t status;
  struct ram_storage_sc *sc = ram_inode->i_private;
  uint32_t sector_size      = sc->sector_size;
  uint8_t *start_addr       = sc->start_addr;

  if (start_sector + nsectors > sc->len / sector_size)
    {
      usb_err("start_sector = %llu, nsectors = %u\n", start_sector, nsectors);
      return -1;
    }

  status = usbd_copy_to_user(buffer, nsectors * sector_size, start_addr +
                             start_sector * sector_size, nsectors * sector_size);
  if(status != EOK)
    {
      return -1;
    }

  return (ssize_t)nsectors;
}

static ssize_t ram_write(struct inode *ram_inode, const uint8_t *buffer,
                         uint64_t start_sector, uint32_t nsectors)
{
  errno_t status;
  struct ram_storage_sc *sc = ram_inode->i_private;
  uint32_t sector_size      = sc->sector_size;
  uint8_t *start_addr       = sc->start_addr;

  if (start_sector + nsectors > sc->len / sector_size)
    {
      usb_err("start_sector = %llu, nsectors = %u\n", start_sector, nsectors);
      return -1;
    }

  status = usbd_copy_from_user(start_addr + start_sector * sector_size,
                               nsectors * sector_size, buffer, nsectors * sector_size);
  if(status != EOK)
    {
      return -1;
    }

  return (ssize_t)nsectors;
}

static int ram_geometry(struct inode *ram_inode, struct geometry *ram_geometry)
{
  struct ram_storage_sc *sc;

  if (ram_geometry == NULL)
    {
      return -1;
    }

  sc = ram_inode->i_private;
  ram_geometry->geo_nsectors   = sc->len / sc->sector_size;
  ram_geometry->geo_sectorsize = sc->sector_size;

  return 0;
}

static int ram_ioctl(struct inode *ram_inode, int cmd, unsigned long arg)
{
  (void)ram_inode;
  (void)cmd;
  (void)arg;
  return 0;
}

const static struct block_operations g_ram_dev_ops =
{
  .open  = ram_open,
  .close = ram_close,
  .read  = ram_read,
  .write = ram_write,
  .geometry = ram_geometry,
  .ioctl    = ram_ioctl,
  .unlink   = NULL
};

int ram_mass_storage_init(void)
{
  void *fat32_part_buf;
  struct ram_storage_sc *sc;
  int32_t disk_id;
  int fd = -1;
  ssize_t ret;

  sc = malloc(sizeof(struct ram_storage_sc));
  if (sc == NULL)
    {
      return -1;
    }
  (void)memset_s(sc, sizeof(struct ram_storage_sc), 0, sizeof(struct ram_storage_sc));

  /* Request a memory block to store the fat32 partition, the length is 2M. */

  fat32_part_buf = memalign(SECTOR_SIZE, FAT_PART_SIZE);
  if (fat32_part_buf == NULL)
    {
      free(sc);
      return -1;
    }

  fd = open(FAT_PART_IMAGE_PATH, O_RDONLY, 0);
  if (fd < 0)
    {
      usb_err("open file failed!\n");
      goto error;
    }

  /* Copy Fat32 partition from flash to memory */

  ret = read(fd, fat32_part_buf, FAT_PART_SIZE);
  if (ret != FAT_PART_SIZE)
    {
      usb_err("read bytes is too small, ret = %d\n", ret);
      (void)close(fd);
      goto error;
    }
  (void)close(fd);

  sc->start_addr  = (uint8_t *)fat32_part_buf;
  sc->sector_size = SECTOR_SIZE;
  sc->len         = FAT_PART_SIZE;

  /* Register the fat32 partition device node (/dev/uramp0). */

  disk_id = los_alloc_diskid_byname(RAM_DEV_NAME);
  OsSetUsbStatus((uint32_t)disk_id);
  ret = los_disk_init(RAM_DEV_NAME, &g_ram_dev_ops, (void *)sc, disk_id, NULL);
  if (ret)
    {
      usb_err("los_disk_init fail!\n");
      goto error;
    }

  return 0;
error:
  free(sc);
  free(fat32_part_buf);
  return -1;
}

int ram_mass_storage_deinit(void)
{
  struct ram_storage_sc *sc;
  struct inode *inode;
  char *fullpath = NULL;
  int32_t disk_id;
  int ret;

  ret = vfs_normalize_path(NULL, RAM_DEV_NAME, &fullpath);
  if (ret < 0)
    {
      return -1;
    }

  inode = inode_find(fullpath, NULL);
  if (inode == NULL)
    {
      free(fullpath);
      return -1;
    }
  sc = inode->i_private;
  inode_release(inode);
  free(fullpath);

  disk_id = los_get_diskid_byname(RAM_DEV_NAME);
  (void)los_disk_deinit(disk_id);
  OsClearUsbStatus((uint32_t)disk_id);

  if (sc != NULL)
    {
      free(sc->start_addr);
      sc->start_addr = NULL;
      free(sc);
    }

  return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

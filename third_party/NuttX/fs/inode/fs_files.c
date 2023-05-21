/****************************************************************************
 * fs/inode/fs_files.c
 *
 *   Copyright (C) 2007-2009, 2011-2013, 2016-2017 Gregory Nutt. All rights
 *     reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "unistd.h"
#include "vfs_config.h"
#include "sys/types.h"
#include "string.h"
#include "dirent.h"
#include "semaphore.h"
#include "assert.h"
#include "errno.h"
#include "fs/file.h"
#include "stdio.h"
#include "stdlib.h"
#include "vnode.h"
#include "los_mux.h"
#include "fs/fd_table.h"
#ifdef LOSCFG_NET_LWIP_SACK
#include "lwip/sockets.h"
#endif
#include "los_process_pri.h"
#include "los_vm_filemap.h"
#include "mqueue.h"
#define ferr PRINTK

#if CONFIG_NFILE_DESCRIPTORS > 0
struct filelist tg_filelist;
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

struct filelist *sched_getfiles(void)
{
#if CONFIG_NFILE_DESCRIPTORS > 0
  return &tg_filelist;
#else
  return NULL;
#endif
}

/* 32: An unsigned int takes 32 bits */

static unsigned int bitmap[CONFIG_NFILE_DESCRIPTORS / 32 + 1] = {0};

static void set_bit(int i, void *addr)
{
  unsigned int tem = (unsigned int)i >> 5; /* Get the bitmap subscript */
  unsigned int *addri = (unsigned int *)addr + tem;
  unsigned int old = *addri;
  old = old | (1UL << ((unsigned int)i & 0x1f)); /* set the new map bit */
  *addri = old;
}

static void clear_bit(int i, void *addr)
{
  unsigned int tem = (unsigned int)i >> 5; /* Get the bitmap subscript */
  unsigned int *addri = (unsigned int *)addr + tem;
  unsigned int old = *addri;
  old = old & ~(1UL << ((unsigned int)i & 0x1f)); /* Clear the old map bit */
  *addri = old;
}

bool get_bit(int i)
{
  unsigned int *p = NULL;
  unsigned int mask;

  p = ((unsigned int *)bitmap) + (i >> 5); /* Gets the location in the bitmap */
  mask = 1 << (i & 0x1f); /* Gets the mask for the current bit int bitmap */
  if (!(~(*p) & mask)){
    return true;
  }
  return false;
}

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: _files_semtake
 ****************************************************************************/

static void _files_semtake(struct filelist *list)
{
  /* Take the semaphore (perhaps waiting) */

  while (sem_wait(&list->fl_sem) != 0)
    {
      /* The only case that an error should occur here is if the wait was
       * awakened by a signal.
       */

      LOS_ASSERT(get_errno() == EINTR);
    }
}

/****************************************************************************
 * Name: _files_semgive
 ****************************************************************************/

#define _files_semgive(list)  (void)sem_post(&list->fl_sem)

void file_hold(struct file *filep)
{
  struct filelist *list = sched_getfiles();
  if (!list)
    {
      return;
    }

  _files_semtake(list);
  if (filep != NULL)
    {
      assert(filep->f_refcount > 0);
      filep->f_refcount++;
    }
  _files_semgive(list);
}

void file_release(struct file *filep)
{
  struct filelist *list = sched_getfiles();
  if (!list)
    {
      return;
    }

  _files_semtake(list);
  if (filep != NULL)
    {
      assert(filep->f_refcount > 0);
      filep->f_refcount--;
    }
  _files_semgive(list);
}

/****************************************************************************
 * Name: _files_close
 *
 * Description:
 *   Close an vnode (if open)
 *
 * Assumuptions:
 *   Caller holds the list semaphore because the file descriptor will be freed.
 *
 ****************************************************************************/

static int _files_close(struct file *filep)
{
  struct Vnode *vnode = filep->f_vnode;
  int ret = OK;

  /* Check if the struct file is open (i.e., assigned an vnode) */
  if (filep->f_oflags & O_DIRECTORY)
    {
      ret = closedir(filep->f_dir);
      if (ret != OK)
        {
          return ret;
        }
    }
  else
    {
      /* Close the file, driver, or mountpoint. */
      if (filep->ops && filep->ops->close)
        {
          /* Perform the close operation */

          ret = filep->ops->close(filep);
          if (ret != OK)
            {
              return ret;
            }
        }
      VnodeHold();
      vnode->useCount--;
      /* Block char device is removed when close */
      if (vnode->type == VNODE_TYPE_BCHR)
        {
          ret = VnodeFree(vnode);
          if (ret < 0)
            {
              PRINTK("Removing bchar device %s failed\n", filep->f_path);
            }
        }
      VnodeDrop();
    }

  /* Release the file descriptor */

  memset(filep, 0, sizeof(struct file));
  filep->fd = -1;

  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: files_initialize
 *
 * Description:
 *   This is called from the FS initialization logic to configure the files.
 *
 ****************************************************************************/

void files_initialize(void)
{
}

/****************************************************************************
 * Name: files_initlist
 *
 * Description: Initializes the list of files for a new task
 *
 ****************************************************************************/

void files_initlist(struct filelist *list)
{
  DEBUGASSERT(list);

  /* Initialize the list access mutex */

  (void)sem_init(&list->fl_sem, 0, 1);
}

/****************************************************************************
 * Name: files_releaselist
 *
 * Description:
 *   Release a reference to the file list
 *
 ****************************************************************************/

void files_releaselist(struct filelist *list)
{
  int i;

  DEBUGASSERT(list);

  /* Close each file descriptor .. Normally, you would need take the list
   * semaphore, but it is safe to ignore the semaphore in this context because
   * there should not be any references in this context.
   */

  for (i = 0; i < CONFIG_NFILE_DESCRIPTORS; i++)
    {
      (void)_files_close(&list->fl_files[i]);
    }

  /* Destroy the semaphore */

  (void)sem_destroy(&list->fl_sem);
}

/****************************************************************************
 * Name: file_dup2
 *
 * Description:
 *   Assign an vnode to a specific files structure.  This is the heart of
 *   dup2.
 *
 *   Equivalent to the non-standard fs_dupfd2() function except that it
 *   accepts struct file instances instead of file descriptors and it does
 *   not set the errno variable.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is return on
 *   any failure.
 *
 ****************************************************************************/

int file_dup2(struct file *filep1, struct file *filep2)
{
  struct filelist *list = NULL;
  struct Vnode *vnode_ptr = NULL;
  int err;
  int ret;

  if (!filep1 || !filep1->f_vnode || !filep2)
    {
      err = -EBADF;
      goto errout;
    }

  list = sched_getfiles();
  DEBUGASSERT(list);
  /* The file list can be NULL under two cases:  (1) One is an obscure
   * cornercase:  When memory management debug output is enabled.  Then
   * there may be attempts to write to stdout from malloc before the group
   * data has been allocated.  The other other is (2) if this is a kernel
   * thread.  Kernel threads have no allocated file descriptors.
   */

  if (list != NULL)
    {
      _files_semtake(list);
    }

  /* If there is already an vnode contained in the new file structure,
   * close the file and release the vnode.
   */

  ret = _files_close(filep2);
  if (ret < 0)
    {
      /* An error occurred while closing the driver */

      goto errout_with_ret;
    }

  /* Increment the reference count on the contained vnode */

  vnode_ptr = filep1->f_vnode;

  /* Then clone the file structure */

  filep2->f_oflags = filep1->f_oflags;
  filep2->f_pos    = filep1->f_pos;
  filep2->f_vnode  = filep1->f_vnode;
  filep2->f_priv   = filep1->f_priv;
  filep2->f_path   = filep1->f_path;
  filep2->ops      = filep1->ops;
  filep2->f_refcount = filep1->f_refcount;

  /* Call the open method on the file, driver, mountpoint so that it
   * can maintain the correct open counts.
   */
  if (vnode_ptr->vop)
    {
      if (vnode_ptr->flag & VNODE_FLAG_MOUNT_NEW)
        {
            ret = -ENOSYS;
        }
      else
        {
          /* (Re-)open the pseudo file or device driver */

          if (vnode_ptr->vop->Open)
            {
              ret = vnode_ptr->vop->Open(vnode_ptr, 0, 0, 0);
            }
          else
            {
              ret = -ENOSYS;
            }
        }

      /* Handle open failures */

      if (ret < 0)
        {
          goto errout_with_vnode;
        }
    }

  if (list != NULL)
    {
      _files_semgive(list);
    }
  return OK;

  /* Handle various error conditions */

errout_with_vnode:
  memset(filep2, 0, sizeof(struct file));
  filep2->fd = -1;

errout_with_ret:
  err               = -ret;
  _files_semgive(list);

errout:
  set_errno(err);
  return VFS_ERROR;
}

struct file *files_allocate(const struct Vnode *vnode_ptr, int oflags, off_t pos, const void *priv, int minfd)
{
  struct filelist *list = NULL;
  unsigned int *p = NULL;
  unsigned int mask;
  unsigned int i;
  struct file *filep = NULL;

  if (minfd < FILE_START_FD)
    {
      minfd = FILE_START_FD;
    }
  i = (unsigned int)minfd;

  list = sched_getfiles();
  DEBUGASSERT(list);

  _files_semtake(list);

  while (i < CONFIG_NFILE_DESCRIPTORS)
    {
      p = ((unsigned int *)bitmap) + (i >> 5); /* Gets the location in the bitmap */
      mask = 1 << (i & 0x1f); /* Gets the mask for the current bit int bitmap */
      if ((~(*p) & mask))
        {
          set_bit(i, bitmap);
          filep = &list->fl_files[i];
          filep->f_oflags   = oflags;
          filep->f_pos      = pos;
          filep->f_vnode    = (struct Vnode *)vnode_ptr;
          filep->f_priv     = (void *)priv;
          filep->f_refcount = 1;
          filep->f_mapping  = (struct page_mapping *)&vnode_ptr->mapping;
          filep->f_dir      = NULL;
          filep->f_path     = vnode_ptr->filePath;
          filep->fd         = i;
          filep->ops        = vnode_ptr->fop;
          _files_semgive(list);
          return filep;
        }
      i++;
    }

  _files_semgive(list);
  return NULL;
}

int files_close_internal(int fd, LosProcessCB *processCB)
{
  int ret = OK;
  struct filelist *list = NULL;
  struct files_struct *process_files = NULL;

  /* 0,1,2 fd is not opened in system, no need to close them */
  if ((fd >= STDIN_FILENO) && (fd <= STDERR_FILENO))
    {
      return OK;
    }

  /* Get the thread-specific file list.  It should never be NULL in this
   * context.
   */

  list = sched_getfiles();
  DEBUGASSERT(list != NULL);

  /* If the file was properly opened, there should be an vnode assigned */

  if (fd < 0 || fd >= CONFIG_NFILE_DESCRIPTORS || !list->fl_files[fd].f_vnode)
    {
      return -EBADF;
    }

  /* Perform the protected close operation */

  _files_semtake(list);
  process_files = processCB->files;
  if (process_files == NULL)
    {
      PRINT_ERR("process files is NULL, %s %d\n", __FUNCTION__ ,__LINE__);
      _files_semgive(list);
      return -EINVAL;
    }

  list->fl_files[fd].f_refcount--;
  if (list->fl_files[fd].f_refcount == 0)
    {
      ret = _files_close(&list->fl_files[fd]);
      if (ret == OK)
        {
          clear_bit(fd, bitmap);
        }
    }

  _files_semgive(list);
  return ret;
}

/****************************************************************************
 * Name: files_close
 *
 * Description:
 *   Close an inode (if open)
 *
 * Assumuptions:
 *   Caller holds the list semaphore because the file descriptor will be freed.
 *
 ****************************************************************************/

int files_close(int fd)
{
  return files_close_internal(fd, OsCurrProcessGet());
}

/****************************************************************************
 * Name: files_release
 *
 * Assumuptions:
 *   Similar to files_close().  Called only from open() logic on error
 *   conditions.
 *
 ****************************************************************************/

void files_release(int fd)
{
  struct filelist *list = NULL;

  list = sched_getfiles();
  DEBUGASSERT(list);

  if (fd >=0 && fd < CONFIG_NFILE_DESCRIPTORS)
    {
      _files_semtake(list);
      struct file *filep = &list->fl_files[fd];

      memset(filep, 0, sizeof(struct file));
      filep->fd = -1;
      clear_bit(fd, bitmap);
      _files_semgive(list);
    }
}

struct Vnode *files_get_openfile(int fd)
{
  struct filelist *list = NULL;
  unsigned int *p = NULL;
  unsigned int mask;

  list = sched_getfiles();
  DEBUGASSERT(list);

  p = ((unsigned int *)bitmap) + ((unsigned int)fd >> 5); /* Gets the location in the bitmap */
  mask = 1 << ((unsigned int)fd & 0x1f); /* Gets the mask for the current bit int bitmap */
  if ((~(*p) & mask))
    {
      return NULL;
    }

  return list->fl_files[fd].f_vnode;
}

int alloc_fd(int minfd)
{
  struct filelist *list = NULL;
  unsigned int *p = NULL;
  unsigned int mask;
  unsigned int i;

  /* minfd should be a positive number,and 0,1,2 had be distributed to stdin,stdout,stderr */

  if (minfd < FILE_START_FD)
    {
      minfd = FILE_START_FD;
    }
  i = (unsigned int)minfd;

  list = sched_getfiles();
  DEBUGASSERT(list);

  _files_semtake(list);

  while (i < CONFIG_NFILE_DESCRIPTORS)
    {
      p = ((unsigned int *)bitmap) + (i >> 5); /* Gets the location in the bitmap */
      mask = 1 << (i & 0x1f); /* Gets the mask for the current bit int bitmap */
      if ((~(*p) & mask))
        {
          set_bit(i, bitmap);
          _files_semgive(list);
          return (int)i;
        }
      i++;
    }
  _files_semgive(list);
  return VFS_ERROR;
}

void clear_fd(int fd)
{
  clear_bit(fd, bitmap);
}

int close_files(struct Vnode *vnode)
{
  int fd = 0;
  int ret = 0;
  struct Vnode *open_file_vnode = NULL;

  for (fd = FILE_START_FD; fd < CONFIG_NFILE_DESCRIPTORS; fd++)
    {
      open_file_vnode = files_get_openfile(fd);
      if (open_file_vnode && (open_file_vnode == vnode))
        {
          ret = files_close(fd);
          if (ret < 0)
            {
              return -EBUSY;
            }
        }
    }

  return 0;
}

void files_refer(int fd)
{
  struct file *filep = NULL;

  struct filelist *list = sched_getfiles();
  if (!list || fd < 0 || fd >= CONFIG_NFILE_DESCRIPTORS)
    {
      return;
    }

  _files_semtake(list);
  (void)fs_getfilep(fd, &filep);
  if (filep != NULL)
    {
      filep->f_refcount++;
    }
  _files_semgive(list);
}

void alloc_std_fd(struct fd_table_s *fdt)
{
  fdt->ft_fds[STDIN_FILENO].sysFd = STDIN_FILENO;
  fdt->ft_fds[STDOUT_FILENO].sysFd = STDOUT_FILENO;
  fdt->ft_fds[STDERR_FILENO].sysFd = STDERR_FILENO;
  FD_SET(STDIN_FILENO, fdt->proc_fds);
  FD_SET(STDOUT_FILENO, fdt->proc_fds);
  FD_SET(STDERR_FILENO, fdt->proc_fds);
}

static void copy_fds(const struct fd_table_s *new_fdt, const struct fd_table_s *old_fdt)
{
  unsigned int sz;

  sz = new_fdt->max_fds * sizeof(struct file_table_s);
  if (sz)
    {
      (void)memcpy_s(new_fdt->ft_fds, sz, old_fdt->ft_fds, sz);
    }
  (void)memcpy_s(new_fdt->proc_fds, sizeof(fd_set), old_fdt->proc_fds, sizeof(fd_set));
  (void)memcpy_s(new_fdt->cloexec_fds, sizeof(fd_set), old_fdt->cloexec_fds, sizeof(fd_set));
}

static void copy_fd_table(struct fd_table_s *new_fdt, struct fd_table_s *old_fdt)
{
  copy_fds((const struct fd_table_s *)new_fdt, (const struct fd_table_s *)old_fdt);
  for (int i = 0; i < new_fdt->max_fds; i++)
    {
      if (FD_ISSET(i, new_fdt->proc_fds))
        {
          int sysFd = GetAssociatedSystemFd(i);
          if ((sysFd >= 0) && (sysFd < CONFIG_NFILE_DESCRIPTORS))
            {
              files_refer(sysFd);
            }
#if defined(LOSCFG_NET_LWIP_SACK)
          if ((sysFd >= CONFIG_NFILE_DESCRIPTORS) && (sysFd < (CONFIG_NFILE_DESCRIPTORS + CONFIG_NSOCKET_DESCRIPTORS)))
            {
              socks_refer(sysFd);
            }
#endif
#if defined(LOSCFG_COMPAT_POSIX)
          if ((sysFd >= MQUEUE_FD_OFFSET) && (sysFd < (MQUEUE_FD_OFFSET + CONFIG_NQUEUE_DESCRIPTORS)))
            {
              MqueueRefer(sysFd);
            }
#endif

        }
    }
}

static struct fd_table_s * alloc_fd_table(unsigned int numbers)
{
  struct fd_table_s *fdt;
  void *data;

  fdt = LOS_MemAlloc(m_aucSysMem0, sizeof(struct fd_table_s));
  if (!fdt)
    {
      goto out;
    }
  fdt->max_fds = numbers;
  if (!numbers)
    {
      fdt->ft_fds = NULL;
      fdt->proc_fds = NULL;
      fdt->cloexec_fds = NULL;
      return fdt;
    }
  data = LOS_MemAlloc(m_aucSysMem0, numbers * sizeof(struct file_table_s));
  if (!data)
    {
      goto out_fdt;
    }
  fdt->ft_fds = data;

  for (int i = STDERR_FILENO + 1; i < numbers; i++)
    {
        fdt->ft_fds[i].sysFd = -1;
    }

  data = LOS_MemAlloc(m_aucSysMem0, 2 * sizeof(fd_set)); /* 2: proc_fds, cloexec_fds */
  if (!data)
    {
      goto out_arr;
    }
  (VOID)memset_s(data, 2 * sizeof(fd_set), 0, 2 * sizeof(fd_set));
  fdt->proc_fds = (fd_set *)data;
  fdt->cloexec_fds = (fd_set *)((uintptr_t)data + sizeof(fd_set));

  alloc_std_fd(fdt);

  (void)sem_init(&fdt->ft_sem, 0, 1);

  return fdt;

out_arr:
  (VOID)LOS_MemFree(m_aucSysMem0, fdt->ft_fds);
out_fdt:
  (VOID)LOS_MemFree(m_aucSysMem0, fdt);
out:
  return NULL;
}

struct files_struct *alloc_files(void)
{
  struct files_struct *files = LOS_MemAlloc(m_aucSysMem0, sizeof(struct files_struct));
  if (!files)
    {
      ferr("malloc file_struct error\n");
      return NULL;
    }
  files->count = 1;
  files->file_lock = 0;
  files->next_fd = 3;
#ifdef VFS_USING_WORKDIR
  spin_lock_init(&files->workdir_lock);
  memset_s(files->workdir, PATH_MAX, 0, PATH_MAX);
  files->workdir[0] = '/';
#endif
  files->fdt = alloc_fd_table(NR_OPEN_DEFAULT);
  if (!files->fdt)
    {
      ferr("malloc fdt error\n");
      (VOID)LOS_MemFree(m_aucSysMem0, files);
      return NULL;
    }

  return files;
}

struct files_struct *dup_fd(struct files_struct *old_files)
{
  struct fd_table_s *old_fdt = NULL;
  struct fd_table_s *new_fdt = NULL;
  struct files_struct *files = NULL;
  if((old_files == NULL) || (old_files->fdt == NULL) || (old_files->fdt->max_fds == 0))
    {
      return alloc_files();
    }
  files = LOS_MemAlloc(m_aucSysMem0, sizeof(struct files_struct));
  if(!files)
    {
      ferr("malloc file_struct error\n");
      return NULL;
    }
  files->count = 1;
  files->file_lock = 0;
  files->next_fd = old_files->next_fd;
#ifdef VFS_USING_WORKDIR
  spin_lock_init(&files->workdir_lock);
  memset_s(files->workdir, PATH_MAX, 0, PATH_MAX);
  strncpy_s(files->workdir, PATH_MAX - 1, old_files->workdir, PATH_MAX - 1);
#endif
  old_fdt = old_files->fdt;
  new_fdt = alloc_fd_table(old_fdt->max_fds);
  if(new_fdt == NULL)
    {
      PRINT_ERR("alloc fd_table failed\n");
      (VOID)LOS_MemFree(m_aucSysMem0, files);
      return NULL;
    }
  copy_fd_table(new_fdt, old_fdt);
  files->fdt = new_fdt;

  return files;
}

/****************************************************************************
 * Name: delete_files
 *
 * Description:
 *   Close a current process's fd specified by struct files.
 *   And delete files struct.
 *
 ****************************************************************************/
void delete_files(struct files_struct *files)
{
  if (files == NULL)
    {
      return;
    }

  if (files->fdt == NULL)
    {
      goto out_file;
    }

  for (int i = 0; i < files->fdt->max_fds; i++)
    {
      if (FD_ISSET(i, files->fdt->proc_fds))
        {
          int sysFd = DisassociateProcessFd(i);
          close(sysFd);
          FreeProcessFd(i);
        }
    }

  (VOID)sem_destroy(&files->fdt->ft_sem);
  (VOID)LOS_MemFree(m_aucSysMem0, files->fdt->ft_fds);
  (VOID)LOS_MemFree(m_aucSysMem0, files->fdt->proc_fds);
  (VOID)LOS_MemFree(m_aucSysMem0, files->fdt);
out_file:
  (VOID)LOS_MemFree(m_aucSysMem0, files);
  return;
}

struct files_struct *create_files_snapshot(const struct files_struct *old_files)
{
  struct fd_table_s *old_fdt = NULL;
  struct fd_table_s *new_fdt = NULL;
  struct files_struct *files = NULL;
  if ((old_files == NULL) || (old_files->fdt == NULL) || (old_files->fdt->max_fds == 0))
    {
      return NULL;
    }
  files = LOS_MemAlloc(m_aucSysMem0, sizeof(struct files_struct));
  if (!files)
    {
      PRINT_ERR("malloc file_struct error\n");
      return NULL;
    }
  files->count = 1;
  files->file_lock = 0;
  files->next_fd = old_files->next_fd;
#ifdef VFS_USING_WORKDIR
  spin_lock_init(&files->workdir_lock);
  memset_s(files->workdir, PATH_MAX, 0, PATH_MAX);
  strncpy_s(files->workdir, PATH_MAX - 1, old_files->workdir, PATH_MAX - 1);
#endif
  old_fdt = old_files->fdt;
  new_fdt = alloc_fd_table(old_fdt->max_fds);
  if (new_fdt == NULL)
    {
      PRINT_ERR("alloc fd_table failed\n");
      (VOID)LOS_MemFree(m_aucSysMem0, files);
      return NULL;
    }
  copy_fds((const struct fd_table_s *)new_fdt, (const struct fd_table_s *)old_fdt);
  files->fdt = new_fdt;

  return files;

}

void delete_files_snapshot(struct files_struct *files)
{
  if (files == NULL)
    {
      return;
    }
  if (files->fdt == NULL)
    {
      goto out_file;
    }

  (VOID)sem_destroy(&files->fdt->ft_sem);
  (VOID)LOS_MemFree(m_aucSysMem0, files->fdt->ft_fds);
  (VOID)LOS_MemFree(m_aucSysMem0, files->fdt->proc_fds);
  (VOID)LOS_MemFree(m_aucSysMem0, files->fdt);
out_file:
  (VOID)LOS_MemFree(m_aucSysMem0, files);
  return;
}

/****************************************************************************
 * fs/dirent/fs_seekdir.c
 *
 *   Copyright (C) 2007, 2008, 2011, 2014 Gregory Nutt. All rights reserved.
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

#include "vfs_config.h"
#include "sys/types.h"
#include "dirent.h"
#include "errno.h"
#include "fs/dirent_fs.h"
#include "vnode.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/
/****************************************************************************
 * Name: seekmountptdir
 ****************************************************************************/

static inline void seekmountptdir(struct fs_dirent_s *idir, off_t offset)
{
  struct Vnode *vnode;
  off_t pos;

  /* Determine a starting point for the seek.  If the seek
   * is "forward" from the current position, then we will
   * start at the current poisition.  Otherwise, we will
   * "rewind" to the root dir.
   */

  vnode = idir->fd_root;
  if (offset < idir->fd_position)
    {
      if (vnode->vop != NULL && vnode->vop->Rewinddir != NULL)
        {
          /* Perform the rewinddir() operation */

          vnode->vop->Rewinddir(vnode, idir);
          pos = 0;
        }
      else
        {
          /* We can't do the seek and there is no way to return
           * an error indication.
           */

          return;
        }
    }
  else
    {
      pos = idir->fd_position;
    }

  /* This is a brute force approach... we will just read
   * directory entries until we are at the desired position.
   */

  idir->read_cnt = 1;

  while (pos < offset)
    {
      if (!vnode->vop || !vnode->vop->Readdir ||
           vnode->vop->Readdir(vnode, idir) <= 0)
        {
          /* We can't read the next entry and there is no way to return
           * an error indication.
           */

           return;
        }

      /* Increment the position on each successful read */

      pos++;
    }

  /* If we get here the directory position has been successfully set */
  idir->fd_position = pos;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: seekdir
 *
 * Description:
 *   The seekdir() function sets the location in the directory stream from
 *   which the next readdir() call will start.  seekdir() should be used with
 *   an offset returned by telldir().
 *
 * Input Parameters:
 *   dirp -- An instance of type DIR created by a previous
 *     call to opendir();
 *   offset -- offset to seek to
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void seekdir(DIR *dirp, long offset)
{
  struct fs_dirent_s *idir = (struct fs_dirent_s *)dirp;

  if (!idir || !idir->fd_root || idir->fd_status != DIRENT_MAGIC)
    {
      set_errno(EBADF);
      return;
    }

  if (offset < 0)
    {
      set_errno(EINVAL);
      return;
    }

  seekmountptdir(idir, offset);
}

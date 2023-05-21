#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import os
import shutil
import argparse
from utils import exec_command
from utils import makedirs
import tarfile


BUFSIZE = 8*1024


def cmp_file(old, new):
    old_file_st = os.stat(old)
    new_file_st = os.stat(new)

    if old_file_st.st_size != new_file_st.st_size:
        return False

    buf_size = BUFSIZE
    with open(old, 'rb') as file_old, open(new, 'rb') as file_new:
        while True:
            old_buf = file_old.read(buf_size)
            new_buf = file_new.read(buf_size)
            if old_buf != new_buf:
                return False
            if not old_buf:
                return True


def move_file(old_path, new_path):
    if os.path.exists(new_path):
        shutil.rmtree(new_path)
    if os.path.exists(old_path):
        shutil.copytree(old_path, new_path)


def is_needed_copy(file, ignore_list):
    for ignore in ignore_list:
        if file.endswith(ignore) or file.startswith(ignore):
            return False

    return True


def copy(source, target, ignore_list):
    for file in os.listdir(source):
        source_file = os.path.join(source, file)
        target_file = os.path.join(target, file)
        if os.path.exists(target_file) and \
            cmp_file(source_file, target_file):
            continue
        if os.path.exists(target_file) and \
            cmp_file(source_file, target_file) == False:
            os.remove(target_file)
        if is_needed_copy(file, ignore_list) and \
            os.path.isfile(source_file):
            shutil.copy(source_file, target)


def mv_usr_libs(path):
    libs = [lib for lib in os.listdir(path) if lib.startswith('lib') and
            lib.endswith('.so')]
    target_path = os.path.join(path, 'libs/usr')
    if not os.path.exists(target_path):
        makedirs(target_path)
    for lib in libs:
        source_file = os.path.join(path, lib)
        target_file = os.path.join(target_path, lib)
        shutil.move(source_file, target_file)


def check_strip(path, strip_cmd, log):
    if strip_cmd == "":
        return
    strip_cmd_list = strip_cmd.split(" ")
    for relpath, _, files in os.walk(path):
        for file in files:
            full_path = os.path.join(path, relpath, file)
            if os.path.isfile(full_path):
                cmd = strip_cmd_list + [full_path]
                exec_command(cmd, log_path=log)


def tee_into_userfs(output_path, userfs):
    vendor_bin_source_dir = os.path.join(output_path, 'vendor/bin')
    if not os.path.exists(vendor_bin_source_dir):
        return
    sec_storage_dir = os.path.join(userfs, 'data/sec_storage_data')
    makedirs(sec_storage_dir)

    sec_storage_root_dir = os.path.join(userfs, 'sec_storage')
    makedirs(sec_storage_root_dir)


def list_all_files(rootdir):
    _files = []
    filelist = os.listdir(rootdir)
    for i in filelist:
        path = os.path.join(rootdir, i)
        if os.path.isdir(path):
            _files.append(path)
            _files.extend(list_all_files(path))
        if os.path.isfile(path):
            _files.append(path)

    return _files


def chmod_files_mode(root_dir, dir_mode, file_mode):
    if os.path.isdir(root_dir):
        os.chmod(root_dir, dir_mode)
    filelist = list_all_files(root_dir)
    for i in filelist:
        if os.path.isdir(i):
            os.chmod(i, dir_mode)
        if os.path.isfile(i):
            os.chmod(i, file_mode)


def change_rootfs_filemode(path):
    # change all files filemode
    chmod_files_mode(path, 493, 365)
    # change special dirs filemode
    tmppath = os.path.join(path, "bin")
    if os.path.exists(tmppath):
        chmod_files_mode(tmppath, 365, 365)
    tmppath = os.path.join(path, "usr")
    if os.path.exists(tmppath):
        chmod_files_mode(tmppath, 365, 365)
    tmppath = os.path.join(path, "lib")
    if os.path.exists(tmppath):
        chmod_files_mode(tmppath, 365, 365)
    tmppath = os.path.join(path, "vendor")
    if os.path.exists(tmppath):
        chmod_files_mode(tmppath, 365, 292)
    tmppath = os.path.join(path, "system")
    if os.path.exists(tmppath):
        chmod_files_mode(tmppath, 365, 292)
    tmppath = os.path.join(path, "etc")
    if os.path.exists(tmppath):
        chmod_files_mode(tmppath, 365, 292)
    tmppath = os.path.join(path, "vendor/bin")
    if os.path.exists(tmppath):
        chmod_files_mode(tmppath, 365, 365)
    # change special files filemode
    tmppath = os.path.join(path, "etc/init.cfg")
    if os.path.exists(tmppath):
        os.chmod(tmppath, 256)
    tmppath = os.path.join(path, "bin/init")
    if os.path.exists(tmppath):
        os.chmod(tmppath, 320)
    tmppath = os.path.join(path, "bin/shell")
    if os.path.exists(tmppath):
        os.chmod(tmppath, 320)


def create_symlinks_for_dv(path):
    dst = os.path.join(path, "usr/lib/a7_softfp_neon-vfpv4")
    if os.path.exists(dst):
        os.remove(dst)
    os.symlink("./", dst)
    dst = os.path.join(path, "bin/shell")
    if os.path.exists(dst):
        os.remove(dst)
    os.symlink("sh", dst)


def change_rootfs_filemode_linux(path):
    tmppath = os.path.join(path, "lib")
    chmod_files_mode(tmppath, 493, 420)
    tmppath = os.path.join(path, "lib/ld-uClibc-0.9.33.2.so")
    if os.path.exists(tmppath):
        os.chmod(tmppath, 365)
    tmppath = os.path.join(path, "lib/ld-2.24.so")
    if os.path.exists(tmppath):
        os.chmod(tmppath, 365)
    tmppath = os.path.join(path, "usr")
    os.chmod(tmppath, 493)
    tmppath = os.path.join(path, "usr/lib")
    chmod_files_mode(tmppath, 493, 420)
    tmppath = os.path.join(path, "etc/init.cfg")
    if os.path.exists(tmppath):
        os.chmod(tmppath, 256)
    if "dv300" or "taurus" in path:
        create_symlinks_for_dv(path)


def change_userfs_filemode(path):
    # change all files filemode
    chmod_files_mode(path, 493, 365)
    # change special files filemode
    tmppath = os.path.join(path, "data/cameradev.ini")
    if os.path.exists(tmppath):
        os.chmod(tmppath, 365)


def remove_file_in_rootfs_linux(output_path):
    rootfs_data = os.path.join(output_path, 'rootfs/data')
    if os.path.exists(rootfs_data):
        shutil.rmtree(rootfs_data)


def remove_file_in_rootfs(output_path):
    rootfs_app = os.path.join(output_path, 'rootfs/app')
    rootfs_data = os.path.join(output_path, 'rootfs/data')
    if os.path.exists(rootfs_app):
        shutil.rmtree(rootfs_app)
    if os.path.exists(rootfs_data):
        shutil.rmtree(rootfs_data)


def make_rootfs_tar(tar_filename, source_dir):
    with tarfile.open(tar_filename, "w") as tar:
        tar.add(source_dir, arcname=os.path.basename(source_dir))


def add_mount_userfs_linux(rootfs):
    mount_userfs_path = os.path.join(rootfs, 'storage')
    if not os.path.exists(mount_userfs_path):
        os.makedirs(mount_userfs_path)


def gen_rootfs(mkfs, output_path, rootfs_dirs_dict, kernel, storage_type):
    mv_usr_libs(output_path)
    rootfs = os.path.join(output_path, 'rootfs')
    rootfs_tar = os.path.join(output_path, 'rootfs.tar')
    if not os.path.exists(rootfs):
        print('rootfs dir not exist in {}'.format(rootfs))
        return 0
    log = os.path.join(output_path, 'build.log')
    for path_part, value_list in rootfs_dirs_dict.items():
        source_path = os.path.join(output_path, path_part)
        target_path = os.path.join(rootfs, value_list[0])
        strip_cmd = value_list[2]

        if os.path.exists(source_path):
            if not os.path.exists(target_path):
                makedirs(target_path)
            ignore_list = value_list[1]
            copy(source_path, target_path, ignore_list)
        if kernel == "liteos_a":
            check_strip(target_path, strip_cmd, log)

    if kernel == "linux":
        remove_file_in_rootfs_linux(output_path)
        change_rootfs_filemode_linux(rootfs)
        add_mount_userfs_linux(rootfs)
        if storage_type == "emmc":
            cmd = [mkfs, rootfs, 'ext4']
            exec_command(cmd, log_path=log)
        if storage_type == "spinor":
            cmd = [mkfs, rootfs, "jffs2"]
            exec_command(cmd, log_path=log)
    if kernel == "liteos_a":
        remove_file_in_rootfs(output_path)
        change_rootfs_filemode(rootfs)
        if storage_type == "emmc":
            cmd = [mkfs, rootfs, 'vfat']
            exec_command(cmd, log_path=log)
        if storage_type == "spinor":
            cmd = [mkfs, rootfs, 'jffs2']
            exec_command(cmd, log_path=log)
        if storage_type == "spinand":
            cmd = [mkfs, rootfs, 'yaffs2']
            exec_command(cmd, log_path=log)
        make_rootfs_tar(rootfs_tar, rootfs)
        if os.path.exists(rootfs):
            chmod_files_mode(rootfs, 511, 511)
            shutil.rmtree(rootfs)

    return 0


def make_userfs_dir(dir_path):
    if not os.path.exists(dir_path):
        makedirs(dir_path)
    if not os.path.exists(dir_path):
        print('make' + str(dir_path) + 'fail')
        return -1


def move_rootfs_to_userfs(output_path):
    rootfs_app = os.path.join(output_path, 'rootfs/app')
    rootfs_data = os.path.join(output_path, 'rootfs/data')
    userfs_app = os.path.join(output_path, 'userfs/app')
    userfs_data = os.path.join(output_path, 'userfs/data')
    if os.path.exists(rootfs_app):
        move_file(rootfs_app, userfs_app)
    if os.path.exists(rootfs_data):
        move_file(rootfs_data, userfs_data)


def gen_userfs(mkfs, output_path, userfs_dirs_dict, kernel, storage_type):
    userfs = os.path.join(output_path, 'userfs')
    userfs_etc = os.path.join(output_path, 'userfs/etc')
    if make_userfs_dir(userfs):
        return 0
    if make_userfs_dir(userfs_etc):
        return 0
    move_rootfs_to_userfs(output_path)
    log = os.path.join(output_path, 'build.log')
    tee_into_userfs(output_path, userfs)
    for path_part, value_list in userfs_dirs_dict.items():
        source_path = os.path.join(output_path, path_part)
        target_path = os.path.join(userfs, value_list[0])
        strip_cmd = value_list[2]
        if os.path.exists(source_path):
            if not os.path.exists(target_path):
                makedirs(target_path)
            ignore_list = value_list[1]
            copy(source_path, target_path, ignore_list)

        check_strip(target_path, strip_cmd, log)
    change_userfs_filemode(userfs)
    if kernel == "linux":
        if storage_type == "emmc":
            cmd = [mkfs, userfs, 'ext4']
            exec_command(cmd, log_path=log)
        if storage_type == "spinor":
            cmd = [mkfs, userfs, "jffs2"]
            exec_command(cmd, log_path=log)
    if kernel == "liteos_a":
        if storage_type == "emmc":
            cmd = [mkfs, userfs, 'vfat', '52428800']
            exec_command(cmd, log_path=log)
        if storage_type == "spinor":
            cmd = [mkfs, userfs, 'jffs2']
            exec_command(cmd, log_path=log)
        if storage_type == "spinand":
            cmd = [mkfs, userfs, 'yaffs2']
            exec_command(cmd, log_path=log)
    return 0


def gen_systemfs(mkfs, output_path, kernel, storage_type):
    if kernel == "linux":
        if storage_type == "emmc":
            systemfs = os.path.join(output_path, 'systemfs')
            if make_userfs_dir(systemfs):
                return -1
            systemhashfs = os.path.join(output_path, 'systemhashfs')
            if make_userfs_dir(systemhashfs):
                return -1
            log = os.path.join(output_path, 'build.log')
            cmd_mksysfs = [mkfs, systemfs, 'ext4']
            exec_command(cmd_mksysfs, log_path=log)
            dmverity = os.path.join(
                output_path,
                '../../../build/lite/make_rootfs/dmverity_linux.sh')
            cmd_veritysetup = [dmverity, output_path, 'veritysetup']
            exec_command(cmd_veritysetup, log_path=log)
            cmd_mksyshashfs = [mkfs, systemhashfs, 'ext4', '6']
            exec_command(cmd_mksyshashfs, log_path=log)
            cmd_adds82ohos = [dmverity, output_path, 'addS82ohos']
            exec_command(cmd_adds82ohos, log_path=log)
    return 0


def main():
    parser = argparse.ArgumentParser(description='Generate rootfs')
    parser.add_argument('--path', help='Build output path')
    parser.add_argument('--kernel', help='OHOS kernel type')
    parser.add_argument('--storage', help='Board storage type')
    parser.add_argument('--strip_command', help='So strip command')
    parser.add_argument('--dmverity', help='OHOS security dmverity type')
    args = parser.parse_args()

    strip_cmd = args.strip_command
    kernel = args.kernel
    storage_type = args.storage
    dmverity_enable = args.dmverity

    if args.path:
        output_path = os.path.abspath(args.path)
        if kernel == "liteos_a":
            mkfs = os.path.join(
                output_path,
                '../../../build/lite/make_rootfs/rootfsimg_liteos.sh')
        if kernel == "linux":
            mkfs = os.path.join(
                output_path,
                '../../../build/lite/make_rootfs/rootfsimg_linux.sh')
        if kernel == "liteos_m":
            print('no need to make rootfs')
            return 0
        if not os.path.exists(mkfs):
            print('mkfs not exist in {}'.format(mkfs))
            return -1
    else:
        return -1

    rootfs_dirs_dict = {
        'bin': ['bin', ['Test.bin', 'TestSuite.bin', 'query.bin', 'cve',
                        'checksum'], strip_cmd],
        'libs': ['lib', ['.a'], strip_cmd],
        'libs/usr': ['usr/lib', ['.a'], strip_cmd],
        'bin/usr': ['usr/bin', [], strip_cmd],
        'vendor/bin': ['vendor/bin', [], ""],
        'vendor/lib': ['vendor/lib', [], ""],
        'vendor/firmware/hi3881': ['vendor/firmware/hi3881', [], ""],
        'config': ['etc', [], ""],
        'system/internal': ['system/internal', [], ""],
        'etc': ['etc', [], ""],
        'data': ['data', [], ""],
        'obj/foundation/distributedschedule/samgr_lite/config': ['etc', [], ""]
    }
    userfs_dirs_dict = {
        'obj/base/security/services/app_verify/config':
        ['data/verify', [], ""],
        'storage/etc': ['etc', [], ""],
        'data': ['data', [], ""]
    }
    ret = gen_userfs(mkfs, output_path, userfs_dirs_dict, kernel, storage_type)
    if ret:
        print('gen userfs failed')
        return -1

    if dmverity_enable == "true":
        ret = gen_systemfs(mkfs, output_path, kernel, storage_type)
        if ret:
            print('gen systemfs failed')
            return -1

    return gen_rootfs(mkfs, output_path, rootfs_dirs_dict,
                      kernel, storage_type)


if __name__ == "__main__":
    sys.exit(main())

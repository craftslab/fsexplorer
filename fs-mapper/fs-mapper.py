#!/usr/bin/env python
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Author: Jia Jia
#

import os, sys
import getopt
from ctypes import *


banner = """
 _____      __  __                             
|  ___|__  |  \/  | __ _ _ __  _ __   ___ _ __ 
| |_ / __| | |\/| |/ _` | '_ \| '_ \ / _ \ '__|
|  _|\__ \ | |  | | (_| | |_) | |_) |  __/ |   
|_|  |___/ |_|  |_|\__,_| .__/| .__/ \___|_|   
                        |_|   |_|              
"""


FS_TYPE_EXT4 = "ext4"
FS_TYPE_FAT = "fat"

FS_DNAME_LEN = 256
FS_DNAME_DOT = '.'
FS_DNAME_DOTDOT = '..'


class libfs_ftype:
    FT_UNKNOWN  = 0
    FT_REG_FILE = 1
    FT_DIR      = 2
    FT_CHRDEV   = 3
    FT_BLKDEV   = 4
    FT_FIFO     = 5
    FT_SOCK     = 6
    FT_SYMLINK  = 7
    FT_MAX      = 8


class libfs_imode:
    IXOTH  = 0x1
    IWOTH  = 0x2
    IROTH  = 0x4
    IXGRP  = 0x8
    IWGRP  = 0x10
    IRGRP  = 0x20
    IXUSR  = 0x40
    IWUSR  = 0x80
    IRUSR  = 0x100
    ISVTX  = 0x200
    ISGID  = 0x400
    ISUID  = 0x800
    IFIFO  = 0x1000
    IFCHR  = 0x2000
    IFDIR  = 0x4000
    IFBLK  = 0x6000
    IFREG  = 0x8000
    IFLNK  = 0xA000
    IFSOCK = 0xC000
    IFMAX  = 0xFFFF


class libfs_timespec(Structure):
    _fields_ = [('tv_sec', c_int64),
                ('tv_nsec', c_int64)]


class fs_fsid_t(Structure):
    _fields_ = [('val', c_int32 * 2)]


class fs_kstatfs(Structure):
    _fields_ = [('f_type', c_int32),
                ('padding', c_uint8 * 4),
                ('f_bsize', c_int64),
                ('f_blocks', c_uint64),
                ('f_bfree', c_uint64),
                ('f_bavail', c_uint64),
                ('f_files', c_uint64),
                ('f_ffree', c_uint64),
                ('f_fsid', fs_fsid_t),
                ('f_namelen', c_int64),
                ('f_frsize', c_int64),
                ('f_flags', c_int64),
                ('f_spare', c_int64 * 4)]


class fs_kstat(Structure):
    _fields_ = [('ino', c_uint64),
                ('mode', c_int32),
                ('nlink', c_uint32),
                ('uid', c_uint32),
                ('gid', c_uint32),
                ('size', c_int64),
                ('atime', libfs_timespec),
                ('mtime', libfs_timespec),
                ('ctime', libfs_timespec),
                ('blksize', c_uint64),
                ('blocks', c_uint64),
                ('extras', c_char * 32)]


class fs_dirent(Structure):
    _fields_ = [('d_ino', c_uint64),
                ('d_off', c_int64),
                ('d_reclen', c_uint16),
                ('padding0', c_uint8 * 2),
                ('d_type', c_int32),
                ('d_name', c_char * FS_DNAME_LEN),
                ('d_childnum', c_uint32),
                ('padding1', c_uint8 * 4)]


class fs_opt_t(Structure):
    _fields_ = [('mount', CFUNCTYPE(c_int32, c_char_p, c_char_p, c_char_p, c_int32, POINTER(fs_dirent))),
                ('umount', CFUNCTYPE(c_int32, c_char_p, c_int32)),
                ('statfs', CFUNCTYPE(c_int32, c_char_p, POINTER(fs_kstatfs))),
                ('statrawfs', CFUNCTYPE(c_int32, c_char_p, POINTER(c_char_p))),
                ('stat', CFUNCTYPE(c_int32, c_uint64, POINTER(fs_kstat))),
                ('statraw', CFUNCTYPE(c_int32, c_uint64, POINTER(c_char_p))),
                ('querydent', CFUNCTYPE(c_int32, c_uint64, POINTER(fs_dirent))),
                ('getdents', CFUNCTYPE(c_int32, c_uint64, POINTER(fs_dirent), c_uint)),
                ('readfile', CFUNCTYPE(c_int32, c_uint64, c_int64, c_char_p, c_int64, POINTER(c_int64)))]


def dump_fs_map(fsmap, mapfile):
    fsbuf = '\n'.join(fsmap)

    fd = os.open(mapfile, os.O_CREAT | os.O_RDWR)
    os.write(fd, fsbuf)
    os.close(fd)


def postproc_fs_map(fsmap):
    fsmap.sort(reverse=True)
    return fsmap


def fill_fs_map(fsopt, fspath, fsdirent, fsmap):
    if fsdirent.d_type != libfs_ftype.FT_REG_FILE:
        return

    fsstat = fs_kstat()
    ret = fsopt.stat(fsdirent.d_ino, byref(fsstat))
    if ret != 0:
        return

    if fsstat.size == 0:
        return

    blocklist = fsstat.extras

    fsmap.append('%s %s' % (fspath + '/' + fsdirent.d_name, blocklist))


def traverse_fs_dents(fsopt, fsino, fspath, fsmap):
    fsdirent = fs_dirent()
    ret = fsopt.querydent(fsino, byref(fsdirent))
    if ret != 0:
        return

    if fsdirent.d_childnum == 0:
        return

    fs_dirents = fs_dirent * fsdirent.d_childnum
    fsdirents = fs_dirents()
    ret = fsopt.getdents(fsino, fsdirents, fsdirent.d_childnum)
    if ret != 0:
        return

    for i in range(fsdirent.d_childnum):
        d_name = fsdirents[i].d_name
        if d_name == FS_DNAME_DOT or d_name == FS_DNAME_DOTDOT:
            continue

        fill_fs_map(fsopt, fspath, fsdirents[i], fsmap)

        path = fspath
        path += '/' + d_name
        traverse_fs_dents(fsopt, fsdirents[i].d_ino, path, fsmap)


def build_fs_map_helper(fsopt, fsroot):
    fsmap = []
    traverse_fs_dents(fsopt, fsroot.d_ino, '/system', fsmap)
    return fsmap


def build_fs_map(fsfile):
    fsopt = fs_opt_t()
    fsroot = fs_dirent()

    lib_handle = load_fs_library()
    if lib_handle is None:
        return None

    lib_handle.fs_opt_init.argtypes = [POINTER(fs_opt_t)]
    lib_handle.fs_opt_init.restype = c_int32
    ret = lib_handle.fs_opt_init(byref(fsopt))
    if ret != 0:
        return None

    ret = fsopt.mount(c_char_p(fsfile), 'mnt', FS_TYPE_EXT4, 0, byref(fsroot))
    if ret != 0:
        return None

    fsmap = build_fs_map_helper(fsopt, fsroot)
    fsopt.umount('mnt', 0)

    return fsmap


def load_fs_library():
    if sys.platform.startswith('linux'):
        path = os.path.join(os.getcwd(), 'lib', 'libyafuse2.so')
        instance = cdll.LoadLibrary(path)
    else:
        instance = None

    return instance


def print_usage():
    print >> sys.stdout, ''
    print >> sys.stdout, 'Version: 15.03'
    print >> sys.stdout, 'License: Apache v2.0'
    print >> sys.stdout, 'Author: Jia Jia'
    print >> sys.stdout, ''
    print >> sys.stdout, '-------------------------------------------------------------------------------'
    print >> sys.stdout, 'USAGE: python fs-mapper.py [OPTION...]'
    print >> sys.stdout, ''
    print >> sys.stdout, 'OPTIONS:'
    print >> sys.stdout, '  -i, --input IMAGE  Input fs image of IMAGE'
    print >> sys.stdout, '  -o, --output MAP   Output map to MAP [IMAGE.map]'
    print >> sys.stdout, '  -h, --help         Display help message'
    print >> sys.stdout, ''


def main():
    global banner

    fs_file = ''
    map_file= ''
    fs_map = []

    print >> sys.stdout, banner

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'i:o:h', ['input=', 'output=', 'help'])
    except getopt.GetoptError, err:
        print >> sys.stderr, err
        print_usage()
        sys.exit(1)

    for o, a in opts:
        if o in ('-i', '--input'):
            fs_file = a
        elif o in ('-o', '--output'):
            map_file = a
        elif o in ('-h', '--help'):
            print_usage()
            sys.exit(0)
        else:
            continue

    if len(fs_file) == 0:
        print_usage()
        sys.exit(1)

    if os.access(fs_file, os.F_OK | os.R_OK) is False:
        print >> sys.stderr, 'invalid %s.' % (fs_file)
        print >> sys.stderr, 'type -h or --help for more info.'
        sys.exit(1)

    if len(map_file) == 0:
        file_list = fs_file.split(os.path.sep)
        file_list = file_list[-1].split('.')
        file_list[-1] = 'map'
        map_file = os.path.join(os.getcwd(), '.'.join(file_list))

    fs_map = build_fs_map(fs_file)
    if fs_map is None or len(fs_map) == 0:
        print >> sys.stderr, 'failed to build fs map.'
        sys.exit(1)

    fs_map = postproc_fs_map(fs_map)
    dump_fs_map(fs_map, map_file)


if __name__ == '__main__':
    main()

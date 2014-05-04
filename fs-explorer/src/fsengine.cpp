/**
 * fsengine.cpp - The entry of fsengine
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This file is part of FS Explorer.
 *
 * FS explorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FS Explorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FS Explorer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fsengine.h"

#define FS_LIB_NAME "libyafuse2"

const char* FsEngine::fileTypeList[] = {
  FS_TYPE_EXT4,
  FS_TYPE_FAT,
};

FsEngine::FsEngine(QWidget *parent)
{
  parent = parent;

  fileLib = NULL;
  fileOpt = NULL;
  fileName = NULL;
  fileMount = NULL;
  fileType = NULL;
  fileRoot = NULL;

  readOnly = true;
}

FsEngine::~FsEngine()
{
  closeFile();
}

bool FsEngine::openFile(QString &name)
{
  const char *dev = NULL, *dir = NULL, *type = NULL;
  QByteArray ba;
  int32_t i, len;
  int32_t ret;

  if (fileName && !fileName->compare(name)) {
    return false;
  }

  if (!loadLibrary()) {
    goto openFileFail;
  }

  if (!fileOpt || !fileOpt->mount || !fileOpt->umount) {
    goto openFileFail;
  }

  /*
   * Invalid character will be appended to string
   * if 'name.toLatin1.data()' is used instead.
   */
  ba = name.toLatin1();
  dev = (const char *)ba.data();

  dir = (const char *)"/";
  len = sizeof(fileTypeList) / sizeof(const char*);

  fileRoot = new fs_dirent;
  if (!fileRoot) {
    goto openFileFail;
  }
  memset((void *)fileRoot, 0, sizeof(struct fs_dirent));

  for (i = 0; i < len; ++i) {
    type = fileTypeList[i];
    ret = fileOpt->mount(dev, dir, type, 0, fileRoot);
    if (ret == 0) {
      break;
    }
  }

  if (ret != 0) {
    goto openFileFail;
  }

  fileName = new QString(name);
  fileMount = new QString(dir);
  fileType = new QString(type);
  if (!fileName || !fileMount || !fileType) {
    goto openFileFail;
  }

  return true;

openFileFail:

  closeFile();

  return false;
}

bool FsEngine::closeFile()
{
  if (fileType) {
    delete fileType;
    fileType = NULL;
  }

  if (fileName) {
    delete fileName;
    fileName = NULL;
  }

  if (fileOpt && fileOpt->umount && fileMount) {
    (void)fileOpt->umount((const char *)fileMount->toLatin1().data(), 0);
  }

  if (fileMount) {
    delete fileMount;
    fileMount = NULL;
  }

  if (fileRoot) {
    delete fileRoot;
    fileRoot = NULL;
  }

  unloadLibrary();

  return true;
}

bool FsEngine::isReadOnly() const
{
  return readOnly;
}

QString FsEngine::getFileType() const
{
  if (!fileType) {
    return QString("N/A");
  }

  return *fileType;
}

struct fs_kstatfs FsEngine::getFileStat() const
{
  struct fs_kstatfs ret;

  memset((void *)&ret, 0, sizeof(struct fs_kstatfs));

  // TODO

  return ret;
}

QString FsEngine::getFileStatDetail() const
{
  QString str;
  const char *buf = NULL;
  int32_t ret;

  str.clear();

  if (!fileName) {
    return str;
  }

  ret = fileOpt->statrawfs((const char *)fileName->constData(), &buf);
  if (ret != 0 || !buf) {
    return str;
  }
  str = buf;

  return str;
}

struct fs_dirent FsEngine::getFileRoot() const
{
  struct fs_dirent dent;

  memset((void *)&dent, 0, sizeof(struct fs_dirent));
  if (fileRoot) {
    dent = *fileRoot;
  }

  return dent;
}

unsigned int FsEngine::getFileChildsNum(unsigned long long ino) const
{
  struct fs_dirent parent;

  if (!fileOpt || !fileOpt->querydent || !fileOpt->getdents) {
    return 0;
  }

  memset((void *)&parent, 0, sizeof(struct fs_dirent));

  int32_t ret = fileOpt->querydent(ino, &parent);
  if (ret != 0) {
    return 0;
  }

  return parent.d_childnum;
}

bool FsEngine::getFileChilds(unsigned long long ino, struct fs_dirent *childs, unsigned int num)
{
  if (!childs || num == 0) {
    return false;
  }

  int32_t ret = fileOpt->getdents(ino, childs, num);
  if (ret != 0) {
    return false;
  }

  return true;
}

struct fs_kstat FsEngine::getFileChildsStat(unsigned long long ino) const
{
  struct fs_kstat ret;

  memset((void *)&ret, 0, sizeof(struct fs_kstat));
  (void)fileOpt->stat(ino, &ret);

  return ret;
}

QString FsEngine::getFileChildsStatDetail(unsigned long long ino) const
{
  QString str;
  const char *buf = NULL;
  int32_t ret;

  str.clear();

  ret = fileOpt->statraw(ino, &buf);
  if (ret != 0 || !buf) {
    return str;
  }
  str = buf;

  return str;
}

bool FsEngine::loadLibrary()
{
  fs_opt_init_t optHandle;

  fileLib = new QLibrary(FS_LIB_NAME);
  if (!fileLib) {
    goto loadLibraryFail;
  }

  optHandle = (fs_opt_init_t)fileLib->resolve(FS_LIB_SYMBOL);
  if (!optHandle) {
    goto loadLibraryFail;
  }

  fileOpt = new fs_opt_t;
  if (!fileOpt) {
    goto loadLibraryFail;
  }

  if (optHandle(fileOpt) != 0) {
    goto loadLibraryFail;
  }

  return true;

 loadLibraryFail:

  unloadLibrary();

  return false;
}

void FsEngine::unloadLibrary()
{
  if (fileOpt) {
    delete fileOpt;
    fileOpt  = NULL;
  }

  if (fileLib) {
    delete fileLib;
    fileLib = NULL;
  }
}

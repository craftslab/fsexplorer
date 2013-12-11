/**
 * fsengine.cpp - The entry of fsengine
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the distribution
 * in the file COPYING); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <QLibrary>
#include <QtGui>

#include "fsengine.h"

#define LIB_NAME "libfs"
#define LIB_SYMBOL "fs_opt_init"

static const char* fileTypeList[] = {
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
  dev = (const char*)ba.data();

  dir = (const char*)"/";
  len = sizeof(fileTypeList) / sizeof(const char*);

  fileRoot = (struct fs_dirent *)malloc(sizeof(struct fs_dirent));
  if (!fileRoot) {
    goto openFileFail;
  }

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
    free(fileRoot);
    fileRoot = NULL;
  }

  unloadLibrary();

  return true;
}

QString FsEngine::getFileType()
{
  if (!fileType) {
    return QString("N/A");
  }

  return *fileType;
}

void FsEngine::dumpInfo()
{
  qDebug() << *fileName;
  qDebug() << *fileMount;
  qDebug() << *fileType;
}

bool FsEngine::loadLibrary()
{
  fs_opt_init_t optHandle;

  fileLib = new QLibrary(LIB_NAME);
  if (!fileLib) {
    goto loadLibraryExit;
  }

  optHandle = (fs_opt_init_t)fileLib->resolve(LIB_SYMBOL);
  if (!optHandle) {
    goto loadLibraryExit;
  }

  fileOpt = new fs_opt_t;
  if (!fileOpt) {
    goto loadLibraryExit;
  }

  if (optHandle(fileOpt) != 0) {
    goto loadLibraryExit;
  }

  return true;

 loadLibraryExit:

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

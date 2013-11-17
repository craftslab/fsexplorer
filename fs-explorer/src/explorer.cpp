/**
 * explorer.cpp - The entry of explorer
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

#include "explorer.h"

#define LIB_NAME "libfs"
#define LIB_SYMBOL "fs_opt_init"

static const char* fileTypeList[] = {
  FS_TYPE_EXT4,
  FS_TYPE_FAT,
};

Explorer::Explorer(QWidget *parent)
{
  parent = parent;

  fileLib = NULL;
  fileOpt = NULL;
  fileName = NULL;
  fileMount = NULL;
  fileType = NULL;
}

bool Explorer::openFile(QString &name)
{
  const char *dev = NULL, *dir = NULL, *type = NULL;
  int32_t i, len;
  int32_t ret;

  if (!loadLibrary()) {
    goto openFileFail;
  }

  if (!fileOpt->mount || !fileOpt->umount) {
    goto openFileFail;
  }

  dev = (const char*)name.toLatin1().data();
  dir = (const char*)"foo";
  len = sizeof(fileTypeList) / sizeof(const char*);

  for (i = 0; i < len; ++i) {
    type = fileTypeList[i];
    ret = fileOpt->mount(dev, dir, type, 0, NULL);
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

  return true;

openFileFail:

  closeFile();

  return false;
}

bool Explorer::closeFile()
{
  if (fileOpt->umount && fileMount) {
    (void)fileOpt->umount((const char *)fileMount->toLatin1().data(), 0);
  }

  unloadLibrary();

  if (fileType) {
    delete fileType;
    fileType = NULL;
  }

  if (fileMount) {
    delete fileMount;
    fileMount = NULL;
  }

  if (fileName) {
    delete fileName;
    fileName = NULL;
  }

  return true;
}

QString Explorer::getFileType()
{
  return *fileType;
}

void Explorer::dumpInfo()
{
  qDebug() << *fileName;
  qDebug() << *fileMount;
  qDebug() << *fileType;
}

bool Explorer::loadLibrary()
{
  fs_opt_init_t optHandle;

  fileLib = new QLibrary(LIB_NAME);

  optHandle = (fs_opt_init_t)fileLib->resolve(LIB_SYMBOL);
  if (!optHandle) {
    goto loadLibraryExit;
  }

  fileOpt = new fs_opt_t;
  if (optHandle(fileOpt) != 0) {
    goto loadLibraryExit;
  }

  return true;

 loadLibraryExit:

  unloadLibrary();

  return false;
}

void Explorer::unloadLibrary()
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

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

#include <QtGui>

#include "explorer.h"

static const char* fileTypeList[] = {
  FS_TYPE_EXT4,
  FS_TYPE_FAT,
};

Explorer::Explorer(QWidget *parent)
{
  parent = parent;

  fileName = QString();
  fileMount = QString();
  fileType = QString();
  memset((void *)&fileOpt, 0, sizeof(fs_opt_t));
}

bool Explorer::openFile(QString &name)
{
  const char *dev = NULL, *dir = NULL, *type = NULL;
  int32_t i, len;
  int32_t ret;

  if (!initOpt(&fileOpt)) {
    goto openFileFail;
  }

  if (!fileOpt.mount || !fileOpt.umount) {
    goto openFileFail;
  }

  dev = (const char*)name.data();
  dir = (const char*)"foo";
  len = sizeof(fileTypeList) / sizeof(const char*);

  for (i = 0; i < len; ++i) {
    type = fileTypeList[i];
    ret = fileOpt.mount(dev, dir, type, 0, NULL);
    if (ret == 0) {
      break;
    }
  }

  if (ret != 0) {
    goto openFileFail;
  }

  fileName = name;
  fileMount = QString(dir);
  fileType = QString(type);

  return true;

openFileFail:

  if (fileOpt.umount) {
    (void)fileOpt.umount(dir, 0);
  }

  deinitOpt();

  return false;
}

bool Explorer::closeFile()
{
  if (fileOpt.umount) {
    (void)fileOpt.umount((const char *)fileMount.data(), 0);
  }

  deinitOpt();

  return true;
}

QString Explorer::getFileType()
{
  return fileType;
}

void Explorer::dumpInfo()
{
  qDebug() << fileName;
  qDebug() << fileMount;
  qDebug() << fileType;
}

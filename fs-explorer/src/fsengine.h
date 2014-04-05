/**
 * fsengine.h - Header of fsengine
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

#ifndef FSENGINE_H
#define FSENGINE_H

#include <QObject>
#include <QLibrary>  

#ifdef __cplusplus
extern "C" {
#include "libfs.h"
}
#endif /* __cplusplus */

class FsEngine : public QObject
{
  Q_OBJECT

public:
  FsEngine(QWidget *parent = 0);
  ~FsEngine();

  bool openFile(QString &name);
  bool closeFile();
  QString getFileType() const;
  QString getFileStat() const;
  struct fs_dirent getFileRoot() const;
  void initFileChilds(unsigned long long ino);
  void deinitFileChilds();
  unsigned int getFileChildsNum() const;
  struct fs_dirent getFileChilds(unsigned int index) const;
  struct fs_kstat getFileChildsStat(unsigned long long ino) const;

private:
  bool loadLibrary();
  void unloadLibrary();

  QLibrary *fileLib;
  fs_opt_t *fileOpt;
  QString *fileName;
  QString *fileMount;
  QString *fileType;
  struct fs_dirent *fileRoot;
  struct fs_dirent *fileParent;
  struct fs_dirent *fileChilds;
  unsigned int fileChildsNum;
};
#endif

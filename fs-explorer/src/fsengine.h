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
#include <QMutexLocker>

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

  bool openFile(const QString &name);
  bool closeFile();
  bool isReadOnly() const;
  QString getFileType() const;
  struct fs_kstatfs getFileStat();
  QString getFileStatDetail();
  struct fs_dirent getFileRoot() const;
  unsigned int getFileChildsNum(unsigned long long ino);
  bool getFileChilds(unsigned long long ino, struct fs_dirent *childs, unsigned int num);
  struct fs_kstat getFileChildsStat(unsigned long long ino);
  QString getFileChildsStatDetail(unsigned long long ino);

private:
  bool loadLibrary();
  void unloadLibrary();

  static const char* fileTypeList[];

  QLibrary *fileLib;
  fs_opt_t *fileOpt;
  QString *fileName;
  QString *fileMount;
  QString *fileType;
  struct fs_dirent *fileRoot;

  QMutex mutex;
  bool readOnly;
};
#endif

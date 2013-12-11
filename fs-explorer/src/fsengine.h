/**
 * fsengine.h - Header of fsengine
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

#ifndef FSENGINE_H
#define FSENGINE_H

#include <QObject>
#include <QLibrary>  

#include "libfs.h"

class FsEngine : public QObject
{
  Q_OBJECT

public:
  FsEngine(QWidget *parent = 0);
  ~FsEngine();

  bool openFile(QString &name);
  bool closeFile();
  QString getFileType();
  void dumpInfo();

private:
  bool loadLibrary();
  void unloadLibrary();

  QLibrary *fileLib;
  fs_opt_t *fileOpt;
  QString *fileName;
  QString *fileMount;
  QString *fileType;
  struct fs_dirent *fileRoot;
};
#endif

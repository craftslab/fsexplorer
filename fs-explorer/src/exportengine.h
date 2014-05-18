/**
 * exportengine.h - Header of exportengine
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

#ifndef EXPORTENGINE_H
#define EXPORTENGINE_H

#include <QString>
#include <QProgressDialog>

#include "fsengine.h"

class ExportEngine : public QObject
{
  Q_OBJECT

public:
  ExportEngine(const QString &title, const QList<unsigned long long> &list, const QString &name, FsEngine *engine, QWidget *parent = 0);
  ~ExportEngine();

private:
  unsigned int count(unsigned long long ino);
  bool traverse(unsigned long long ino);
  bool handleExport(unsigned long long ino);

  QProgressDialog *progress;
  FsEngine *fsEngine;
  QList<unsigned long long> fileList;
  QString filePath;
  int fileCounter;
};
#endif

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
#include <QDir>
#include <QProgressDialog>
#include <QMessageBox>

#include "fsengine.h"

class ExportEngine : public QObject
{
  Q_OBJECT

public:
  ExportEngine(const QString &title, const QList<unsigned long long> &list, const QString &path, FsEngine *engine, QWidget *parent = 0);
  ~ExportEngine();

private:
  unsigned int count(unsigned long long ino);
  bool traverse(unsigned long long ino, const QStringList &address);
  bool handleExport(unsigned long long ino, const QStringList &address);
  bool confirm(const QString &name, enum libfs_ftype type);

  QProgressDialog *progress;
  FsEngine *fsEngine;
  QDir *filePath;
  int fileCounter;
};
#endif

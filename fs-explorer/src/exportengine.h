/**
 * exportengine.h - Header of exportengine
 *
 * Copyright (c) 2014-2015 angersax@gmail.com
 *
 * This file is part of Fs Explorer.
 *
 * Fs Explorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fs Explorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fs Explorer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXPORTENGINE_H
#define EXPORTENGINE_H

#include <QString>
#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "fsengine.h"

class ExportEngine : public QObject
{
  Q_OBJECT

public:
  ExportEngine(const QList<unsigned long long> &list, const QString &path, FsEngine *engine);
  ~ExportEngine();

  int count();

signals:
  void current(int num);
  void message(const QString text);
  void finished();

public slots:
  void process();

private:
  unsigned int countHelper(unsigned long long ino);
  bool traverse(unsigned long long ino, const QStringList &address);
  bool traverseHelper(unsigned long long ino, const QStringList &address);
  bool exportNoConfirm(unsigned long long ino, const QString &name);
  bool exportWithConfirm(unsigned long long ino, const QString &name);
  bool exportDir(const QString &name);
  bool exportFile(unsigned long long ino, const QString &name);
  bool exportLink(unsigned long long ino, const QString &name);
  QFileDevice::Permissions getFilePermissions(unsigned long long ino);
  bool showError(const QString &msg);

  QList<unsigned long long> fileList;
  QDir *filePath;
  int fileCounter;
  char *fileBuf;
  FsEngine *fsEngine;

  static const int size;
};
#endif

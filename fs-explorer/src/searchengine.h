/**
 * searchengine.h - Header of searchengine
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
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

#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QObject>
#include <QLibrary>
#include <QThread>
#include <QMutex>
#include <QStringList>

#include "fsengine.h"

class SearchEngine : public QThread
{
  Q_OBJECT

public:
  SearchEngine(FsEngine *engine, QObject *parent = 0);
  ~SearchEngine();

signals:
  void found(const QStringList &address);

public slots:
  void search(const QString &name);
  void stop();

protected:
  void run();

private:
  void traverse(const struct fs_dirent &dent, const QStringList &address);

  FsEngine *fsEngine;
  QString searchName;
  bool searchStopped;
};
#endif

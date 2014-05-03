/**
 * searchengine.h - Header of searchengine
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

#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QObject>
#include <QLibrary>  
#include <QThread>
#include <QMutex>

#include "fsengine.h"

class SearchThread;

class SearchEngine : public QObject
{
  Q_OBJECT

public:
  SearchEngine(FsEngine *engine, QObject *parent = 0);
  ~SearchEngine();

signals:
  void found(const QString &name);
  void finished();

public slots:
  void search(const QString &name);
  void stop();

private slots:
  void handleFound(const QString &name);
  void handleFinished();

private:
  SearchThread *searchThread;
};

class SearchThread : public QThread
{
  Q_OBJECT

public:
  SearchThread(FsEngine *engine, QObject *parent = 0);
  ~SearchThread();

signals:
  void found(const QString &name);

protected:
  void run();

private:
  FsEngine *fsEngine;
};
#endif

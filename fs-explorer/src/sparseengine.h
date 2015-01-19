/**
 * sparseengine.h - Header of sparseengine
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

#ifndef SPARSEENGINE_H
#define SPARSEENGINE_H

#include <QObject>
#include <QLibrary>
#include <QThread>
#include <QMutexLocker>
#include <QFile>

#ifdef __cplusplus
extern "C" {
#include "sparse/sparse.h"
}
#endif /* __cplusplus */

class SparseEngine : public QThread
{
  Q_OBJECT

public:
  SparseEngine(QObject *parent = 0);
  ~SparseEngine();

signals:
  void finished();

public slots:
  void unsparse(const QFile *src, QFile *dst);
  void stop();

protected:
  void run();

private:
  const QFile *srcFile;
  QFile *dstFile;
  bool isStopped;

  QMutex mutex;
};
#endif

/**
 * sparseengine.h - Header of sparseengine
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

#ifndef SPARSEENGINE_H
#define SPARSEENGINE_H

#include <QObject>
#include <QLibrary>
#include <QFile>
#include <QTemporaryFile>

#ifdef __cplusplus
extern "C" {
#include "sparse_format.h"
#include "sparse/sparse.h"
}
#endif /* __cplusplus */

class SparseEngine : public QObject
{
  Q_OBJECT

public:
  SparseEngine(const QString &name);
  ~SparseEngine();

  static bool isSparseFile(const QString &name);
  int count();

signals:
  void current(int num);
  void message(const QString &text);
  void finished(const QString &name);

public slots:
  void process();

private:
  QFile *srcFile;
  QTemporaryFile *dstFile;
};
#endif

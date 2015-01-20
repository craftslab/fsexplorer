/**
 * sparseengine.cpp - The entry of sparseengine
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

#include "sparseengine.h"

SparseEngine::SparseEngine(const QString &name, QObject *parent)
{
  srcFile = new QFile(name);
  parent = parent;

  dstFile = new QTemporaryFile();
  dstFile->setAutoRemove(false);
}

SparseEngine::~SparseEngine()
{
  handleStop();
}

void SparseEngine::handleStop()
{
  if (dstFile) {
    dstFile->close();
    dstFile = NULL;
  }

  if (srcFile) {
    srcFile->close();
    srcFile = NULL;
  }
}

void SparseEngine::run()
{
  QMutexLocker locker(&mutex);

  int srcHandle = srcFile->handle();
  struct sparse_file *s = sparse_file_import(srcHandle, false, false);
  if (!s) {
    exit(-1);
  }

  int dstHandle = dstFile->handle();
  dstFile->seek(0);
  int ret = sparse_file_write(s, dstHandle, false, false, false);
  if (ret < 0) {
    exit(ret);
  }

  sparse_file_destroy(s);

  emit resultReady(dstFile->fileName());
}

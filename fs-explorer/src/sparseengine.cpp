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

static bool unsparse(const QFile *src, QTemporaryFile *dst);

static bool unsparse(const QFile *src, QTemporaryFile *dst)
{
  int srcHandle = src->handle();
  bool status = true;

  struct sparse_file *s = sparse_file_import(srcHandle, false, false);
  if (!s) {
    return false;
  }

  int dstHandle = dst->handle();
  dst->seek(0);

  int ret = sparse_file_write(s, dstHandle, false, false, false);
  if (ret < 0) {
    status = false;
  }

  dst->flush();

  sparse_file_destroy(s);

  return status;
}

SparseEngine::SparseEngine(const QString &name, QObject *parent)
{
  srcFile = new QFile(name);
  srcFile->open(QIODevice::ReadOnly);

  dstFile = new QTemporaryFile();
  dstFile->setAutoRemove(false);
  dstFile->open();

  parent = parent;
}

SparseEngine::~SparseEngine()
{
  handleStop();
}

bool SparseEngine::isSparseFile(const QString &src)
{
  QFile file(src);
  sparse_header_t header;
  qint64 ret;

  file.open(QIODevice::ReadOnly);

  memset(&header, 0, sizeof(header));

  file.seek(0);
  ret = file.read((char *)&header, sizeof(header));
  if (ret < 0) {
    file.close();
    return false;
  }
  file.close();

  if (header.magic != SPARSE_HEADER_MAGIC) {
    return false;
  }

  return true;
}

bool SparseEngine::unsparseFile(const QString &src, QString &dst)
{
  QFile inFile(src);
  inFile.open(QIODevice::ReadOnly);

  QTemporaryFile outFile;
  outFile.open();
  outFile.setAutoRemove(false);
  QString name = outFile.fileName();

  bool ret = ::unsparse(&inFile, &outFile);
  if (!ret) {
    inFile.close();
    (void)QFile::remove(name);

    return false;
  }

  inFile.close();
  outFile.close();
  dst = name;

  return true;
}

void SparseEngine::handleStop()
{
  if (dstFile) {
    QString name = dstFile->fileName();

    dstFile->flush();
    dstFile->close();
    (void)QFile::remove(name);

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
  QString name = dstFile->fileName();

  bool ret = ::unsparse(srcFile, dstFile);
  if (!ret) {
    name.clear();
    handleStop();
  }

  emit resultReady(name);
}

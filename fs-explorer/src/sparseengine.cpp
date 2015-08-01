/**
 * sparseengine.cpp - The entry of sparseengine
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

#include "sparseengine.h"

SparseEngine::SparseEngine(const QString &name)
{
  srcFile = new QFile(name);
  srcFile->open(QIODevice::ReadOnly);

  dstFile = new QTemporaryFile();
  dstFile->setAutoRemove(false);
  dstFile->open();
}

SparseEngine::~SparseEngine()
{
  if (dstFile) {
    delete dstFile;
    dstFile = NULL;
  }

  if (srcFile) {
    delete srcFile;
    srcFile = NULL;
  }
}

bool SparseEngine::isSparseFile(const QString &name)
{
#if 0
  QFile file(name);
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
#else
  return false;
#endif
}

int SparseEngine::count()
{
  return 0;
}

const QString SparseEngine::unsparseFile()
{
  return dstFile->fileName();
}

void SparseEngine::process()
{
#if 0
  int srcHandle = srcFile->handle();
  int dstHandle = dstFile->handle();
  QString dstName = dstFile->fileName();
  struct sparse_file *s = NULL;
  int ret;
  bool status;

  s = sparse_file_import(srcHandle, false, false);
  if (!s) {
    status = false;
    goto processExit;
  }

  dstFile->seek(0);

  ret = sparse_file_write(s, dstHandle, false, false, false);
  if (ret < 0) {
    status = false;
    goto processExit;
  }

  dstFile->flush();

  sparse_file_destroy(s);

  status = true;

 processExit:

  dstFile->close();
  srcFile->close();

  if (status) {
    emit message(QString(tr("Completed.")));
  } else {
    (void)QFile::remove(dstName);
    dstName.clear();
    emit message(QString(tr("Aborted.")));
  }

  emit finished();
#endif
}

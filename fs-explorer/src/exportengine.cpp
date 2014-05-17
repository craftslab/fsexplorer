/**
 * exportengine.cpp - The entry of exportengine
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

#include "exportengine.h"

ExportEngine::ExportEngine(const QString &title, const QList<unsigned long long> &list, const QString &name, FsEngine *engine, QWidget *parent)
{
  bool ret;

  progress = new QProgressDialog(parent);

  progress->setWindowTitle(title);
  progress->setWindowModality(Qt::WindowModal);

  QSize sizeProgress = progress->size();
  sizeProgress.setWidth(sizeProgress.width() << 1);
  progress->resize(sizeProgress);

  fsEngine = engine;
  exportName = name;

  for (int i = 0; i < list.size(); ++i) {
    ret = traverse(list[i]);
    if (!ret) {
      break;
    }
  }
}

ExportEngine::~ExportEngine()
{
  if (progress) {
    delete progress;
    progress = NULL;
  }
}

bool ExportEngine::traverse(unsigned long long ino)
{
  unsigned int num;
  bool ret;

  ret = exportFile(ino);
  if (!ret) {
    return false;
  }

  num = fsEngine->getFileChildsNum(ino);
  if (num == 0) {
    return true;
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return false;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  ret = fsEngine->getFileChildsList(ino, childs, num);
  if (!ret) {
    goto traverseExit;
  }

  for (int i = 0; i < (int)num; ++i) {
    ret = traverse(childs[i].d_ino);
    if (!ret) {
      break;
    }
  }

traverseExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }

  return ret;
}

bool ExportEngine::exportFile(unsigned long long ino)
{
  struct fs_dirent dent = fsEngine->getFileChildsDent(ino);
  struct fs_kstat stat = fsEngine->getFileChildsStat(ino);
  int size = static_cast<int> (stat.size);

  progress->setLabelText(QString(tr(dent.d_name)));
  progress->setRange(0, size);

  for (int i = 0; i < size; i++) {
    progress->setValue(i);
    if (progress->wasCanceled()) {
      break;
    }
  }
  progress->setValue(size);

  return true;
}

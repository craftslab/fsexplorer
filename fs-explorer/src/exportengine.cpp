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
  int num = 0;
  int ret;

  progress = new QProgressDialog(parent);

  progress->setWindowTitle(title);
  progress->setWindowModality(Qt::WindowModal);
  progress->setMinimumDuration(0);

  QSize sizeProgress = progress->size();
  sizeProgress.setWidth(parent->width() >> 1);
  progress->resize(sizeProgress);

  QPoint pointProgress = parent->pos();
  pointProgress.setX(pointProgress.x() + ((parent->width() - sizeProgress.width()) >> 1));
  pointProgress.setY(pointProgress.y() + ((parent->height() - sizeProgress.height()) >> 1));
  progress->move(pointProgress);

  fsEngine = engine;
  fileList = list;
  filePath = name;
  fileCounter = 0;

  for (int i = 0; i < fileList.size(); ++i) {
    num += (int)count(fileList[i]);
  }
  progress->setRange(0, num);

  for (int i = 0; i < fileList.size(); ++i) {
    ret = traverse(fileList[i]);
    if (!ret) {
      break;
    }
  }
  progress->setValue(num);
}

ExportEngine::~ExportEngine()
{
  if (progress) {
    delete progress;
    progress = NULL;
  }
}

unsigned int ExportEngine::count(unsigned long long ino)
{
  struct fs_dirent dent;
  unsigned int num, len;

  dent = fsEngine->getFileChildsDent(ino);
  num = fsEngine->getFileChildsNum(ino);

  if ((dent.d_type == FT_DIR && num == 2)
      || (dent.d_type != FT_DIR && num == 0)) {
    return 1;
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return num;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  bool ret = fsEngine->getFileChildsList(ino, childs, num);
  if (!ret) {
    goto countExit;
  }

  len = num;
  for (unsigned int i = 0; i < len; ++i) {
    if (!strcmp((const char *)childs[i].d_name, (const char *)FS_DNAME_DOT)
        || !strcmp((const char *)childs[i].d_name, (const char *)FS_DNAME_DOTDOT)) {
      continue;
    }

    num += count(childs[i].d_ino);
  }

countExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }

  return num;
}

bool ExportEngine::traverse(unsigned long long ino)
{
  struct fs_dirent dent;
  unsigned int num;
  bool ret;

  ret = handleExport(ino);
  if (!ret) {
    return false;
  }

  dent = fsEngine->getFileChildsDent(ino);
  num = fsEngine->getFileChildsNum(ino);

  if ((dent.d_type == FT_DIR && num == 2)
      || (dent.d_type != FT_DIR && num == 0)) {
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

  for (unsigned int i = 0; i < num; ++i) {
    if (!strcmp((const char *)childs[i].d_name, (const char *)FS_DNAME_DOT)
        || !strcmp((const char *)childs[i].d_name, (const char *)FS_DNAME_DOTDOT)) {
      continue;
    }

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

bool ExportEngine::handleExport(unsigned long long ino)
{
  struct fs_dirent dent = fsEngine->getFileChildsDent(ino);
  struct fs_kstat stat = fsEngine->getFileChildsStat(ino);
  int size = static_cast<int> (stat.size);

  progress->setLabelText(QString(tr(dent.d_name)));
  progress->setValue(++fileCounter);

  return true;
}

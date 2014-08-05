/**
 * chartengine.cpp - The entry of chartengine
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

#include "chartengine.h"

ChartEngine::ChartEngine(FsEngine *engine, QWidget *parent)
{
  fsEngine = engine;
  parent = parent;
}

ChartEngine::~ChartEngine()
{
  // Do nothing here
}

void ChartEngine::capacityList(QList<int64_t> &capList)
{
  struct fs_kstatfs stat = fsEngine->getFileStat();

  int64_t sizeFree = stat.f_bfree * stat.f_bsize;
  int64_t sizeUsed = stat.f_blocks * stat.f_bsize - sizeFree;

  capList << sizeUsed << sizeFree;
}

void ChartEngine::sizeRankingList(QStringList &nameList, QList<int64_t> &sizeList, int listLen)
{
  struct fs_dirent root = fsEngine->getFileRoot();

  traverse(root, nameList, sizeList, listLen);
}

void ChartEngine::traverse(const struct fs_dirent &dent, QStringList &nameList, QList<int64_t> &sizeList, int listLen)
{
  unsigned long long ino = dent.d_ino;
  unsigned int num = fsEngine->getFileChildsNum(ino);

  if (num == 0) {
    return;
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  bool ret = fsEngine->getFileChildsList(ino, childs, num);
  if (!ret) {
    goto traverseExit;
  }

  for (int i = 0; i < (int)num; ++i) {
    QString name = QObject::tr(childs[i].d_name);

    if (!QString::compare(name, QString(FS_DNAME_DOT))
        || !QString::compare(name, QString(FS_DNAME_DOTDOT))) {
      continue;
    }

    if (childs[i].d_type != FT_DIR) {
      struct fs_kstat stat = fsEngine->getFileChildsStat((unsigned long long)childs[i].d_ino);

      int index = insertSizeList(sizeList, stat.size, listLen);
      if (index >= 0) {
        nameList.insert(index, name);

        if (nameList.size() > listLen) {
          nameList.removeLast();
        }
      }
    }

    traverse(childs[i], nameList, sizeList, listLen);
  }

traverseExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }
}

int ChartEngine::insertSizeList(QList<int64_t> &sizeList, int64_t item, int listLen)
{
  int index = -1;
  int i;

  if (sizeList.size() == 0) {
    sizeList << item;
    return 0;
  }

  if ((item <= sizeList.last()) && (sizeList.size() == listLen)) {
    return -1;
  }

  for (i = sizeList.size() - 1; i >= 0; --i) {
    if (item <= sizeList.at(i)) {
      index = i + 1;
      sizeList.insert(index, item);
      
      if (sizeList.size() > listLen) {
        sizeList.removeLast();
      }

      break;
    }
  }

  if (i < 0) {
    index = 0;
    sizeList.insert(index, item);

    if (sizeList.size() > listLen) {
      sizeList.removeLast();
    }
  }

  return index;
}

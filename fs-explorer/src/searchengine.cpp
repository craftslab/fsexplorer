/**
 * searchengine.cpp - The entry of searchengine
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

#include "searchengine.h"

SearchEngine::SearchEngine(FsEngine *engine, QObject *parent)
{
  fsEngine = engine;
  parent = parent;
}

SearchEngine::~SearchEngine()
{
  stop();
}

void SearchEngine::search(const QString &name)
{
  if (name.isEmpty()) {
    return;
  }

  searchName = name;
  start();
}

void SearchEngine::stop()
{
  // TODO
}

void SearchEngine::run()
{
  unsigned long long ino = fsEngine->getFileRoot().d_ino;
  traverse(ino);
}

void SearchEngine::traverse(unsigned long long ino)
{
  unsigned int num = fsEngine->getFileChildsNum(ino);
  if (num == 0) {
    return;
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  bool ret = fsEngine->getFileChilds(ino, childs, num);
  if (!ret) {
    goto traverseFail;
  }

  for (int i = (int)num - 1; i >= 0; --i) {
    QString name = childs[i].d_name;

    if (!QString::compare(name, QString(FS_DNAME_DOT))
        || !QString::compare(name, QString(FS_DNAME_DOTDOT))) {
      continue;
    }

    if (!QString::compare(name, searchName)) {
      emit found(name);
    }

    traverse(childs[i].d_ino);
  }

traverseFail:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }
}

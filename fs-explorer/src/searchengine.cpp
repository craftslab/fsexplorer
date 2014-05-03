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
  // Do nothing here
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
  fsEngine->initFileChilds(ino);

  unsigned int childsNum = fsEngine->getFileChildsNum();
  if (childsNum == 0) {
    fsEngine->deinitFileChilds();
    return;
  }

  for (int i = (int)childsNum - 1; i >= 0; --i) {
    struct fs_dirent child = fsEngine->getFileChilds((unsigned int)i);
    QString childName = child.d_name;

    if (!QString::compare(childName, searchName)) {
      emit found(childName);
    }

    traverse(child.d_ino);
  }

  fsEngine->deinitFileChilds();
}

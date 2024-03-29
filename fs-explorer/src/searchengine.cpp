/**
 * searchengine.cpp - The entry of searchengine
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

#include "searchengine.h"

SearchEngine::SearchEngine(FsEngine *engine, QObject *parent)
{
  fsEngine = engine;
  parent = parent;

  searchName.clear();
  searchStopped = true;
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
  searchStopped = false;

  start();
}

void SearchEngine::stop()
{
  searchStopped = true;
}

void SearchEngine::run()
{
  struct fs_dirent root = fsEngine->getFileRoot();
  QStringList address;

  address << QString(tr(root.d_name));
  traverse(root, address);
}

void SearchEngine::traverse(const struct fs_dirent &dent, const QStringList &address)
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
    if (searchStopped) {
      break;
    }

    QString name = QObject::tr(childs[i].d_name);

    if (!QString::compare(name, QString(FS_DNAME_DOT))
        || !QString::compare(name, QString(FS_DNAME_DOTDOT))) {
      continue;
    }

    QStringList list = address;
    list << name;

#if 0 // DISUSED here
    if (!QString::compare(name, searchName)) {
      emit found(list);
    }
#else
    QRegExp rx(searchName);
    rx.setPatternSyntax(QRegExp::Wildcard);
    if (rx.exactMatch(name)) {
      emit found(list);
    }
#endif

    traverse(childs[i], list);
  }

traverseExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }
}

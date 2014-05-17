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
  progress = new QProgressDialog(parent);

  progress->setWindowTitle(title);
  progress->setWindowModality(Qt::WindowModal);

  QSize sizeProgress = progress->size();
  sizeProgress.setWidth(sizeProgress.width() << 1);
  progress->resize(sizeProgress);

  fsEngine = engine;
  exportName = name;

  traverse(list);
}

void ExportEngine::traverse(const QList<unsigned long long> &list)
{
  int numFiles = 100000;

  //progress->setLabelText(title);
  progress->setMinimum(0);
  progress->setMaximum(numFiles);

  for (int i = 0; i < numFiles; i++) {
    progress->setValue(i);
    if (progress->wasCanceled()) {
      break;
    }
  }
  progress->setValue(numFiles);
}

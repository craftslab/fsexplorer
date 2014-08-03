/**
 * chartengine.h - Header of chartengine
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

#ifndef CHARTENGINE_H
#define CHARTENGINE_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

#include "fsengine.h"

class ChartEngine : public QObject
{
  Q_OBJECT

public:
  ChartEngine(FsEngine *engine, QWidget *parent = 0);
  ~ChartEngine();

  void capacityList(QList<int64_t> &capList);
  int sizeRankingList(QStringList &nameList, QList<int64_t> &sizeList, int listLen);

private:
  FsEngine *fsEngine;
};
#endif

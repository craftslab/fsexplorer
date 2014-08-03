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

void ChartEngine::capacityList(QList<int> &capList)
{
  capList << 30 << 70;
}

int ChartEngine::sizeRankingList(QStringList &nameList, QList<int> &sizeList, int listLen)
{
  for (int i = 0; i < listLen; ++i) {
    nameList << "bar";
  }

  int size = 100;
  for (int i = 0; i < listLen; ++i) {
    sizeList << size;
    size -= 10;
  }

  return listLen;
}

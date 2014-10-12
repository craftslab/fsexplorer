/**
 * fslistitem.h - Header of fslistitem
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
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

#ifndef FSLISTITEM_H
#define FSLISTITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QStringList>

class FsListItem
{
public:
  FsListItem(const QVector<QVariant> &data, FsListItem *parent = 0);
  ~FsListItem();

  FsListItem *child(int number);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  bool insertChildren(int position, int count, int columns);
  bool insertColumns(int position, int columns);
  FsListItem *parent();
  bool removeChildren(int position, int count);
  bool removeColumns(int position, int columns);
  int childNumber() const;
  bool setData(int column, const QVariant &value);

private:
  QList<FsListItem*> childItems;
  QVector<QVariant> itemData;
  FsListItem *parentItem;
};
#endif

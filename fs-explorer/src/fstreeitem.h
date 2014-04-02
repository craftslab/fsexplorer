/**
 * fstreeitem.h - Header of fstreeitem
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

#ifndef FSTREEITEM_H
#define FSTREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QStringList>

class FsTreeItem
{
public:
  FsTreeItem(const QVector<QVariant> &data, FsTreeItem *parent = 0);
  ~FsTreeItem();

  FsTreeItem *child(int number);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  bool insertChildren(int position, int count, int columns);
  bool insertColumns(int position, int columns);
  FsTreeItem *parent();
  bool removeChildren(int position, int count);
  bool removeColumns(int position, int columns);
  int childNumber() const;
  bool setData(int column, const QVariant &value);

private:
  QList<FsTreeItem*> childItems;
  QVector<QVariant> itemData;
  FsTreeItem *parentItem;
};
#endif

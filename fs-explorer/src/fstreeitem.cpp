/**
 * fstreeitem.cpp - The entry of fstreeitem
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the distribution
 * in the file COPYING); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <QStringList>

#include "fstreeitem.h"

FsTreeItem::FsTreeItem(const QList<QVariant> &data, FsTreeItem *parent)
{
  parentItem = parent;
  itemData = data;
}

FsTreeItem::~FsTreeItem()
{
  qDeleteAll(childItems);
}

void FsTreeItem::appendChild(FsTreeItem *item)
{
  childItems.append(item);
}

FsTreeItem *FsTreeItem::child(int row)
{
  return childItems.value(row);
}

int FsTreeItem::childCount() const
{
  return childItems.count();
}

int FsTreeItem::columnCount() const
{
  return itemData.count();
}

QVariant FsTreeItem::data(int column) const
{
  return itemData.value(column);
}

FsTreeItem *FsTreeItem::parent()
{
  return parentItem;
}

int FsTreeItem::row() const
{
  if (parentItem) {
    return parentItem->childItems.indexOf(const_cast<FsTreeItem*>(this));
  }

  return 0;
}

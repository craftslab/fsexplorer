/**
 * fstreeitem.h - Header of fstreeitem
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

#ifndef FSTREEITEM_H
#define FSTREEITEM_H

#include <QList>
#include <QVariant>

class FsTreeItem
{
public:
  FsTreeItem(const QList<QVariant> &data, FsTreeItem *parent = 0);
  ~FsTreeItem();

  void appendChild(FsTreeItem *child);

  FsTreeItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  int row() const;
  FsTreeItem *parent();

private:
  QList<FsTreeItem*> childItems;
  QList<QVariant> itemData;

  FsTreeItem *parentItem;
};
#endif

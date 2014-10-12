/**
 * fstreeitem.cpp - The entry of fstreeitem
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

#include "fstreeitem.h"

FsTreeItem::FsTreeItem(const QVector<QVariant> &data, FsTreeItem *parent)
{
  parentItem = parent;
  itemData = data;
}

FsTreeItem::~FsTreeItem()
{
  qDeleteAll(childItems);
}

FsTreeItem *FsTreeItem::child(int number)
{
  return childItems.value(number);
}

int FsTreeItem::childCount() const
{
  return childItems.count();
}

int FsTreeItem::childNumber() const
{
  if (parentItem) {
    return parentItem->childItems.indexOf(const_cast<FsTreeItem*>(this));
  }

  return 0;
}

int FsTreeItem::columnCount() const
{
  return itemData.count();
}

QVariant FsTreeItem::data(int column) const
{
  return itemData.value(column);
}

bool FsTreeItem::insertChildren(int position, int count, int columns)
{
  if (position < 0 || position > childItems.size()) {
    return false;
  }

  for (int row = 0; row < count; ++row) {
    QVector<QVariant> data(columns);
    FsTreeItem *item = new FsTreeItem(data, this);
    childItems.insert(position, item);
  }

  return true;
}

bool FsTreeItem::insertColumns(int position, int columns)
{
  if (position < 0 || position > itemData.size()) {
    return false;
  }

  for (int column = 0; column < columns; ++column) {
    itemData.insert(position, QVariant());
  }

  foreach (FsTreeItem *child, childItems) {
    child->insertColumns(position, columns);
  }

  return true;
}

FsTreeItem *FsTreeItem::parent()
{
  return parentItem;
}

bool FsTreeItem::removeChildren(int position, int count)
{
  if (position < 0 || position + count > childItems.size()) {
    return false;
  }

  for (int row = 0; row < count; ++row) {
    delete childItems.takeAt(position);
  }

  return true;
}

bool FsTreeItem::removeColumns(int position, int columns)
{
  if (position < 0 || position + columns > itemData.size()) {
    return false;
  }

  for (int column = 0; column < columns; ++column) {
    itemData.remove(position);
  }

  foreach (FsTreeItem *child, childItems) {
    child->removeColumns(position, columns);
  }

  return true;
}

bool FsTreeItem::setData(int column, const QVariant &value)
{
  if (column < 0 || column >= itemData.size()) {
    return false;
  }

  itemData[column] = value;

  return true;
}

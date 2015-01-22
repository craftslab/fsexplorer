/**
 * fslistitem.cpp - The entry of fslistitem
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

#include "fslistitem.h"

FsListItem::FsListItem(const QVector<QVariant> &data, FsListItem *parent)
{
  parentItem = parent;
  itemData = data;
}

FsListItem::~FsListItem()
{
  qDeleteAll(childItems);
}

FsListItem *FsListItem::child(int number)
{
  return childItems.value(number);
}

int FsListItem::childCount() const
{
  return childItems.count();
}

int FsListItem::childNumber() const
{
  if (parentItem) {
    return parentItem->childItems.indexOf(const_cast<FsListItem*>(this));
  }

  return 0;
}

int FsListItem::columnCount() const
{
  return itemData.count();
}

QVariant FsListItem::data(int column) const
{
  return itemData.value(column);
}

bool FsListItem::insertChildren(int position, int count, int columns)
{
  if (position < 0 || position > childItems.size()) {
    return false;
  }

  for (int row = 0; row < count; ++row) {
    QVector<QVariant> data(columns);
    FsListItem *item = new FsListItem(data, this);
    childItems.insert(position, item);
  }

  return true;
}

bool FsListItem::insertColumns(int position, int columns)
{
  if (position < 0 || position > itemData.size()) {
    return false;
  }

  for (int column = 0; column < columns; ++column) {
    itemData.insert(position, QVariant());
  }

  foreach (FsListItem *child, childItems) {
    child->insertColumns(position, columns);
  }

  return true;
}

FsListItem *FsListItem::parent()
{
  return parentItem;
}

bool FsListItem::removeChildren(int position, int count)
{
  if (position < 0 || position + count > childItems.size()) {
    return false;
  }

  for (int row = 0; row < count; ++row) {
    delete childItems.takeAt(position);
  }

  return true;
}

bool FsListItem::removeColumns(int position, int columns)
{
  if (position < 0 || position + columns > itemData.size()) {
    return false;
  }

  for (int column = 0; column < columns; ++column) {
    itemData.remove(position);
  }

  foreach (FsListItem *child, childItems) {
    child->removeColumns(position, columns);
  }

  return true;
}

bool FsListItem::setData(int column, const QVariant &value)
{
  if (column < 0 || column >= itemData.size()) {
    return false;
  }

  itemData[column] = value;

  return true;
}

/**
 * fslistitem.cpp - The entry of fslistitem
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

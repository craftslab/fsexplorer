/**
 * fslistitem.h - Header of fslistitem
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

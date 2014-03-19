/**
 * fslistmodel.h - Header of fslistmodel
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

#ifndef FSLISTMODEL_H
#define FSLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QPixmap>

#include "libfs.h"
#include "fslistitem.h"

class FsListModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  FsListModel(const QStringList &headers, const QStringList &data,
              QObject *parent = 0);
  FsListModel(const QStringList &headers, QObject *parent = 0);
  ~FsListModel();

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;

  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole);
  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role = Qt::EditRole);

  bool insertColumns(int position, int columns,
                     const QModelIndex &parent = QModelIndex());
  bool removeColumns(int position, int columns,
                     const QModelIndex &parent = QModelIndex());
  bool insertRows(int position, int rows,
                  const QModelIndex &parent = QModelIndex());
  bool removeRows(int position, int rows,
                  const QModelIndex &parent = QModelIndex());

private:
  void setupModelData(const QStringList &lines, FsListItem *parent);
  FsListItem *getItem(const QModelIndex &index) const;

  FsListItem *rootItem;
};
#endif

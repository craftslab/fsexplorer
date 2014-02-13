/**
 * fstreemodel.h - Header of fstreemodel
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

#ifndef FSTREEMODEL_H
#define FSTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class FsTreeItem;

class FsTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  FsTreeModel(const QStringList &headers, const QStringList &data,
              QObject *parent = 0);
  FsTreeModel(const QStringList &headers, QObject *parent = 0);
  ~FsTreeModel();

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
  void setupModelData(const QStringList &lines, FsTreeItem *parent);
  FsTreeItem *getItem(const QModelIndex &index) const;

  FsTreeItem *rootItem;
};
#endif

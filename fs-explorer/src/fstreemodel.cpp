/**
 * fstreemodel.cpp - The entry of fstreemodel
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

#include "fstreeitem.h"
#include "fstreemodel.h"

FsTreeModel::FsTreeModel(const QStringList &headers, const QStringList &data,
                         QObject *parent)
  : QAbstractItemModel(parent)
{
  QVector<QVariant> rootData;
  foreach (QString header, headers) {
    rootData << header;
  }

  rootItem = new FsTreeItem(rootData);
  setupModelData(data, rootItem);
}

FsTreeModel::~FsTreeModel()
{
  delete rootItem;
}

int FsTreeModel::columnCount(const QModelIndex & /* parent */) const
{
  return rootItem->columnCount();
}

QVariant FsTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole && role != Qt::EditRole) {
    return QVariant();
  }

  FsTreeItem *item = getItem(index);

  return item->data(index.column());
}

Qt::ItemFlags FsTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return 0;
  }

  return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

FsTreeItem *FsTreeModel::getItem(const QModelIndex &index) const
{
  if (index.isValid()) {
    FsTreeItem *item = static_cast<FsTreeItem*>(index.internalPointer());
    if (item) return item;
  }

  return rootItem;
}

QVariant FsTreeModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return rootItem->data(section);
  }

  return QVariant();
}

QModelIndex FsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() != 0) {
    return QModelIndex();
  }

  FsTreeItem *parentItem = getItem(parent);

  FsTreeItem *childItem = parentItem->child(row);
  if (childItem) {
    return createIndex(row, column, childItem);
  } else {
    return QModelIndex();
  }
}

bool FsTreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
  bool success;

  beginInsertColumns(parent, position, position + columns - 1);
  success = rootItem->insertColumns(position, columns);
  endInsertColumns();

  return success;
}

bool FsTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
  FsTreeItem *parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

QModelIndex FsTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return QModelIndex();
  }

  FsTreeItem *childItem = getItem(index);
  FsTreeItem *parentItem = childItem->parent();

  if (parentItem == rootItem) {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool FsTreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
  bool success;

  beginRemoveColumns(parent, position, position + columns - 1);
  success = rootItem->removeColumns(position, columns);
  endRemoveColumns();

  if (rootItem->columnCount() == 0) {
    removeRows(0, rowCount());
  }

  return success;
}

bool FsTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
  FsTreeItem *parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

int FsTreeModel::rowCount(const QModelIndex &parent) const
{
  FsTreeItem *parentItem = getItem(parent);

  return parentItem->childCount();
}

bool FsTreeModel::setData(const QModelIndex &index, const QVariant &value,
                          int role)
{
  if (role != Qt::EditRole) {
    return false;
  }

  FsTreeItem *item = getItem(index);
  bool result = item->setData(index.column(), value);

  if (result) {
    emit dataChanged(index, index);
  }

  return result;
}

bool FsTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                                const QVariant &value, int role)
{
  if (role != Qt::EditRole || orientation != Qt::Horizontal) {
    return false;
  }

  bool result = rootItem->setData(section, value);
  if (result) {
    emit headerDataChanged(orientation, section, section);
  }

  return result;
}

void FsTreeModel::setupModelData(const QStringList &lines, FsTreeItem *parent)
{
#if 0
  QList<FsTreeItem*> parents;
  QList<int> indentations;
  parents << parent;
  indentations << 0;

  int number = 0;

  while (number < lines.count()) {
    int position = 0;
    while (position < lines[number].length()) {
      if (lines[number].mid(position, 1) != " ") {
        break;
      }
      position++;
    }

    QString lineData = lines[number].mid(position).trimmed();

    if (!lineData.isEmpty()) {
      QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
      QVector<QVariant> columnData;
      for (int column = 0; column < columnStrings.count(); ++column) {
        columnData << columnStrings[column];
      }

      if (position > indentations.last()) {
        if (parents.last()->childCount() > 0) {
          parents << parents.last()->child(parents.last()->childCount()-1);
          indentations << position;
        }
      } else {
        while (position < indentations.last() && parents.count() > 0) {
          parents.pop_back();
          indentations.pop_back();
        }
      }

      FsTreeItem *parent = parents.last();
      parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
      for (int column = 0; column < columnData.size(); ++column) {
        parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
      }
    }

    number++;
  }
#else
  QVector<QVariant> columnData;

  for (int column = 0; column < lines.count(); ++column) {
    columnData << lines[column];
  }

  parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
  for (int column = 0; column < columnData.size(); ++column) {
    parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
  }
#endif
}

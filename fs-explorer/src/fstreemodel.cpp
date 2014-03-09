/**
 * fstreemodel.cpp - The entry of fstreemodel
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

FsTreeModel::FsTreeModel(const QStringList &headers, QObject *parent)
  : QAbstractItemModel(parent)
{
  QVector<QVariant> rootData;
  foreach (QString header, headers) {
    rootData << header;
  }

  rootItem = new FsTreeItem(rootData);
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

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    FsTreeItem *item = getItem(index);
    return item->data(index.column());
  } else if (role == Qt::DecorationRole) {
    FsTreeItem *item = getItem(index);
    if (item->parent() != rootItem) {
      return QPixmap(":/images/folder.png").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
  } else if (role == Qt::SizeHintRole) {
    return QPixmap(":/images/folder.png").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation).size();
  }

  return QVariant();
}

Qt::ItemFlags FsTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return 0;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
  if (role != Qt::EditRole && role != Qt::DisplayRole) {
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
  if ((role != Qt::EditRole && role != Qt::DisplayRole) || orientation != Qt::Horizontal) {
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

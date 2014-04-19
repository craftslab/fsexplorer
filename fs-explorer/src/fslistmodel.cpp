/**
 * fslistmodel.cpp - The entry of fslistmodel
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This file is part of FS Explorer.
 *
 * FS explorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FS Explorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FS Explorer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fslistmodel.h"

FsListModel::FsListModel(const QStringList &headers, const QStringList &data,
                         QObject *parent)
  : QAbstractItemModel(parent)
{
  QVector<QVariant> rootData;
  foreach (QString header, headers) {
    rootData << header;
  }

  rootItem = new FsListItem(rootData);
  setupModelData(data, rootItem);
}

FsListModel::FsListModel(const QStringList &headers, QObject *parent)
  : QAbstractItemModel(parent)
{
  QVector<QVariant> rootData;
  foreach (QString header, headers) {
    rootData << header;
  }

  rootItem = new FsListItem(rootData);
}

FsListModel::~FsListModel()
{
  delete rootItem;
}

int FsListModel::columnCount(const QModelIndex & /* parent */) const
{
  return rootItem->columnCount();
}

QVariant FsListModel::data(const QModelIndex &index, int role) const
{
  static int maxWidth = 0, maxHeight = 0;

  if (!index.isValid()) {
    return QVariant();
  }

  FsListItem *item = getItem(index);

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    return item->data(index.column());
  } else if (role == Qt::DecorationRole) {
    if (index.column() == 0) {
      if (item->data(columnCount() - 1) == FT_DIR) {
        return QPixmap(":/images/folder.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      } else {
        return QPixmap(":/images/file.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      }
    }
  } else if (role == Qt::SizeHintRole) {
    int width = item->data(index.column()).toString().size();
    maxWidth = width > maxWidth ? width : maxWidth;

    int height;
    if (item->data(columnCount() - 1) == FT_DIR) {
      height = QPixmap(":/images/folder.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation).height();
    } else {
      height = QPixmap(":/images/file.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation).height();
    }
    maxHeight = height > maxHeight ? height : maxHeight;

    return QSize(maxWidth, maxHeight);
  }

  return QVariant();
}

QVariant FsListModel::data(const QModelIndex &index, int column, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  FsListItem *item = getItem(index);

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    return item->data(column);
  }

  return QVariant();
}

Qt::ItemFlags FsListModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return 0;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

FsListItem *FsListModel::getItem(const QModelIndex &index) const
{
  if (index.isValid()) {
    FsListItem *item = static_cast<FsListItem*>(index.internalPointer());
    if (item) return item;
  }

  return rootItem;
}

QVariant FsListModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return rootItem->data(section);
  }

  return QVariant();
}

QModelIndex FsListModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() != 0) {
    return QModelIndex();
  }

  FsListItem *parentItem = getItem(parent);

  FsListItem *childItem = parentItem->child(row);
  if (childItem) {
    return createIndex(row, column, childItem);
  } else {
    return QModelIndex();
  }
}

bool FsListModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
  bool success;

  beginInsertColumns(parent, position, position + columns - 1);
  success = rootItem->insertColumns(position, columns);
  endInsertColumns();

  return success;
}

bool FsListModel::insertRows(int position, int rows, const QModelIndex &parent)
{
  FsListItem *parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

QModelIndex FsListModel::parent(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return QModelIndex();
  }

  FsListItem *childItem = getItem(index);
  FsListItem *parentItem = childItem->parent();

  if (parentItem == rootItem) {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool FsListModel::removeColumns(int position, int columns, const QModelIndex &parent)
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

bool FsListModel::removeRows(int position, int rows, const QModelIndex &parent)
{
  FsListItem *parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

int FsListModel::rowCount(const QModelIndex &parent) const
{
  FsListItem *parentItem = getItem(parent);

  return parentItem->childCount();
}

bool FsListModel::setData(const QModelIndex &index, const QVariant &value,
                          int role)
{
  if (role != Qt::EditRole && role != Qt::DisplayRole) {
    return false;
  }

  FsListItem *item = getItem(index);
  bool result = item->setData(index.column(), value);

  if (result) {
    emit dataChanged(index, index);
  }

  return result;
}

bool FsListModel::setHeaderData(int section, Qt::Orientation orientation,
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

void FsListModel::setupModelData(const QStringList &lines, FsListItem *parent)
{
#if 0 // DISUSED here
  QList<FsListItem*> parents;
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

      FsListItem *parent = parents.last();
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

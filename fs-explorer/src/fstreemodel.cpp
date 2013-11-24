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

#include <QtGui>

#include "fstreeitem.h"
#include "fstreemodel.h"

FsTreeModel::FsTreeModel(const QString &data, QObject *parent)
  : QAbstractItemModel(parent)
{
  QList<QVariant> rootData;
  rootData << "Title" << "Summary";
  rootItem = new FsTreeItem(rootData);
  setupModelData(data.split(QString("\n")), rootItem);
}

FsTreeModel::~FsTreeModel()
{
  delete rootItem;
}

int FsTreeModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return static_cast<FsTreeItem*>(parent.internalPointer())->columnCount();
  } else {
    return rootItem->columnCount();
  }
}

QVariant FsTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  FsTreeItem *item = static_cast<FsTreeItem*>(index.internalPointer());

  return item->data(index.column());
}

Qt::ItemFlags FsTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return 0;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant FsTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return rootItem->data(section);
  }

  return QVariant();
}

QModelIndex FsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  FsTreeItem *parentItem;

  if (!parent.isValid()) {
    parentItem = rootItem;
  } else {
    parentItem = static_cast<FsTreeItem*>(parent.internalPointer());
  }

  FsTreeItem *childItem = parentItem->child(row);
  if (childItem) {
    return createIndex(row, column, childItem);
  } else {
    return QModelIndex();
  }
}

QModelIndex FsTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return QModelIndex();
  }

  FsTreeItem *childItem = static_cast<FsTreeItem*>(index.internalPointer());
  FsTreeItem *parentItem = childItem->parent();

  if (parentItem == rootItem) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int FsTreeModel::rowCount(const QModelIndex &parent) const
{
  FsTreeItem *parentItem;
  if (parent.column() > 0) {
    return 0;
  }

  if (!parent.isValid()) {
    parentItem = rootItem;
  } else {
    parentItem = static_cast<FsTreeItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

void FsTreeModel::setupModelData(const QStringList &lines, FsTreeItem *parent)
{
  QList<FsTreeItem*> parents;
  QList<int> indentations;
  parents << parent;
  indentations << 0;

  int number = 0;

  while (number < lines.count()) {
    int position = 0;
    while (position < lines[number].length()) {
      if (lines[number].mid(position, 1) != " ")
	break;
      position++;
    }

    QString lineData = lines[number].mid(position).trimmed();

    if (!lineData.isEmpty()) {
      // Read the column data from the rest of the line.
      QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
      QList<QVariant> columnData;
      for (int column = 0; column < columnStrings.count(); ++column) {
	columnData << columnStrings[column];
      }

      if (position > indentations.last()) {
	// The last child of the current parent is now the new parent
	// unless the current parent has no children.
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

      // Append a new item to the current parent's list of children.
      parents.last()->appendChild(new FsTreeItem(columnData, parents.last()));
    }

    number++;
  }
}

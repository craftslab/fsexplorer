/**
 * fslistview.cpp - The entry of fslistview
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

#include "fslistview.h"

const int FsListView::columnWidthMax = 224;
const int FsListView::columnWidthMin = 96;

FsListView::FsListView(FsListModel *model, QWidget *parent)
  : QTreeView(parent)
{
  setModel(model);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setDragEnabled(true);
  setAcceptDrops(true);
  setDropIndicatorShown(true);

  setHeaderHidden(false);
  setColumnHidden(model->columnCount() - 1, true);

  setContextMenuPolicy(Qt::CustomContextMenu);

  setColumnWidth(LIST_NAME, columnWidthMax);
  setColumnWidth(LIST_SIZE, columnWidthMin);
  setColumnWidth(LIST_MTIME, columnWidthMax);
  setColumnWidth(LIST_ATIME, columnWidthMax);
  setColumnWidth(LIST_CTIME, columnWidthMax);

  for (int column = LIST_INO; column < model->columnCount(); ++column) {
    resizeColumnToContents(column);
  }

  QModelIndex index = model->index(0, 0);
  scrollTo(index);
  expand(index);
  setCurrentIndex(index);
}

FsListView::~FsListView()
{
  // Do nothing here
}

int FsListView::getColumnWidthMax() const
{
  return columnWidthMax;
}

int FsListView::getColumnWidthMin() const
{
  return columnWidthMin;
}

void FsListView::dragLeaveEvent(QDragLeaveEvent *event)
{
  event = event;
}

void FsListView::dragMoveEvent(QDragMoveEvent *event)
{
  event = event;
}

void FsListView::dropEvent(QDropEvent *event)
{
  event = event;
}

void FsListView::startDrag(Qt::DropActions supportedActions)
{
  supportedActions = supportedActions;
}

/**
 * fslistview.cpp - The entry of fslistview
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

#include "fslistview.h"

const int FsListView::columnWidthMax = 224;
const int FsListView::columnWidthMin = 96;

FsListView::FsListView(FsListModel *model, QWidget *parent)
  : QTreeView(parent)
{
  setModel(model);
  setSelectionMode(QAbstractItemView::SingleSelection);

#if 0 // DISUSED here
  setDragEnabled(true);
  setAcceptDrops(true);
  setDropIndicatorShown(true);
#else
  setDragDropMode(QAbstractItemView::DragDrop);
#endif

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

#if 0 // DISUSED here
  tempFile.setFileName(QString(QDir::tempPath().append(QDir::separator()).append(QString(tr("foo.tmp")))));
  tempFile.setAutoRemove(false);
#endif
}

FsListView::~FsListView()
{
#if 0 // DISUSED here
  tempFile.close();
  tempFile.remove();
#endif
}

int FsListView::getColumnWidthMax() const
{
  return columnWidthMax;
}

int FsListView::getColumnWidthMin() const
{
  return columnWidthMin;
}

void FsListView::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat(QString::fromUtf8("text/uri-list"))) {
    event->accept();
  } else {
    event->ignore();
  }
}

void FsListView::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void FsListView::dragMoveEvent(QDragMoveEvent *event)
{
  if (event->mimeData()->hasFormat(QString::fromUtf8("text/uri-list"))) {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  } else {
    event->ignore();
  }
}

void FsListView::dropEvent(QDropEvent *event)
{
  event->ignore();
}

void FsListView::startDrag(Qt::DropActions supportedActions)
{
  supportedActions = supportedActions;

#if 0 // DISUSED here
  if (!tempFile.open()) {
    tempFile.remove();
    return;
  }
  tempFile.write("foo");
  tempFile.flush();
  tempFile.close();

  QMimeData *mimeData = new QMimeData;
  mimeData->setData(QString::fromUtf8("text/uri-list"), tempFile.fileName().toUtf8());

  QByteArray fileData("foo");
  mimeData->setData("application/octet-stream", fileData);

  QDrag *drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
#endif
}

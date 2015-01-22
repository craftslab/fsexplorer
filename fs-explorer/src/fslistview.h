/**
 * fslistview.h - Header of fslistview
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

#ifndef FSLISTVIEW_H
#define FSLISTVIEW_H

#include <QModelIndex>
#include <QVariant>
#include <QTreeView>
#include <QtGui>

#include "fslistmodel.h"

enum FsListHeader {
  LIST_NAME = 0,
  LIST_SIZE,
  LIST_MTIME,
  LIST_ATIME,
  LIST_CTIME,
  LIST_INO,
  LIST_MODE,
  LIST_UID,
  LIST_GID,
  LIST_TYPE,
  LIST_MAX
};

class FsListView : public QTreeView
{
  Q_OBJECT

public:
  FsListView(FsListModel *model, QWidget *parent = 0);
  ~FsListView();

  int getColumnWidthMax() const;
  int getColumnWidthMin() const;

protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dropEvent(QDropEvent *event);
  void startDrag(Qt::DropActions supportedActions);

private:
  static const int columnWidthMax;
  static const int columnWidthMin;

#if 0 // DISUSED here
  QTemporaryFile tempFile;
#endif
};
#endif

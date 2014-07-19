/**
 * barchartview.h - Header of barchartview
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

#ifndef BARCHARTVIEW_H
#define BARCHARTVIEW_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QAbstractItemView>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QFont>
#include <QRect>
#include <QSize>
#include <QPoint>

class BarChartView : public QAbstractItemView
{
  Q_OBJECT

public:
  BarChartView(QWidget *parent = 0);
  ~BarChartView();

  QRect visualRect(const QModelIndex &index) const;
  void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
  QModelIndex indexAt(const QPoint &point) const;

protected slots:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
  void rowsInserted(const QModelIndex &parent, int start, int end);
  void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

protected:
  bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event);
  QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                         Qt::KeyboardModifiers modifiers);

  int horizontalOffset() const;
  int verticalOffset() const;

  bool isIndexHidden(const QModelIndex &index) const;

  void setSelection(const QRect&, QItemSelectionModel::SelectionFlags command);

  void mousePressEvent(QMouseEvent *event);

  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *event);
  void scrollContentsBy(int dx, int dy);

  QRegion visualRegionForSelection(const QItemSelection &selection) const;
};
#endif

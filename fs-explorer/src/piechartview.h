/**
 * piechartview.h - Header of piechartview
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

#ifndef PIECHARTVIEW_H
#define PIECHARTVIEW_H

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

class PieChartView : public QAbstractItemView
{
  Q_OBJECT

public:
  PieChartView(QWidget *parent = 0);
  ~PieChartView();

  QRect visualRect(const QModelIndex &index) const;
  void scrollTo(const QModelIndex &/* index */, ScrollHint /* hint = EnsureVisible */);
  QModelIndex indexAt(const QPoint &point) const;

protected slots:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());
  void rowsInserted(const QModelIndex &parent, int start, int end);
  void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

protected:
  bool edit(const QModelIndex &/* index */, EditTrigger /* trigger */, QEvent * /* event */);
  QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                         Qt::KeyboardModifiers /* modifiers */);
  int horizontalOffset() const;
  int verticalOffset() const;
  bool isIndexHidden(const QModelIndex &/* index */) const;
  void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *event);
  void scrollContentsBy(int dx, int dy);
  QRegion visualRegionForSelection(const QItemSelection &selection) const;

private:
  QRect itemRect(const QModelIndex &item) const;
  QRegion itemRegion(const QModelIndex &index) const;
  int rows(const QModelIndex &index = QModelIndex()) const;
  void updateGeometries();

  static const int marginY;

  int marginX;
  int totalSize;
  int pieSize;
  int validItems;
  double totalValue;
  QPoint origin;
  QRubberBand *rubberBand;
};
#endif

/**
 * piechartview.cpp - The entry of piechartview
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

#include "piechartview.h"

PieChartView::PieChartView(QWidget *parent)
  : QAbstractItemView(parent)
{
  // TODO
}

PieChartView::~PieChartView()
{
  // Do nothing here
}

QRect PieChartView::visualRect(const QModelIndex &index) const
{
  // TODO

  return QRect();
}

void PieChartView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
  // TODO
}

QModelIndex PieChartView::indexAt(const QPoint &point) const
{
  // TODO

  return QModelIndex();
}

void PieChartView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
  // TODO
}

void PieChartView::rowsInserted(const QModelIndex &parent, int start, int end)
{
  // TODO
}

void PieChartView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
  // TODO
}

bool PieChartView::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event)
{
  // TODO

  return false;
}

QModelIndex PieChartView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                     Qt::KeyboardModifiers modifiers)
{
  // TODO

  return QModelIndex();
}

int PieChartView::horizontalOffset() const
{
  // TODO

  return 0;
}

int PieChartView::verticalOffset() const
{
  // TODO

  return 0;
}

bool PieChartView::isIndexHidden(const QModelIndex &index) const
{
  // TODO

  return false;
}

void PieChartView::setSelection(const QRect&, QItemSelectionModel::SelectionFlags command)
{
}

void PieChartView::mousePressEvent(QMouseEvent *event)
{
  // TODO
}

void PieChartView::mouseMoveEvent(QMouseEvent *event)
{
}

void PieChartView::mouseReleaseEvent(QMouseEvent *event)
{
  // TODO
}

void PieChartView::paintEvent(QPaintEvent *event)
{
  // TODO
}

void PieChartView::resizeEvent(QResizeEvent *event)
{
  // TODO
}

void PieChartView::scrollContentsBy(int dx, int dy)
{
  // TODO
}

QRegion PieChartView::visualRegionForSelection(const QItemSelection &selection) const
{
  // TODO

  return QRect();
}

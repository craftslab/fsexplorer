/**
 * barchartview.cpp - The entry of barchartview
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

#include "barchartview.h"

BarChartView::BarChartView(QWidget *parent)
  : QAbstractItemView(parent)
{
  model = new QStandardItemModel(2, 2, this);
  model->setHeaderData(0, Qt::Horizontal, tr("Label"));
  model->setHeaderData(1, Qt::Horizontal, tr("Size"));
  setModel(model);

  selectionModel = new QItemSelectionModel(model);
  setSelectionModel(selectionModel);
}

BarChartView::~BarChartView()
{
  // Do nothing here
}

QRect BarChartView::visualRect(const QModelIndex &/* index */) const
{
  // TODO

  return QRect();
}

void BarChartView::scrollTo(const QModelIndex &/* index */, ScrollHint hint)
{
  hint = hint;

  // TODO
}

QModelIndex BarChartView::indexAt(const QPoint &/* point */) const
{
  // TODO

  return QModelIndex();
}

void BarChartView::dataChanged(const QModelIndex &/* topLeft */, const QModelIndex &/* bottomRight */)
{
  // TODO
}

void BarChartView::rowsInserted(const QModelIndex &/* parent */, int start, int end)
{
  start = start;
  end = end;

  // TODO
}

void BarChartView::rowsAboutToBeRemoved(const QModelIndex &/* parent */, int start, int end)
{
  start = start;
  end = end;

  // TODO
}

bool BarChartView::edit(const QModelIndex &/* index */, EditTrigger trigger, QEvent *event)
{
  trigger = trigger;
  event = event;

  // TODO

  return false;
}

QModelIndex BarChartView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                     Qt::KeyboardModifiers modifiers)
{
  cursorAction = cursorAction;
  modifiers = modifiers;

  // TODO

  return QModelIndex();
}

int BarChartView::horizontalOffset() const
{
  // TODO

  return 0;
}

int BarChartView::verticalOffset() const
{
  // TODO

  return 0;
}

bool BarChartView::isIndexHidden(const QModelIndex &/* index */) const
{
  // TODO

  return false;
}

void BarChartView::setSelection(const QRect &/* rect */, QItemSelectionModel::SelectionFlags command)
{
  command = command;

  // TODO
}

void BarChartView::mousePressEvent(QMouseEvent *event)
{
  event = event;

  // TODO
}

void BarChartView::mouseMoveEvent(QMouseEvent *event)
{
  event = event;

  // TODO
}

void BarChartView::mouseReleaseEvent(QMouseEvent *event)
{
  event = event;

  // TODO
}

void BarChartView::paintEvent(QPaintEvent *event)
{
  event = event;

  // TODO
}

void BarChartView::resizeEvent(QResizeEvent *event)
{
  event = event;

  // TODO
}

void BarChartView::scrollContentsBy(int dx, int dy)
{
  dx = dx;
  dy = dy;

  // TODO
}

QRegion BarChartView::visualRegionForSelection(const QItemSelection &/* selection */) const
{
  // TODO

  return QRect();
}

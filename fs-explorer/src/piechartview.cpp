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

#ifndef M_PI
#define M_PI 3.1415927
#endif

const int PieChartView::marginY = 8;

PieChartView::PieChartView(QWidget *parent)
  : QAbstractItemView(parent)
{
  horizontalScrollBar()->setRange(0, 0);
  verticalScrollBar()->setRange(0, 0);

  marginX = 0;
  totalSize = 0;
  pieSize = 0;
  validItems = 0;
  totalValue = 0.0;
  rubberBand = NULL;
}

PieChartView::~PieChartView()
{
  // Do nothing here
}

QRect PieChartView::visualRect(const QModelIndex &index) const
{
  QRect rect = itemRect(index);

  if (rect.isValid()) {
    return QRect(rect.left() - horizontalScrollBar()->value(),
                 rect.top() - verticalScrollBar()->value(),
                 rect.width(), rect.height());
  } else {
    return rect;
  }
}

void PieChartView::scrollTo(const QModelIndex &/* index */, ScrollHint /* hint */)
{
#if 0 // DISUSED here
  QRect area = viewport()->rect();
  QRect rect = visualRect(index);

  if (rect.left() < area.left()) {
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + rect.left() - area.left());
  } else if (rect.right() > area.right()) {
    horizontalScrollBar()->setValue(horizontalScrollBar()->value()
                                    + qMin(rect.right() - area.right(), rect.left() - area.left()));
  }

  if (rect.top() < area.top()) {
    verticalScrollBar()->setValue(verticalScrollBar()->value() + rect.top() - area.top());
  } else if (rect.bottom() > area.bottom()) {
    verticalScrollBar()->setValue(verticalScrollBar()->value()
                                  + qMin(rect.bottom() - area.bottom(), rect.top() - area.top()));
  }

  update();
#endif
}

QModelIndex PieChartView::indexAt(const QPoint &point) const
{
  if (validItems == 0) {
    return QModelIndex();
  }

  int wx = point.x() + horizontalScrollBar()->value();
  int wy = point.y() + verticalScrollBar()->value();

  if (wx < totalSize) {
    double cx = wx - (totalSize / 2);
    double cy = totalSize / 2 - wy;

    double d = pow(pow(cx, 2) + pow(cy, 2), 0.5);
    if (d == 0 || d > (pieSize / 2)) {
      return QModelIndex();
    }

    double angle = (180 / M_PI) * acos(cx / d);
    if (cy < 0) {
      angle = 360 - angle;
    }

    double startAngle = 0.0;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
      QModelIndex index = model()->index(row, 1, rootIndex());
      double value = model()->data(index).toDouble();

      if (value > 0.0) {
        double sliceAngle = (360 * value) / totalValue;

        if (angle >= startAngle && angle < (startAngle + sliceAngle)) {
          return model()->index(row, 1, rootIndex());
        }

        startAngle += sliceAngle;
      }
    }
  } else {
    double itemHeight = QFontMetrics(viewOptions().font).height();
    int listItem = int((wy - marginY) / itemHeight);
    int validRow = 0;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
      QModelIndex index = model()->index(row, 1, rootIndex());
      if (model()->data(index).toDouble() > 0.0) {
        if (listItem == validRow) {
          return model()->index(row, 0, rootIndex());
        }

        validRow++;
      }
    }
  }

  return QModelIndex();
}

void PieChartView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
  QAbstractItemView::dataChanged(topLeft, bottomRight, roles);

  validItems = 0;
  totalValue = 0.0;

  for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    double value = model()->data(index).toDouble();

    if (value > 0.0) {
      totalValue += value;
      validItems++;
    }
  }

  viewport()->update();
}

void PieChartView::rowsInserted(const QModelIndex &parent, int start, int end)
{
  for (int row = start; row <= end; ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    double value = model()->data(index).toDouble();

    if (value > 0.0) {
      totalValue += value;
      validItems++;
    }
  }

  QAbstractItemView::rowsInserted(parent, start, end);
}

void PieChartView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
  for (int row = start; row <= end; ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    double value = model()->data(index).toDouble();
    if (value > 0.0) {
      totalValue -= value;
      validItems--;
    }
  }

  QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}

bool PieChartView::edit(const QModelIndex &/* index */, EditTrigger /* trigger */, QEvent */* event */)
{
  // Do nothing here

  return false;
}

QModelIndex PieChartView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                     Qt::KeyboardModifiers /* modifiers */)
{
  QModelIndex current = currentIndex();

  switch (cursorAction) {
  case MoveLeft:
  case MoveUp:
    if (current.row() > 0) {
      current = model()->index(current.row() - 1, current.column(),
                               rootIndex());
    } else {
      current = model()->index(0, current.column(), rootIndex());
    }
    break;
  case MoveRight:
  case MoveDown:
    if (current.row() < rows(current) - 1) {
      current = model()->index(current.row() + 1, current.column(),
                               rootIndex());
    } else {
      current = model()->index(rows(current) - 1, current.column(),
                               rootIndex());
    }
    break;
  default:
    break;
  }

  viewport()->update();

  return current;
}

int PieChartView::horizontalOffset() const
{
  return horizontalScrollBar()->value();
}

int PieChartView::verticalOffset() const
{
  return verticalScrollBar()->value();
}

bool PieChartView::isIndexHidden(const QModelIndex &/* index */) const
{
  return false;
}

void PieChartView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
  QRect contentsRect = rect.translated(horizontalScrollBar()->value(),
                                       verticalScrollBar()->value()).normalized();
  int rows = model()->rowCount(rootIndex());
  int columns = model()->columnCount(rootIndex());
  QModelIndexList indexes;

  for (int row = 0; row < rows; ++row) {
    for (int column = 0; column < columns; ++column) {
      QModelIndex index = model()->index(row, column, rootIndex());
      QRegion region = itemRegion(index);
      if (!region.intersected(contentsRect).isEmpty()) {
        indexes.append(index);
      }
    }
  }

  if (indexes.size() > 0) {
    int firstRow = indexes[0].row();
    int lastRow = indexes[0].row();
    int firstColumn = indexes[0].column();
    int lastColumn = indexes[0].column();

    for (int i = 1; i < indexes.size(); ++i) {
      firstRow = qMin(firstRow, indexes[i].row());
      lastRow = qMax(lastRow, indexes[i].row());
      firstColumn = qMin(firstColumn, indexes[i].column());
      lastColumn = qMax(lastColumn, indexes[i].column());
    }

    QItemSelection selection(model()->index(firstRow, firstColumn, rootIndex()),
                             model()->index(lastRow, lastColumn, rootIndex()));
    selectionModel()->select(selection, command);
  } else {
    QModelIndex noIndex;
    QItemSelection selection(noIndex, noIndex);
    selectionModel()->select(selection, command);
  }

  update();
}

void PieChartView::mousePressEvent(QMouseEvent *event)
{
  QAbstractItemView::mousePressEvent(event);
  origin = event->pos();

  if (!rubberBand) {
    rubberBand = new QRubberBand(QRubberBand::Rectangle, viewport());
  }

  rubberBand->setGeometry(QRect(origin, QSize()));
  rubberBand->show();
}

void PieChartView::mouseMoveEvent(QMouseEvent *event)
{
  if (rubberBand) {
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
  }

  QAbstractItemView::mouseMoveEvent(event);
}

void PieChartView::mouseReleaseEvent(QMouseEvent *event)
{
  QAbstractItemView::mouseReleaseEvent(event);

  if (rubberBand) {
    rubberBand->hide();
  }

  viewport()->update();
}

void PieChartView::paintEvent(QPaintEvent *event)
{
  QItemSelectionModel *selections = selectionModel();
  QStyleOptionViewItem option = viewOptions();

  QPainter painter(viewport());
  painter.setRenderHint(QPainter::Antialiasing);

  QBrush background = option.palette.base();
  painter.fillRect(event->rect(), background);

  QPen foreground(option.palette.color(QPalette::WindowText));
  painter.setPen(foreground);

  QRect pieRect = QRect(marginX, marginY, pieSize, pieSize);

  if (validItems > 0) {
    painter.save();
    painter.translate(pieRect.x() - horizontalScrollBar()->value(),
                      pieRect.y() - verticalScrollBar()->value());
    painter.drawEllipse(0, 0, pieSize, pieSize);

    double startAngle = 0.0;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
      QModelIndex index = model()->index(row, 1, rootIndex());
      double value = model()->data(index).toDouble();

      if (value > 0.0) {
        double angle = (360 * value) / totalValue;

        QModelIndex colorIndex = model()->index(row, 0, rootIndex());
        QColor color = QColor(model()->data(colorIndex, Qt::DecorationRole).toString());

        if (currentIndex() == index) {
          painter.setBrush(QBrush(color, Qt::Dense4Pattern));
        } else if (selections->isSelected(index)) {
          painter.setBrush(QBrush(color, Qt::Dense3Pattern));
        } else {
          painter.setBrush(QBrush(color));
        }

        painter.drawPie(0, 0, pieSize, pieSize, static_cast<int> (startAngle * 16), static_cast<int> (angle * 16));

        startAngle += angle;
      }
    }
    painter.restore();

    int keyNumber = 0;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
      QModelIndex index = model()->index(row, 1, rootIndex());
      double value = model()->data(index).toDouble();

      if (value > 0.0) {
        QModelIndex labelIndex = model()->index(row, 0, rootIndex());

        QStyleOptionViewItem option = viewOptions();
        option.rect = visualRect(labelIndex);

#if 0 // DISUSED here
        if (selections->isSelected(labelIndex)) {
          option.state |= QStyle::State_Selected;
        }

        if (currentIndex() == labelIndex) {
          option.state |= QStyle::State_HasFocus;
        }
#endif

        itemDelegate()->paint(&painter, option, labelIndex);

        keyNumber++;
      }
    }
  }
}

void PieChartView::resizeEvent(QResizeEvent *event)
{
  QSize size = event->size();
  if (!size.isValid()) {
    return;
  }

  pieSize = size.height() - (marginY * 2);
  if (pieSize < 0) {
    pieSize = size.height();
  }

  marginX = (size.width() - pieSize) / 2;
  if (marginX < 0) {
    marginX = 0;
  }

  totalSize = marginX + pieSize + (2 * marginY);

  updateGeometries();
}

void PieChartView::scrollContentsBy(int dx, int dy)
{
  viewport()->scroll(dx, dy);
}

QRegion PieChartView::visualRegionForSelection(const QItemSelection &selection) const
{
  int ranges = selection.count();

  if (ranges == 0) {
    return QRect();
  }

  QRegion region;

  for (int i = 0; i < ranges; ++i) {
    QItemSelectionRange range = selection.at(i);
    for (int row = range.top(); row <= range.bottom(); ++row) {
      for (int col = range.left(); col <= range.right(); ++col) {
        QModelIndex index = model()->index(row, col, rootIndex());
        region += visualRect(index);
      }
    }
  }

  return region;
}

QRect PieChartView::itemRect(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return QRect();
  }

  QModelIndex valueIndex;

  if (index.column() != 1) {
    valueIndex = model()->index(index.row(), 1, rootIndex());
  } else {
    valueIndex = index;
  }

  if (model()->data(valueIndex).toDouble() > 0.0) {
    int listItem = 0;
    for (int row = index.row() - 1; row >= 0; --row) {
      if (model()->data(model()->index(row, 1, rootIndex())).toDouble() > 0.0) {
        listItem++;
      }
    }

    double itemHeight;

    switch (index.column()) {
    case 0:
      itemHeight = QFontMetrics(viewOptions().font).height();

      return QRect(totalSize,
                   static_cast<int> (marginY + listItem*itemHeight),
                   totalSize - marginY,
                   static_cast<int> (itemHeight));
    case 1:
      return viewport()->rect();
    }
  }

  return QRect();
}

QRegion PieChartView::itemRegion(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return QRegion();
  }

  if (index.column() != 1) {
    return itemRect(index);
  }

  if (model()->data(index).toDouble() <= 0.0) {
    return QRegion();
  }

  double startAngle = 0.0;
  for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
    QModelIndex sliceIndex = model()->index(row, 1, rootIndex());
    double value = model()->data(sliceIndex).toDouble();

    if (value > 0.0) {
      double angle = (360 * value) / totalValue;

      if (sliceIndex == index) {
        QPainterPath slicePath;
        slicePath.moveTo(totalSize / 2, totalSize / 2);
        slicePath.arcTo(marginY, marginY, marginY + pieSize, marginY + pieSize,
                        startAngle, angle);
        slicePath.closeSubpath();

        return QRegion(slicePath.toFillPolygon().toPolygon());
      }

      startAngle += angle;
    }
  }

  return QRegion();
}

int PieChartView::rows(const QModelIndex &index) const
{
  return model()->rowCount(model()->parent(index));
}

void PieChartView::updateGeometries()
{
#if 0 // DISUSED here
  horizontalScrollBar()->setPageStep(viewport()->width());
  horizontalScrollBar()->setRange(0, qMax(0, (2 * totalSize) - viewport()->width()));

  verticalScrollBar()->setPageStep(viewport()->height());
  verticalScrollBar()->setRange(0, qMax(0, totalSize - viewport()->height()));
#endif
}

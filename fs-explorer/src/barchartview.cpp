/**
 * barchartview.cpp - The entry of barchartview
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

#include "barchartview.h"

const int BarChartView::marginX = 16;
const int BarChartView::marginY = 12;

BarChartView::BarChartView(QWidget *parent)
  : QAbstractItemView(parent)
{
  horizontalScrollBar()->setRange(0, 0);
  verticalScrollBar()->setRange(0, 0);

  labelWidth = 0;
  barWidth = 0;
  sizeWidth = 0;
  validItems = 0;
  rubberBand = NULL;
}

BarChartView::~BarChartView()
{
  // Do nothing here
}

QRect BarChartView::visualRect(const QModelIndex &index) const
{
  QRect rect = itemRect(index, marginX, marginY);

  if (rect.isValid()) {
    return QRect(rect.left() - horizontalScrollBar()->value(),
                 rect.top() - verticalScrollBar()->value(),
                 rect.width(), rect.height());
  } else {
    return rect;
  }
}

QRect BarChartView::visualRect(const QModelIndex &index, int offsetX, int offsetY) const
{
  QRect rect = itemRect(index, offsetX, offsetY);

  if (rect.isValid()) {
    return QRect(rect.left() - horizontalScrollBar()->value(),
                 rect.top() - verticalScrollBar()->value(),
                 rect.width(), rect.height());
  } else {
    return rect;
  }
}

void BarChartView::scrollTo(const QModelIndex &/* index */, ScrollHint /* hint */)
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

QModelIndex BarChartView::indexAt(const QPoint &point) const
{
  if (validItems == 0) {
    return QModelIndex();
  }

  int wy = point.y() + verticalScrollBar()->value();

  int itemHeight = QFontMetrics(viewOptions().font).height();
  int listItem = static_cast<int> ((wy - marginY) / itemHeight);
  int validRow = 0;

  for (int row = 0; row < model()->rowCount(rootIndex()) - 1; ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    if (model()->data(index).toLongLong() >= 0) {
      if (listItem == validRow) {
        return model()->index(row, 0, rootIndex());
      }

      validRow++;
    }
  }

  return QModelIndex();
}

void BarChartView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
  QAbstractItemView::dataChanged(topLeft, bottomRight, roles);

  validItems = 0;

  for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
    QModelIndex index = model()->index(row, 0, rootIndex());
    QString str = model()->data(index).toString();

    int width = QFontMetrics(viewOptions().font).width(str.append(QString(tr("   "))));
    labelWidth = (width > labelWidth) ? width : labelWidth;

    index = model()->index(row, 3, rootIndex());
    str = model()->data(index).toString();

    width = QFontMetrics(viewOptions().font).width(str.append(QString(tr("   "))));
    sizeWidth = (width > sizeWidth) ? width : sizeWidth;

    index = model()->index(row, 1, rootIndex());
    qint64 value = model()->data(index).toLongLong();

    if (value >= 0) {
      validItems++;
    }
  }

  viewport()->update();
}

void BarChartView::rowsInserted(const QModelIndex &parent, int start, int end)
{
  for (int row = start; row <= end; ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    qint64 value = model()->data(index).toLongLong();

    if (value >= 0) {
      validItems++;
    }
  }

  QAbstractItemView::rowsInserted(parent, start, end);
}

void BarChartView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
  for (int row = start; row <= end; ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    qint64 value = model()->data(index).toLongLong();

    if (value >= 0) {
      validItems--;
    }
  }

  QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}

bool BarChartView::edit(const QModelIndex &/* index */, EditTrigger /* trigger */, QEvent * /* event */)
{
  // Do nothing here

  return false;
}

QModelIndex BarChartView::moveCursor(QAbstractItemView::CursorAction cursorAction,
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

int BarChartView::horizontalOffset() const
{
  return horizontalScrollBar()->value();
}

int BarChartView::verticalOffset() const
{
  return verticalScrollBar()->value();
}

bool BarChartView::isIndexHidden(const QModelIndex &/* index */) const
{
  return false;
}

void BarChartView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
  QRect contentsRect = rect.translated(horizontalScrollBar()->value(),
                                       verticalScrollBar()->value()).normalized();
  int rows = model()->rowCount(rootIndex());
  int columns = model()->columnCount(rootIndex());
  QModelIndexList indexes;

  for (int row = 0; row < rows; ++row) {
    for (int column = 0; column < columns; ++column) {
      QModelIndex index = model()->index(row, column, rootIndex());
      QRegion region = itemRegion(index, marginX, marginY);
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

void BarChartView::mousePressEvent(QMouseEvent *event)
{
  QAbstractItemView::mousePressEvent(event);
  origin = event->pos();

  if (!rubberBand) {
    rubberBand = new QRubberBand(QRubberBand::Rectangle, viewport());
  }

  rubberBand->setGeometry(QRect(origin, QSize()));
  rubberBand->show();
}

void BarChartView::mouseMoveEvent(QMouseEvent *event)
{
  if (rubberBand) {
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
  }

  QAbstractItemView::mouseMoveEvent(event);
}

void BarChartView::mouseReleaseEvent(QMouseEvent *event)
{
  QAbstractItemView::mouseReleaseEvent(event);

  if (rubberBand) {
    rubberBand->hide();
  }

  viewport()->update();
}

void BarChartView::paintEvent(QPaintEvent *event)
{
  QItemSelectionModel *selections = selectionModel();
  QStyleOptionViewItem option = viewOptions();

  QPainter painter(viewport());
  painter.setRenderHint(QPainter::Antialiasing);

  QBrush background = option.palette.base();
  painter.fillRect(event->rect(), background);

  QPen foreground(option.palette.color(QPalette::WindowText));
  painter.setPen(foreground);

  if (validItems <= 0) {
    return;
  }

  for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    qint64 value = model()->data(index).toLongLong();

    if (value >= 0) {
      QModelIndex labelIndex = model()->index(row, 0, rootIndex());

      QStyleOptionViewItem option = viewOptions();
      option.rect = visualRect(labelIndex, marginX, marginY);

#if 0 // DISUSED here
      if (selections->isSelected(labelIndex)) {
        option.state |= QStyle::State_Selected;
      }

      if (currentIndex() == labelIndex) {
        option.state |= QStyle::State_HasFocus;
      }
#endif

      itemDelegate()->paint(&painter, option, labelIndex);
    }
  }

  for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    qint64 value = model()->data(index).toLongLong();

    if (value >= 0) {
      QModelIndex labelIndex = model()->index(row, 3, rootIndex());

      QStyleOptionViewItem option = viewOptions();
      option.rect = visualRect(labelIndex, marginX + labelWidth + marginX + barWidth, marginY);

#if 0 // DISUSED here
      if (selections->isSelected(labelIndex)) {
        option.state |= QStyle::State_Selected;
      }

      if (currentIndex() == labelIndex) {
        option.state |= QStyle::State_HasFocus;
      }
#endif

      itemDelegate()->paint(&painter, option, labelIndex);
    }
  }

  painter.save();
  painter.translate(marginX + labelWidth + (marginX / 2) - horizontalScrollBar()->value(),
                    marginY - verticalScrollBar()->value());

  int itemHeight = QFontMetrics(viewOptions().font).height();

  QModelIndex maxWidthIndex = model()->index(0, 1, rootIndex());
  qint64 maxWidth = model()->data(maxWidthIndex).toLongLong();

  for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
    QModelIndex index = model()->index(row, 1, rootIndex());
    qint64 value = model()->data(index).toLongLong();

    if (value >= 0) {
      QModelIndex colorIndex = model()->index(row, 2, rootIndex());
      QColor color = QColor(model()->data(colorIndex, Qt::DisplayRole).toString());

      if (currentIndex() == index) {
        painter.setBrush(QBrush(color, Qt::Dense4Pattern));
      } else if (selections->isSelected(index)) {
        painter.setBrush(QBrush(color, Qt::Dense3Pattern));
      } else {
        painter.setBrush(QBrush(color));
      }

      int width = maxWidth != 0 ? static_cast<int> ((value * barWidth) / maxWidth) : 0;
      painter.drawRect(0, itemHeight * row, width, itemHeight);
    }
  }
  painter.restore();
}

void BarChartView::resizeEvent(QResizeEvent *event)
{
  QSize size = event->size();
  if (!size.isValid()) {
    return;
  }

#if 0 // DISUSED here
  barWidth = size.width() - marginX - (labelWidth + marginX) - sizeWidth - marginX;
#else
  barWidth = (size.width() * 2) / 5;
#endif

  updateGeometries();
}

void BarChartView::scrollContentsBy(int dx, int dy)
{
  viewport()->scroll(dx, dy);
}

QRegion BarChartView::visualRegionForSelection(const QItemSelection &selection) const
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

QRect BarChartView::itemRect(const QModelIndex &index, int offsetX, int offsetY) const
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

  if (model()->data(valueIndex).toLongLong() >= 0) {
    int listItem = 0;
    for (int row = index.row() - 1; row >= 0; --row) {
      if (model()->data(model()->index(row, 1, rootIndex())).toLongLong() >= 0) {
        listItem++;
      }
    }

    int itemHeight;

    switch (index.column()) {
    case 0:
      itemHeight = QFontMetrics(viewOptions().font).height();
      return QRect(offsetX,
                   static_cast<int> (offsetY + listItem * itemHeight),
                   labelWidth,
                   static_cast<int> (itemHeight));
    case 1:
      return viewport()->rect();
    case 2:
      // Do nothing here
      break;
    case 3:
      itemHeight = QFontMetrics(viewOptions().font).height();
      return QRect(offsetX,
                   static_cast<int> (offsetY + listItem * itemHeight),
                   sizeWidth,
                   static_cast<int> (itemHeight));
    default:
      break;
    }
  }

  return QRect();
}

QRegion BarChartView::itemRegion(const QModelIndex &index, int offsetX, int offsetY) const
{
  if (!index.isValid()) {
    return QRegion();
  }

  if (index.column() != 1) {
    return itemRect(index, offsetX, offsetY);
  }

  if (model()->data(index).toLongLong() < 0) {
    return QRegion();
  }

  return QRegion();
}

int BarChartView::rows(const QModelIndex &index) const
{
  return model()->rowCount(model()->parent(index));
}

void BarChartView::updateGeometries()
{
#if 0 // DISUSED here
  horizontalScrollBar()->setPageStep(viewport()->width());
  horizontalScrollBar()->setRange(0, qMax(0, (2 * totalSize) - viewport()->width()));

  verticalScrollBar()->setPageStep(viewport()->height());
  verticalScrollBar()->setRange(0, qMax(0, totalSize - viewport()->height()));
#endif
}

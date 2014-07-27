/**
 * chartwindow.cpp - The entry of chartwindow
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

#include "chartwindow.h"

const int ChartWindow::width = 640;
const int ChartWindow::height = 480;

ChartWindow::ChartWindow(const QString &title, FsEngine *engine, QWidget *parent)
  : QWidget(parent)
{
  chartEngine = new ChartEngine(engine, this);

  setupPieChartView();
  setupBarChartView();

  frameHLine = new QFrame(this);
  frameHLine->setFrameShape(QFrame::HLine);
  frameHLine->setFrameShadow(QFrame::Sunken);
  frameHLine->setLineWidth(1);
  frameHLine->setMidLineWidth(0);

  closeButton = new QPushButton(tr("Close"), this);
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

  hLayout = new QHBoxLayout(this);
  hLayout->insertSpacing(0, 300);
  hLayout->insertStretch(0, 1);
  hLayout->addWidget(closeButton);

  hLayoutWidget = new QWidget(this);
  hLayoutWidget->setLayout(hLayout);

  vLayout = new QVBoxLayout(this);
  vLayout->addWidget(pieChartView);
  vLayout->addWidget(barChartView);
  vLayout->addWidget(frameHLine);
  vLayout->addWidget(hLayoutWidget);
  setLayout(vLayout);

  setWindowTitle(title);
  setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_DeleteOnClose, true);

  QDesktopWidget *desktopWidget = QApplication::desktop();
  QRect screenRect = desktopWidget->screenGeometry();
  if ((screenRect.width() - width) >= 0 && ((screenRect.height() - height) >= 0)) {
    move((screenRect.width() - width) / 2, (screenRect.height() - height) / 2);
  } else {
    move(0, 0);
  }
  resize(width, height);

  showPieChartView();
  showBarChartView();
}

ChartWindow::~ChartWindow()
{
  if (chartEngine) {
    delete chartEngine;
    chartEngine = NULL;
  }
}

void ChartWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void ChartWindow::setupPieChartView()
{
  pieChartModel = new QStandardItemModel(2, 2, this);
  pieChartModel->setHeaderData(0, Qt::Horizontal, tr("Label"));
  pieChartModel->setHeaderData(1, Qt::Horizontal, tr("Capacity"));

  pieChartView = new PieChartView(this);
  pieChartView->setModel(pieChartModel);

  pieChartSelectModel = new QItemSelectionModel(pieChartModel);
  pieChartView->setSelectionModel(pieChartSelectModel);
}

void ChartWindow::setupBarChartView()
{
  barChartModel = new QStandardItemModel(10, 3, this);
  barChartModel->setHeaderData(0, Qt::Horizontal, tr("Label"));
  barChartModel->setHeaderData(1, Qt::Horizontal, tr("Bar"));
  barChartModel->setHeaderData(2, Qt::Horizontal, tr("Size"));

  barChartView = new BarChartView(this);
  barChartView->setModel(barChartModel);

  barChartSelectModel = new QItemSelectionModel(barChartModel);
  barChartView->setSelectionModel(barChartSelectModel);
}

void ChartWindow::showPieChartView()
{
  int row;
  int size;
  QStringList pieces;

  pieChartModel->removeRows(0, pieChartModel->rowCount(QModelIndex()), QModelIndex());

  row = 0;
  size = 30;
  pieces.clear();
  pieces << QString::number(size, 10).append(QString(tr("MB used"))) << QString::number(size, 10) << QString(tr("#fce94f"));

  pieChartModel->insertRows(row, 1, QModelIndex());
  pieChartModel->setData(pieChartModel->index(row, 0, QModelIndex()), pieces.value(0));
  pieChartModel->setData(pieChartModel->index(row, 1, QModelIndex()), pieces.value(1));
  pieChartModel->setData(pieChartModel->index(row, 0, QModelIndex()), QColor(pieces.value(2)), Qt::DecorationRole);

  row = 1;
  size = 70;
  pieces.clear();
  pieces << QString::number(size, 10).append(QString(tr("MB free"))) << QString::number(size, 10) << QString(tr("#729fcf"));

  pieChartModel->insertRows(row, 1, QModelIndex());
  pieChartModel->setData(pieChartModel->index(row, 0, QModelIndex()), pieces.value(0));
  pieChartModel->setData(pieChartModel->index(row, 1, QModelIndex()), pieces.value(1));
  pieChartModel->setData(pieChartModel->index(row, 0, QModelIndex()), QColor(pieces.value(2)), Qt::DecorationRole);
}

void ChartWindow::showBarChartView()
{
  // TODO
}

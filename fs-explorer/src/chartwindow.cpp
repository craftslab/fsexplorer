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

const int ChartWindow::pieChartRowCount = 3;
const int ChartWindow::barChartRowCount = 10;

const QString ChartWindow::colorYellow = QObject::tr("#fce94f");
const QString ChartWindow::colorBlue = QObject::tr("#729fcf");

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
  pieChartModel = new QStandardItemModel(pieChartRowCount, 2, this);
  pieChartModel->setHeaderData(0, Qt::Horizontal, tr("Label"));
  pieChartModel->setHeaderData(1, Qt::Horizontal, tr("Capacity"));

  pieChartView = new PieChartView(this);
  pieChartView->setModel(pieChartModel);

  pieChartSelectModel = new QItemSelectionModel(pieChartModel);
  pieChartView->setSelectionModel(pieChartSelectModel);
}

void ChartWindow::setupBarChartView()
{
  barChartModel = new QStandardItemModel(barChartRowCount, 3, this);
  barChartModel->setHeaderData(0, Qt::Horizontal, tr("Label"));
  barChartModel->setHeaderData(1, Qt::Horizontal, tr("Bar"));
  barChartModel->setHeaderData(2, Qt::Horizontal, tr("Color"));

  barChartView = new BarChartView(this);
  barChartView->setModel(barChartModel);

  barChartSelectModel = new QItemSelectionModel(barChartModel);
  barChartView->setSelectionModel(barChartSelectModel);
}

void ChartWindow::showPieChartView()
{
  int size;
  QList<int> sizeList;
  QStringList pieces;
  QList<QStringList> piecesList;
  int i;

  sizeList.clear();
  getPieChartInfo(sizeList);

  piecesList.clear();

  size = sizeList.at(0);
  pieces.clear();
  pieces << QString::number(size, 10).append(QString(tr("MB used"))) << QString::number(size, 10) << colorYellow;
  piecesList.append(pieces);

  size = sizeList.at(1);
  pieces.clear();
  pieces << QString::number(size, 10).append(QString(tr("MB free"))) << QString::number(size, 10) << colorBlue;
  piecesList.append(pieces);

  size = sizeList.at(2);
  pieces.clear();
  pieces << QString(tr("Total capacity: ")).append(QString::number(size, 10)).append(QString(tr("MB"))) << QString::number(size, 10);
  piecesList.append(pieces);

  pieChartModel->removeRows(0, pieChartModel->rowCount(QModelIndex()), QModelIndex());

  for (i = 0; i < pieChartRowCount - 1; ++i) {
    pieChartModel->insertRows(i, 1, QModelIndex());
    pieChartModel->setData(pieChartModel->index(i, 0, QModelIndex()), piecesList.at(i).value(0), Qt::DisplayRole);
    pieChartModel->setData(pieChartModel->index(i, 1, QModelIndex()), piecesList.at(i).value(1), Qt::DisplayRole);
    pieChartModel->setData(pieChartModel->index(i, 0, QModelIndex()), QColor(piecesList.at(i).value(2)), Qt::DecorationRole);
  }

  i = pieChartRowCount - 1;
  if (i > 0) {
    pieChartModel->insertRows(i, 1, QModelIndex());
    pieChartModel->setData(pieChartModel->index(i, 0, QModelIndex()), piecesList.at(i).value(0), Qt::DisplayRole);
    pieChartModel->setData(pieChartModel->index(i, 1, QModelIndex()), piecesList.at(i).value(1), Qt::DisplayRole);
  }
}

void ChartWindow::showBarChartView()
{
  QStringList nameList;
  QList<int> sizeList;
  QStringList bar;
  QList<QStringList> barList;

  nameList.clear();
  sizeList.clear();
  getBarChartInfo(nameList, sizeList, barChartRowCount);

  barList.clear();

  for (int i = 0; i < barChartRowCount; ++i) {
    QString name = nameList.at(i);
    int size = sizeList.at(i);

    bar.clear();
    bar << name.append(QString(tr(" "))).append(QString::number(size, 10)).append(QString(tr("MB"))) << QString::number(size, 10) << colorYellow;

    barList.append(bar);
  }

  barChartModel->removeRows(0, barChartModel->rowCount(QModelIndex()), QModelIndex());

  for (int i = 0; i < barChartRowCount; ++i) {
    barChartModel->insertRows(i, 1, QModelIndex());
    barChartModel->setData(barChartModel->index(i, 0, QModelIndex()), barList.at(i).value(0), Qt::DisplayRole);
    barChartModel->setData(barChartModel->index(i, 1, QModelIndex()), barList.at(i).value(1), Qt::DisplayRole);
    barChartModel->setData(barChartModel->index(i, 2, QModelIndex()), barList.at(i).value(2), Qt::DisplayRole);
  }
}

void ChartWindow::getPieChartInfo(QList<int> &sizeList)
{
  QList<int> list;
  list.clear();

  chartEngine->capacityList(list);

  int total = 0;

  for (int i = 0; i < list.size(); ++i) {
    sizeList << list.at(i);
    total += list.at(i);
  }
  sizeList << total;
}

void ChartWindow::getBarChartInfo(QStringList &nameList, QList<int> &sizeList, int listLen)
{
  (void)chartEngine->sizeRankingList(nameList, sizeList, listLen);
}

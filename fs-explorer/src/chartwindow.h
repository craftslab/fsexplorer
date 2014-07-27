/**
 * chartwindow.h - Header of chartwindow
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

#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

#include "fsengine.h"
#include "chartengine.h"
#include "piechartview.h"
#include "barchartview.h"

class ChartWindow : public QWidget
{
  Q_OBJECT

public:
  ChartWindow(const QString &title, FsEngine *engine, QWidget *parent = 0);
  ~ChartWindow();

protected:
  void closeEvent(QCloseEvent *event);

private:
  void setupPieChartView();
  void setupBarChartView();
  void showPieChartView();
  void showBarChartView();

  static const int width;
  static const int height;

  ChartEngine *chartEngine;

  QAbstractItemModel *pieChartModel;
  QAbstractItemView *pieChartView;
  QItemSelectionModel *pieChartSelectModel;

  QAbstractItemModel *barChartModel;
  QAbstractItemView *barChartView;
  QItemSelectionModel *barChartSelectModel;

  QFrame *frameHLine;
  QPushButton *closeButton;
  QHBoxLayout *hLayout;
  QWidget *hLayoutWidget;
  QVBoxLayout *vLayout;
  QShortcut *shortcut;
};
#endif

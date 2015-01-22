/**
 * sparsewindow.h - Header of sparsewindow
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

#ifndef SPARSEWINDOW_H
#define SPARSEWINDOW_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

#include "sparseengine.h"

class SparseWindow : public QWidget
{
  Q_OBJECT

public:
  SparseWindow(const QString &text, const QString &name, QWidget *parent = 0);
  ~SparseWindow();

  QString getSparseName();

signals:
  void stop();

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void handleResultReady(const QString &name);

private:
  static const int width;
  static const int height;

  QLabel *label;
  QPushButton *cancelButton;
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;
  QWidget *hLayoutWidget;
  QShortcut *shortcut;

  SparseEngine *sparseEngine;
  QString sparseName;
};
#endif

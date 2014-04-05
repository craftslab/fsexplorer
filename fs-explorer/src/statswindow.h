/**
 * statswindow.h - Header of statswindow
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

#ifndef STATSWINDOW_H
#define STATSWINDOW_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QClipboard>
#include <QMimeData>

class StatsWindow : public QWidget
{
  Q_OBJECT

public:
  StatsWindow(const QString &stat, QWidget *parent = 0);

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void copyToClipboard();

private:
  QTextEdit *textEdit;
  QFrame *frameHLine;
  QPushButton *copyToClipboardButton;
  QPushButton *closeButton;
  QHBoxLayout *hLayout;
  QWidget *hLayoutWidget;
  QVBoxLayout *vLayout;
};
#endif

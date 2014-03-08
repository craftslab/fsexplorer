/**
 * statswindow.h - Header of statswindow
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
  StatsWindow(QWidget *parent = 0);

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

/**
 * searchwindow.h - Header of searchwindow
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

#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QClipboard>
#include <QMimeData>

#include "searchengine.h"

class SearchWindow : public QWidget
{
  Q_OBJECT

public:
  SearchWindow(const QString &title, const QString &text, FsEngine *engine, QWidget *parent = 0);
  ~SearchWindow();

signals:
  void search(const QString &name);
  void stop();
  void selected(const QString &address);

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void go();
  void copyToClipboard();
  void stopStart();

  void handleStarted();
  void handleFinished();
  void handleFound(const QStringList &address);
  void handleItemDoubleClicked(QListWidgetItem *item);

private:
  static const QString separator;
  static const QString stopstr;
  static const QString startstr;
  static const int width;
  static const int height;

  QListWidget *listWidget;
  QFrame *frameHLine;
  QPushButton *goButton;
  QPushButton *copyToClipboardButton;
  QPushButton *switchButton;
  QPushButton *closeButton;
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;
  QWidget *hLayoutWidget;
  QShortcut *shortcut;

  QString searchName;
  SearchEngine *searchEngine;
};
#endif

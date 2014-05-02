/**
 * searchwindow.h - Header of searchwindow
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
  SearchWindow(const QString &title, const QString &text, QWidget *parent = 0);

signals:
  void selected(const QString &name);

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void go();
  void copyToClipboard();
  void doubleClickItem(QModelIndex index);

private:
  static const int width;
  static const int height;

  QListView *listView;
  QStandardItemModel *listModel;
  QFrame *frameHLine;
  QPushButton *goButton;
  QPushButton *copyToClipboardButton;
  QPushButton *closeButton;
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;
  QWidget *hLayoutWidget;
};
#endif

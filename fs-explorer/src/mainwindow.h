/**
 * mainwindow.h - Header of mainwindow
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the distribution
 * in the file COPYING); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QToolBar;
class QSplitter;
class QTreeView;
class Explorer;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

signals:
  void mounted(bool);

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void openFile();
  void importDir();
  void exportDir();
  void closeAll();
  void console();
  void about();

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createConnections();

  QTreeView *treeView;
  QSplitter *splitter;
  QLabel *readyLabel;
  Explorer *explorer;

  QMenu *fileMenu;
  QMenu *toolsMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *toolsToolBar;
  QAction *openAction;
  QAction *importAction;
  QAction *exportAction;
  QAction *closeAction;
  QAction *exitAction;
  QAction *consoleAction;
  QAction *aboutAction;
  QAction *aboutQtAction;
};
#endif

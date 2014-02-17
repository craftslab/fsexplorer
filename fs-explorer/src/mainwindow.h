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
class QTreeView;
class QListView;
class QTextEdit;
class QSplitter;
class QHBoxLayout;

class FsEngine;
class FsTreeModel;
class StatsWindow;
class ConsoleWindow;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

signals:
  void mounted(bool status);

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void openFile();
  void importDir();
  void exportDir();
  void closeFile();
  void console();
  void stats();
  void about();
  void showWidgets(bool show);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createWidgets();
  void createConnections();
  void loadFile(QString &name);
  void setOutput(const QString &text);
  void createTreeRoot(struct fs_dirent *root);
  void createTreeItems(unsigned long long ino);
  void insertTreeRow(const QStringList &data);
  void insertTreeChild(const QStringList &data, const QModelIndex &parent);
  void removeTreeRowsAll();
  void updateTreeItem(int row, const QStringList &data);

  QWidget *layoutWidget;
  QHBoxLayout *hBoxLayout;
  FsTreeModel *treeModel;
  QTreeView *treeView;
  QListView *listView;
  QTextEdit *outputView;
  QSplitter *vertSplitter;
  QSplitter *horiSplitter;
  QLabel *bgLabel;
  QLabel *readyLabel;

  QMenu *fileMenu;
  QMenu *optionsMenu;
  QMenu *helpMenu;

  QToolBar *fileToolBar;
  QToolBar *optionsToolBar;

  QAction *openAction;
  QAction *importAction;
  QAction *exportAction;
  QAction *closeAction;
  QAction *exitAction;
  QAction *consoleAction;
  QAction *statsAction;
  QAction *aboutAction;
  QAction *aboutQtAction;

  StatsWindow *statsWindow;
  ConsoleWindow *consoleWindow;

  FsEngine *fsEngine;
  QString filePath;
};
#endif

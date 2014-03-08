/**
 * mainwindow.h - Header of mainwindow
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
  void pressTreeItem();
  void clickListItem();
  void doubleClickListItem();

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createWidgets();
  void createConnections();
  void loadFile(QString &name);
  void setOutput(const QString &text);
  void createTreeView(const struct fs_dirent *dent);
  void createTreeRoot(const char *name, unsigned long long ino);
  void createTreeItem(unsigned long long ino);
  void insertTreeRow(const QStringList &data);
  void insertTreeChild(const QStringList &data, const QModelIndex &parent);
  void removeTreeView();
  void removeTreeColumnsAll();
  void removeTreeRowsAll();
  void showTreeItem();
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
  QString fsPath;
};
#endif

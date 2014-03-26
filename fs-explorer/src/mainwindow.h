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

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QtCore>
#include <QMap>
#include <QStyle>
#include <QDateTime>
#include <QSplitter>
#include <QHBoxLayout>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QTreeView>
#include <QTextEdit>
#include <QMainWindow>

#include "fsengine.h"
#include "fstreemodel.h"
#include "fslistmodel.h"
#include "statswindow.h"
#include "consolewindow.h"
#include "aboutdialog.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

signals:
  void mounted(bool status);
  void syncTree(QModelIndex index);
  void syncList(QModelIndex index);

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void openFile();
  void importDir();
  void exportDir();
  void closeFile();
  void stats();
  void console();
  void goHome();
  void goUp();
  void about();
  void showWidgets(bool show);

  void pressTreeItem(QModelIndex index);
  void syncTreeItem(QModelIndex index);

  void clickListItem(QModelIndex index);
  void doubleClickListItem(QModelIndex index);
  void syncListItem(QModelIndex index);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createWidgets();
  void createConnections();
  void loadFile(QString &name);
  void setOutput(const QString &text) const;

  void createFileDentList(unsigned long long ino, QList<struct fs_dirent> &list);
  void createFileStatList(QList<struct fs_dirent> &dentList, QList<struct fs_kstat> &statList);
  void createTreeRoot(const char *name, unsigned long long ino);
  void createTreeItem(unsigned long long ino, const QList<struct fs_dirent> &list);
  void createListItem(const QList<struct fs_dirent> &list);
  void updateTreeItem(unsigned long long ino);
  void updateListItem(unsigned long long ino);
  void insertTreeRow(const QStringList &data);
  void insertTreeChild(const QStringList &data, const QModelIndex &parent);
  void insertListRow(const QStringList &data);
  void removeTreeAll();
  void removeTreeColumnsAll();
  void removeTreeRowsAll();
  void removeListAll();
  void removeListColumnsAll();
  void removeListRowsAll();

  void showFileStat(unsigned long long ino) const;

  QWidget *layoutWidget;
  QHBoxLayout *hBoxLayout;
  FsTreeModel *treeModel;
  FsListModel *listModel;
  QTreeView *treeView;
  QTreeView *listView;
  QTextEdit *outputView;
  QSplitter *vertSplitter;
  QSplitter *horiSplitter;
  QLabel *bgLabel;
  QLabel *readyLabel;

  QMenu *fileMenu;
  QMenu *optionsMenu;
  QMenu *goMenu;
  QMenu *helpMenu;

  QToolBar *fileToolBar;
  QToolBar *optionsToolBar;
  QToolBar *goToolBar;

  QAction *openAction;
  QAction *importAction;
  QAction *exportAction;
  QAction *closeAction;
  QAction *exitAction;
  QAction *consoleAction;
  QAction *statsAction;
  QAction *homeAction;
  QAction *upAction;
  QAction *aboutAction;
  QAction *aboutQtAction;

  StatsWindow *statsWindow;
  ConsoleWindow *consoleWindow;

  FsEngine *fsEngine;
  QString fsPath;
  QList<struct fs_dirent> fileDentList;
  QList<struct fs_kstat> fileStatList;

  QStringList treeHeader;
  QStringList listHeader;

  QMap<QString, unsigned long long> mapTreeNameIno;
  QMap<QString, unsigned long long> mapListNameIno;

  QMap<unsigned long long, bool> mapTreeInoExpand;
  QMap<unsigned long long, libfs_ftype> mapListInoType;

  int columnWidth;
};
#endif

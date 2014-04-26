/**
 * mainwindow.h - Header of mainwindow
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
#include <QLineEdit>
#include <QTreeView>
#include <QTextEdit>
#include <QSettings>
#include <QMainWindow>

#include "fsengine.h"
#include "fstreemodel.h"
#include "fslistmodel.h"
#include "statswindow.h"
#include "consolewindow.h"
#include "aboutdialog.h"

enum TreeHeader{
  TREE_NAME = 0,
  TREE_INO,
  TREE_MAX
};

enum ListHeader{
  LIST_NAME = 0,
  LIST_SIZE,
  LIST_MTIME,
  LIST_ATIME,
  LIST_CTIME,
  LIST_INO,
  LIST_MODE,
  LIST_UID,
  LIST_GID,
  LIST_TYPE,
  LIST_MAX
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

signals:
  void mounted(bool status);
  void syncTree(unsigned long long ino);
  void syncList(unsigned long long ino);

protected:
  void resizeEvent(QResizeEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
  void closeEvent(QCloseEvent *event);

private slots:
  void openFile();
  void closeFile();
  void importFile();
  void exportFile();
  void removeFile();
  void prop();
  void stats();
  void console();
  void goHome();
  void goUp();
  void about();
  void address();
  void search();
  void showWidgets(bool show);

  void pressTreeItem(QModelIndex index);
  void syncTreeItem(unsigned long long ino);

  void clickListItem(QModelIndex index);
  void doubleClickListItem(QModelIndex index);
  void syncListItem(unsigned long long ino);
  void showContextMenu(const QPoint &pos);

private:
  void initSettings();
  void writeSettings();
  void readSettings();

  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createWidgets();
  void createConnections();
  void confirmFileStatus(bool &status);
  void confirmAddressStatus(const QString &text);
  void loadFile(QString &name);
  void setOutput(const QString &text) const;
  void showAddress(QModelIndex index) const;
  bool findAddress(QModelIndex modelIndex, const QStringList &list, int listIndex);

  void createFileDentList(unsigned long long ino, QList<struct fs_dirent> &list);
  void createFileStatList(QList<struct fs_dirent> &dentList, QList<struct fs_kstat> &statList);
  void createTreeRoot(const char *name, unsigned long long ino);
  void createTreeItem(unsigned long long ino, const QList<struct fs_dirent> &list);
  void createListItem(const QList<struct fs_dirent> &dentList, const QList<struct fs_kstat> &statList);
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

  QSettings *settings;

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
  QToolBar *searchToolBar;

  QLineEdit *addressBar;
  QLineEdit *searchBar;
  QSplitter *searchSplitter;

  QAction *openAction;
  QAction *closeAction;
  QAction *exitAction;
  QAction *importAction;
  QAction *exportAction;
  QAction *removeAction;
  QAction *consoleAction;
  QAction *propAction;
  QAction *statsAction;
  QAction *homeAction;
  QAction *upAction;
  QAction *aboutAction;
  QAction *aboutQtAction;
  QAction *addressAction;
  QAction *searchAction;

  StatsWindow *statsWindow;
  ConsoleWindow *consoleWindow;

  FsEngine *fsEngine;
  QString fsPath;
  bool fsStatus;
  QList<struct fs_dirent> fileDentList;
  QList<struct fs_kstat> fileStatList;

  QStringList treeHeader;
  QStringList listHeader;

  QMap<unsigned long long, QModelIndex> mapTreeInoIndex;
  QMap<unsigned long long, bool> mapTreeInoExpand;
};
#endif

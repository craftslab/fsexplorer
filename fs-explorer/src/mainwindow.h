/**
 * mainwindow.h - Header of mainwindow
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QtCore>
#include <QMap>
#include <QRegExp>
#include <QStyle>
#include <QDateTime>
#include <QSplitter>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QSettings>
#include <QMainWindow>

#include "fsengine.h"
#include "sparseengine.h"
#include "fstreemodel.h"
#include "fslistmodel.h"
#include "fstreeview.h"
#include "fslistview.h"
#include "searchwindow.h"
#include "consolewindow.h"
#include "statswindow.h"
#include "chartwindow.h"
#include "exportengine.h"
#include "aboutdialog.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

signals:
  void mounted(bool status);
  void mountedRw(bool status);
  void mountedOpen(bool status);
  void mountedHome(bool status);
  void mountedHistory(bool status);
  void mountedWidgets(bool status);
  void syncTreeItem(unsigned long long ino);
  void syncListItem(unsigned long long ino);
  void exportFileList(const QList<unsigned long long> &list, const QString &path);

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
  void exportFileAll();
  void removeFile();
  void console();
  void prop();
  void stats();
  void chart();
  void goHome();
  void goUp();
  void history(QAction *action);
  void about();
  void address();
  void address(const QString &name);
  void search();
  void setOutput(const QString &text) const;
  void appendOutput(const QString &text) const;
  void showWidgets(bool show);
  void showContextMenu(const QPoint &pos);
  void showProgressBar();
  void setProgressBar(int val);
  void showStatusLabel();
  void setHistoryEnabled(bool enable);
  void restoreActions();
  void deactivateActions();

  void handlePreprocFile();
  void handleExportFileList(const QList<unsigned long long> &list, const QString &path);

  void pressTreeItem(const QModelIndex &index);
  void currentTreeItem(const QModelIndex &current, const QModelIndex &previous);
  void handleSyncTreeItem(unsigned long long ino);

  void clickListItem(const QModelIndex &index);
  void doubleClickListItem(const QModelIndex &index);
  void activateListItem(const QModelIndex &index);
  void currentListItem(const QModelIndex &current, const QModelIndex &previous);
  void handleSyncListItem(unsigned long long ino);

private:
  void showWindowTitle();

  void openSettings();
  void closeSettings();
  void writeFsPathSettings();
  void readFsPathSettings();
  void clearFsPathSettings();
  void writeHistorySettings();
  void readHistorySettings();
  void clearHistorySettings();

  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createWidgets();
  void createConnections();

  QList<QAction *> getHistoryActions();
  void insertHistoryAction(const QString &name);
  void appendHistoryAction(const QString &name);
  void clearHistoryActions();
  void clearHistory();

  bool confirmFileStatus();
  void confirmAddressStatus(const QString &text);
  void loadFile(const QString &orig, const QString &preproced);
  void preprocFile(const QString &name);
  QString stripString(const QString &name);
  QStringList parseAddress(const QString &name);
  bool findTreeAddress(const QString &name, QModelIndex &index);
  bool findListFile(const QString &name, QModelIndex &index);
  void showTreeAddress(const QModelIndex &index) const;
  void showFileStat(unsigned long long ino) const;

  void createFileDentList(unsigned long long ino, QList<struct fs_dirent> &list);
  void createFileStatList(QList<struct fs_dirent> &dentList, QList<struct fs_kstat> &statList);
  void createTreeRoot(const char *name, unsigned long long ino);
  void createTreeItem(const QList<struct fs_dirent> &list);
  void createListItem(const QList<struct fs_dirent> &dentList, const QList<struct fs_kstat> &statList);
  void expandTreeItem(const QModelIndex &index);
  void expandListItem(unsigned long long ino);
  void insertTreeRow(const QStringList &data);
  void insertTreeChild(const QStringList &data, const QModelIndex &parent);
  void insertListRow(const QStringList &data);
  void removeTreeAll();
  void removeTreeColumnsAll();
  void removeTreeRowsAll();
  void removeListAll();
  void removeListColumnsAll();
  void removeListRowsAll();

  static const QString title;
  static const QString version;
  static const QString separator;
  static const QString label;

  QThread *thread;
  QSettings *settings;

  QWidget *layoutWidget;
  QHBoxLayout *hBoxLayout;
  FsTreeModel *treeModel;
  FsListModel *listModel;
  FsTreeView *treeView;
  FsListView *listView;
  QItemSelectionModel *treeItemSelectionModel;
  QItemSelectionModel *listItemSelectionModel;
  QTextEdit *outputView;
  QSplitter *vertSplitter;
  QSplitter *horiSplitter;
  QLabel *bgLabel;
  QLabel *statusLabel;
  QProgressBar *progressBar;

  QMenu *fileMenu;
  QMenu *optionsMenu;
  QMenu *goMenu;
  QMenu *historyMenu;
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
  QAction *exportAllAction;
  QAction *removeAction;
  QAction *consoleAction;
  QAction *propAction;
  QAction *statsAction;
  QAction *chartAction;
  QAction *homeAction;
  QAction *upAction;
  QAction *clearAction;
  QAction *aboutAction;
  QAction *aboutQtAction;
  QAction *addressAction;
  QAction *searchAction;

  SearchWindow *searchWindow;
  ConsoleWindow *consoleWindow;
  StatsWindow *statsWindow;
  ChartWindow *chartWindow;
  AboutDialog *aboutDialog;

  QStringList treeHeader;
  QStringList listHeader;

  SparseEngine *sparseEngine;
  ExportEngine *exportEngine;
  FsEngine *fsEngine;

  QString preprocPathOpen;
  QString fsPathOpen;
  QString fsPathExport;
  bool fsStatus;
  bool fsHome;
  QList<struct fs_dirent> treeFileDentList;
  QList<struct fs_kstat> treeFileStatList;
  QList<struct fs_dirent> listFileDentList;
  QList<struct fs_kstat> listFileStatList;
};
#endif

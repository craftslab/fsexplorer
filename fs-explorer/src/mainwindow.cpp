/**
 * mainwindow.cpp - The entry of mainwindow
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

#include "mainwindow.h"

const QString MainWindow::title = QObject::tr("Fs Explorer");
const QString MainWindow::version = QObject::tr("15.01");
const QString MainWindow::separator = QObject::tr("/");

#if 0 // DISUSED here
const QString MainWindow::label = QObject::tr("<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> <img src= :/images/label.png </img> <span style=\" font-size:30pt; font-weight:600;\">" "Fs Explorer" "</span></p>");
#else
const QString MainWindow::label = QObject::tr("<p align=\"center\"> <img src= :/images/label.png </img> </p>");
#endif

MainWindow::MainWindow()
{
  showWindowTitle();
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createWidgets();
  createConnections();
  showWidgets(false);

  openSettings();

  fsEngine = new FsEngine(this);
  fsPathOpen = fsPathOpen;
  fsPathExport = fsPathExport;
  fsStatus = false;
  fsHome = true;

  sparsePathOpen = sparsePathOpen;

  setAcceptDrops(true);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  QSize size = event->size();

  if (searchSplitter) {
    QList<int> searchList = searchSplitter->sizes();
    searchList[1] = size.width() / 4;
    searchList[0] = size.width() - searchList[1];
    searchSplitter->setSizes(searchList);
  }

  if (horiSplitter) {
    QList<int> horiList = horiSplitter->sizes();
    horiList[0] = size.width() / 5;
    horiList[1] = size.width() - horiList[0];
    horiSplitter->setSizes(horiList);
  }

  if (vertSplitter) {
    QList<int> vertList = vertSplitter->sizes();
    vertList[1] = size.height() / 3;
    vertList[0] = size.height() - vertList[1];
    vertSplitter->setSizes(vertList);
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void MainWindow::dropEvent(QDropEvent *event)
{
  QList<QUrl> list;
  QString name;
  QFileInfo info;

  if (!event->mimeData()->hasUrls()) {
    return;
  }

  /*
   * text is dropped if size == 0
   */
  list = event->mimeData()->urls();
  if (list.size() != 1) {
    return;
  }

  name = list[0].toLocalFile();
  info.setFile(name);
  if (!info.isFile()) {
    return;
  }

  if (fsStatus) {
    fsStatus = confirmFileStatus();
  }

  if (!fsStatus) {
    fsPathOpen = QDir::toNativeSeparators(name);
    loadFile(fsPathOpen);
  }

  writeFsPathSettings();
  insertHistoryAction(fsPathOpen);
  writeHistorySettings();

  event->acceptProposedAction();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (fsEngine) {
    delete fsEngine;
    fsEngine = NULL;
  }

  if (!sparsePathOpen.isEmpty()) {
    (void)QFile::remove(sparsePathOpen);
    sparsePathOpen.clear();
  }

  closeSettings();

  event->accept();
}

void MainWindow::openFile()
{
  QString filter = tr("Fs Image (*.img *.ext4 *.fat)");
  filter += tr(";;All Files (*)");

  QString file = QFileDialog::getOpenFileName(this, tr("Choose File"), fsPathOpen, filter);
  if (file.isEmpty()) {
    return;
  }

  if (fsStatus) {
    fsStatus = confirmFileStatus();
  }

  if (!fsStatus) {
    fsPathOpen = QDir::toNativeSeparators(file);
    loadFile(fsPathOpen);
  }

  writeFsPathSettings();
  insertHistoryAction(fsPathOpen);
  writeHistorySettings();
}

void MainWindow::closeFile()
{
  addressBar->clear();

  disconnect(treeItemSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
             this, SLOT(currentTreeItem(const QModelIndex &, const QModelIndex &)));

  removeTreeAll();
  removeListAll();

  fsEngine->closeFile();
  setWindowTitle(tr("%1").arg(title));

  if (!sparsePathOpen.isEmpty()) {
    (void)QFile::remove(sparsePathOpen);
    sparsePathOpen.clear();
  }

  fsStatus = false;
  emit mounted(fsStatus);
  emit mountedRw(fsStatus);
  emit mountedWidgets(fsStatus);

  fsHome = true;
  emit mountedHome(!fsHome);
}

void MainWindow::importFile()
{
  // TODO
}

void MainWindow::exportFile()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Export to..."),
                                                  fsPathExport,
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
  if (dir.isEmpty()) {
    return;
  }

  QModelIndex index = listView->selectionModel()->currentIndex();
  QList<unsigned long long> list;
  list.clear();
  list << listModel->data(index, LIST_INO, Qt::DisplayRole).toULongLong();

  fsPathExport = QDir::toNativeSeparators(dir);

  writeFsPathSettings();

  emit exportFileList(list, fsPathExport);
}

void MainWindow::exportFileAll()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Export all to..."),
                                                  fsPathExport,
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
  if (dir.isEmpty()) {
    return;
  }

  struct fs_dirent fsRoot = fsEngine->getFileRoot();
  QList<unsigned long long> list;
  list.clear();
  list << fsRoot.d_ino;

  fsPathExport = QDir::toNativeSeparators(dir);

  writeFsPathSettings();

  emit exportFileList(list, fsPathExport);
}

void MainWindow::removeFile()
{
  // TODO
}

void MainWindow::console()
{
  QString welcome = QObject::tr("Fs Console");
  QDateTime dt = QDateTime::currentDateTime();
  welcome.append(QObject::tr(" (%1)\n").arg(dt.toString(tr("yyyy-MM-dd hh:mm:ss"))));
  welcome.append(QObject::tr("Type \"help\" for more information."));

  consoleWindow = new ConsoleWindow(welcome, fsEngine, this);
  consoleWindow->show();
}

void MainWindow::prop()
{
  QModelIndex index = listView->selectionModel()->currentIndex();
  unsigned long long ino = listModel->data(index, LIST_INO, Qt::DisplayRole).toULongLong();
  QString stat = fsEngine->getFileChildsStatDetail(ino);

  statsWindow = new StatsWindow(tr("File Stats"), stat, this);
  statsWindow->show();
}

void MainWindow::stats()
{
  QString stat = fsEngine->getFileStatDetail();

  statsWindow = new StatsWindow(tr("Fs Stats"), stat, this);
  statsWindow->show();
}

void MainWindow::chart()
{
  chartWindow = new ChartWindow(tr("Fs Chart"), fsEngine, this);
  chartWindow->show();
}

void MainWindow::goHome()
{
  QModelIndex index = treeModel->index(0, 0);
  pressTreeItem(index);
}

void MainWindow::goUp()
{
  QModelIndex index = treeView->selectionModel()->currentIndex();

  if (index.isValid() && index.parent().isValid()) {
    pressTreeItem(index.parent());
  }
}

void MainWindow::history(QAction *action)
{
  if (action->isSeparator()) {
    return;
  }

  QString text = action->text();

  if (!QString::compare(text, QString(tr("C&lear History...")))) {
    clearHistory();
  } else {
    if (fsStatus) {
      fsStatus = confirmFileStatus();
    }

    if (!fsStatus) {
      fsPathOpen = text;
      loadFile(fsPathOpen);
    }

    writeFsPathSettings();
    insertHistoryAction(fsPathOpen);
    writeHistorySettings();
  }
}

void MainWindow::about()
{
  aboutDialog = new AboutDialog(version, this);
  aboutDialog->exec();
}

void MainWindow::address()
{
  QString text = addressBar->text();
  address(text);
}

void MainWindow::address(const QString &name)
{
  QModelIndex index = treeModel->index(0, 0);
  QStringList list;
  unsigned long long ino;
  int i;
  bool found = false;

  if (!index.isValid()) {
    return;
  }

  list = parseAddress(name);

  if (list.size() <= 0) {
    return;
  } else if ((list.size() == 1) && !QString::compare(separator, list[0])) {
    treeView->setCurrentIndex(index);
    ino = treeModel->data(index, TREE_INO, Qt::DisplayRole).toULongLong();
    handleSyncListItem(ino);

    if (index.parent().isValid()) {
      fsHome = false;
    } else {
      fsHome = true;
    }
    emit mountedHome(!fsHome);

    return;
  }

  for (i = 0; i < list.size(); ++i) {
    found = findTreeAddress(list[i], index);
    if (found) {
      treeView->setCurrentIndex(index);
      if (!treeModel->hasChildren(index)) {
        expandTreeItem(index);
      }
      treeView->expand(index);
    } else {
      break;
    }
  }

  ino = treeModel->data(index, TREE_INO, Qt::DisplayRole).toULongLong();
  handleSyncListItem(ino);

  if (index.parent().isValid()) {
    fsHome = false;
  } else {
    fsHome = true;
  }
  emit mountedHome(!fsHome);

  if (!found && (i == list.size() - 1)) {
    index = listModel->index(0, 0);
    found = findListFile(list[i], index);
    if (found) {
      listView->setCurrentIndex(index);
    }
  }

  if (!found) {
    confirmAddressStatus(name);
  }
}

void MainWindow::search()
{
  QString text = stripString(searchBar->text());

  if (text.isEmpty()) {
    return;
  }

  searchWindow = new SearchWindow(tr("Search Result"), text, fsEngine, this);
  searchWindow->show();
}

void MainWindow::setOutput(const QString &text) const
{
  if (outputView) {
    outputView->clear();
    outputView->setPlainText(text);
  }
}

void MainWindow::appendOutput(const QString &text) const
{
  if (outputView) {
    outputView->append(text);
  }
}

void MainWindow::showWidgets(bool show)
{
  if (!searchToolBar || !vertSplitter || !bgLabel) {
    return;
  }

  searchBar->setText(QString(tr("")));

  if (show) {
    searchToolBar->setVisible(true);
    vertSplitter->setVisible(true);
    bgLabel->setVisible(false);
  } else {
    searchToolBar->setVisible(false);
    vertSplitter->setVisible(false);
    bgLabel->setVisible(true);
  }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
  QPoint globalPos = listView->mapToGlobal(pos);

  QMenu menu;
  menu.addAction(exportAction);
  menu.addAction(removeAction);
  menu.addSeparator();
  menu.addAction(consoleAction);
  menu.addSeparator();
  menu.addAction(propAction);

  QAction *selectedItem = menu.exec(globalPos);
  if (selectedItem) {
    // Do nothing here
  }
}

void MainWindow::showProgressBar()
{
  statusBar()->removeWidget(statusLabel);
  statusBar()->addWidget(progressBar, 1);
  progressBar->show();
  statusBar()->show();
}

void MainWindow::setProgressBar(int val)
{
  progressBar->setValue(val);
}

void MainWindow::showStatusLabel()
{
  statusBar()->removeWidget(progressBar);
  statusBar()->addWidget(statusLabel, 1);
  statusBar()->show();
}

void MainWindow::restoreActions()
{
  emit mounted(fsStatus);
  emit mountedRw(!fsEngine->isReadOnly());
  emit mountedOpen(true);
  emit mountedHome(!fsHome);
}

void MainWindow::deactivateActions()
{
  emit mounted(false);
  emit mountedRw(false);
  emit mountedOpen(false);
  emit mountedHome(false);
}

void MainWindow::pressTreeItem(const QModelIndex &index)
{
  treeView->setCurrentIndex(index);
  if (!treeModel->hasChildren(index)) {
    expandTreeItem(index);
  }
  treeView->expand(index);
}

void MainWindow::currentTreeItem(const QModelIndex &current, const QModelIndex &previous)
{
  unsigned long long ino = treeModel->data(current, TREE_INO, Qt::DisplayRole).toULongLong();
  QModelIndex dummy = previous;

  dummy = dummy;

  if (current.parent().isValid()) {
    fsHome = false;
  } else {
    fsHome = true;
  }
  emit mountedHome(!fsHome);

  emit syncListItem(ino);

  showTreeAddress(current);
  showFileStat(ino);
}

void MainWindow::handleSyncTreeItem(unsigned long long ino)
{
  QModelIndex index = treeView->selectionModel()->currentIndex();
  QModelIndex parent, child;
  bool found = false;

  if (ino == treeModel->data(index, TREE_INO, Qt::DisplayRole).toULongLong()) {
    if (index.parent().isValid()) {
      fsHome = false;
    } else {
      fsHome = true;
    }
    emit mountedHome(!fsHome);

    return;
  }

  if (index.parent().isValid()
      && ino == treeModel->data(index.parent(), TREE_INO, Qt::DisplayRole).toULongLong()) {
    treeView->setCurrentIndex(index.parent());
    showTreeAddress(index.parent());

    if (index.parent().parent().isValid()) {
      fsHome = false;
    } else {
      fsHome = true;
    }
    emit mountedHome(!fsHome);

    return;
  }

  for (int i = 0; i < treeModel->rowCount(index); ++i) {
    child = treeModel->index(i, TREE_NAME, index);
    unsigned long long childIno = treeModel->data(child, TREE_INO, Qt::DisplayRole).toULongLong();

    if (childIno == ino) {
      found = true;
      break;
    }
  }

  if (found) {
    treeView->setCurrentIndex(child);
    if (!treeModel->hasChildren(child)) {
      expandTreeItem(child);
    }
    treeView->expand(child);

    showTreeAddress(child);

    if (child.parent().isValid()) {
      fsHome = false;
    } else {
      fsHome = true;
    }
    emit mountedHome(!fsHome);
  }
}

void MainWindow::clickListItem(const QModelIndex &index)
{
  unsigned long long ino = listModel->data(index, LIST_INO, Qt::DisplayRole).toULongLong();
  showFileStat(ino);
}

void MainWindow::doubleClickListItem(const QModelIndex &index)
{
  enum libfs_ftype type = static_cast<enum libfs_ftype> (listModel->data(index, LIST_TYPE, Qt::DisplayRole).toInt());
  unsigned long long ino = listModel->data(index, LIST_INO, Qt::DisplayRole).toULongLong();

  if (type == FT_DIR) {
    removeListAll();
    expandListItem(ino);

    emit syncTreeItem(ino);
  }

  showFileStat(ino);
}

void MainWindow::activateListItem(const QModelIndex &index)
{
  enum libfs_ftype type = static_cast<enum libfs_ftype> (listModel->data(index, LIST_TYPE, Qt::DisplayRole).toInt());

  if (type < FT_MAX && (type != FT_UNKNOWN && type != FT_DIR)) {
    prop();
  }

  doubleClickListItem(index);
}

void MainWindow::currentListItem(const QModelIndex &current, const QModelIndex &previous)
{
  QModelIndex dummy = previous;
  dummy = dummy;

  clickListItem(current);
}

void MainWindow::handleSyncListItem(unsigned long long ino)
{
  removeListAll();
  expandListItem(ino);
}

void MainWindow::handleExportFileList(const QList<unsigned long long> &list, const QString &path)
{
  thread = new QThread(this);

  exportEngine = new ExportEngine(list, path, fsEngine);
  exportEngine->moveToThread(thread);

  setOutput(QString(tr("start now...")));
  progressBar->setRange(0, exportEngine->count());

  connect(thread, SIGNAL(started()), this, SLOT(deactivateActions()));
  connect(thread, SIGNAL(started()), this, SLOT(showProgressBar()));
  connect(thread, SIGNAL(started()), exportEngine, SLOT(process()));
  connect(exportEngine, SIGNAL(message(const QString)), this, SLOT(appendOutput(const QString)));
  connect(exportEngine, SIGNAL(current(int)), this, SLOT(setProgressBar(int)));
  connect(exportEngine, SIGNAL(finished()), thread, SLOT(quit()));
  connect(exportEngine, SIGNAL(finished()), exportEngine, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), this, SLOT(showStatusLabel()));
  connect(thread, SIGNAL(finished()), this, SLOT(restoreActions()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

  thread->start();
}

void MainWindow::showWindowTitle()
{
  QCoreApplication::setOrganizationName(title);
  QCoreApplication::setOrganizationDomain(title);
  QCoreApplication::setApplicationName(title);

  setWindowIcon(QPixmap(":/images/icon.png"));
  setWindowTitle(tr("%1").arg(title));
}

void MainWindow::openSettings()
{
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
  settings = new QSettings(tr("settings.ini"), QSettings::IniFormat);
#elif defined(Q_OS_MAC)
  settings = new QSettings(tr("settings.plist"), QSettings::NativeFormat);
#else
  settings = new QSettings(tr("settings.ini"), QSettings::IniFormat);
#endif

  readFsPathSettings();
  readHistorySettings();
}

void MainWindow::closeSettings()
{
  clearFsPathSettings();
  writeFsPathSettings();

  clearHistorySettings();
  writeHistorySettings();

  if (settings) {
    delete settings;
    settings = NULL;
  }
}

void MainWindow::writeFsPathSettings()
{
  settings->beginGroup(tr("FsPath"));

  settings->setValue(tr("fsPathOpen"), fsPathOpen);
  settings->setValue(tr("fsPathExport"), fsPathExport);

  settings->endGroup();

  settings->sync();
}

void MainWindow::readFsPathSettings()
{
  settings->beginGroup(tr("FsPath"));

  fsPathOpen = settings->value(tr("fsPathOpen"), QString(QDir::homePath())).value<QString>();
  fsPathExport = settings->value(tr("fsPathExport"), QString(QDir::homePath())).value<QString>();

  settings->endGroup();
}

void MainWindow::clearFsPathSettings()
{
  settings->beginGroup(tr("FsPath"));
  settings->remove("");
  settings->endGroup();

  settings->sync();
}

void MainWindow::writeHistorySettings()
{
  QList<QAction *> actions = getHistoryActions();

  if (actions.size() == 0) {
    return;
  }

  settings->beginGroup(tr("History"));
  settings->beginWriteArray(tr("fsPathOpen"));

  for (int i = 0; i < actions.size(); ++i) {
    settings->setArrayIndex(i);
    settings->setValue(tr("path"), actions.at(i)->text());
  }

  settings->endArray();
  settings->endGroup();

  settings->sync();
}

void MainWindow::readHistorySettings()
{
  settings->beginGroup(tr("History"));
  int size = settings->beginReadArray(tr("fsPathOpen"));

  for (int i = 0; i < size; ++i) {
    settings->setArrayIndex(i);
    QString path = settings->value(tr("path")).toString();
    appendHistoryAction(path);
  }

  settings->endArray();
  settings->endGroup();
}

void MainWindow::clearHistorySettings()
{
  settings->beginGroup(tr("History"));
  settings->remove("");
  settings->endGroup();

  settings->sync();
}

void MainWindow::createActions()
{
  openAction = new QAction(tr("&Open File..."), this);
#if 0 // DISUSED here
  QStyle *appStyle = QApplication::style();
  openAction->setIcon(appStyle->standardIcon(QStyle::SP_DirOpenIcon));
#else
  openAction->setIcon(QIcon(":/images/open.png"));
#endif
  openAction->setShortcut(QKeySequence::Open);
  openAction->setStatusTip(tr("Open an file"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

  closeAction = new QAction(tr("&Close"), this);
  closeAction->setIcon(QIcon(":/images/close.png"));
  closeAction->setShortcut(QKeySequence::Close);
  closeAction->setStatusTip(tr("Close the file"));
  closeAction->setEnabled(false);
  connect(closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

  exitAction = new QAction(tr("E&xit"), this);
  exitAction->setShortcut(tr("Ctrl+Q"));
  exitAction->setStatusTip(tr("Exit the application"));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

  importAction = new QAction(tr("&Import File From..."), this);
  importAction->setIcon(QIcon(":/images/import.png"));
  importAction->setShortcut(QKeySequence(tr("Ctrl+I")));
  importAction->setStatusTip(tr("Import file"));
  importAction->setEnabled(false);
  connect(importAction, SIGNAL(triggered()), this, SLOT(importFile()));

  exportAction = new QAction(tr("&Export To..."), this);
  exportAction->setIcon(QIcon(":/images/export.png"));
  exportAction->setShortcut(QKeySequence(tr("Ctrl+E")));
  exportAction->setStatusTip(tr("Export file"));
  exportAction->setEnabled(false);
  connect(exportAction, SIGNAL(triggered()), this, SLOT(exportFile()));

  exportAllAction = new QAction(tr("&Export All To..."), this);
  exportAllAction->setIcon(QIcon(":/images/export.png"));
  exportAllAction->setShortcut(QKeySequence(tr("Ctrl+E")));
  exportAllAction->setStatusTip(tr("Export all"));
  exportAllAction->setEnabled(false);
  connect(exportAllAction, SIGNAL(triggered()), this, SLOT(exportFileAll()));

  connect(this, SIGNAL(exportFileList(const QList<unsigned long long> &, const QString &)), this, SLOT(handleExportFileList(const QList<unsigned long long> &, const QString &)));

  removeAction = new QAction(tr("&Remove"), this);
  removeAction->setIcon(QIcon(":/images/remove.png"));
  removeAction->setShortcut(QKeySequence(tr("Ctrl+R")));
  removeAction->setStatusTip(tr("Remove file"));
  removeAction->setEnabled(false);
  connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFile()));

  consoleAction = new QAction(tr("Co&nsole"), this);
  consoleAction->setIcon(QIcon(":/images/console.png"));
  consoleAction->setShortcut(QKeySequence(tr("Ctrl+N")));
  consoleAction->setStatusTip(tr("Run console"));
  consoleAction->setEnabled(false);
  connect(consoleAction, SIGNAL(triggered()), this, SLOT(console()));

  propAction = new QAction(tr("&Properties"), this);
  propAction->setShortcut(QKeySequence(tr("Ctrl+P")));
  propAction->setStatusTip(tr("Show properties"));
  propAction->setEnabled(false);
  connect(propAction, SIGNAL(triggered()), this, SLOT(prop()));

  statsAction = new QAction(tr("&Stats"), this);
  statsAction->setIcon(QIcon(":/images/stats.png"));
  statsAction->setShortcut(QKeySequence(tr("Ctrl+S")));
  statsAction->setStatusTip(tr("Show stats"));
  statsAction->setEnabled(false);
  connect(statsAction, SIGNAL(triggered()), this, SLOT(stats()));

  chartAction = new QAction(tr("Char&t"), this);
  chartAction->setIcon(QIcon(":/images/chart.png"));
  chartAction->setShortcut(QKeySequence(tr("Ctrl+T")));
  chartAction->setStatusTip(tr("Show chart"));
  chartAction->setEnabled(false);
  connect(chartAction, SIGNAL(triggered()), this, SLOT(chart()));

  homeAction = new QAction(tr("Ho&me"), this);
  homeAction->setIcon(QIcon(":/images/home.png"));
  homeAction->setShortcut(QKeySequence(tr("Ctrl+M")));
  homeAction->setStatusTip(tr("Go home"));
  homeAction->setEnabled(false);
  connect(homeAction, SIGNAL(triggered()), this, SLOT(goHome()));

  upAction = new QAction(tr("&Up"), this);
  upAction->setIcon(QIcon(":/images/up.png"));
  upAction->setShortcut(QKeySequence(tr("Ctrl+U")));
  upAction->setStatusTip(tr("Go up"));
  upAction->setEnabled(false);
  connect(upAction, SIGNAL(triggered()), this, SLOT(goUp()));

  clearAction = new QAction(tr("C&lear History..."), this);
  clearAction->setShortcut(QKeySequence(tr("Ctrl+L")));
  clearAction->setStatusTip(tr("Clear history"));
  clearAction->setEnabled(false);

  QString about = tr("About ");
  about.append(title);
  aboutAction = new QAction(about, this);
  aboutAction->setStatusTip(tr("Show the application's About box"));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAction = new QAction(tr("About Qt"), this);
  aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAction);
  fileMenu->addAction(closeAction);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAction);

  optionsMenu = menuBar()->addMenu(tr("&Options"));
  optionsMenu->addAction(importAction);
  optionsMenu->addAction(exportAllAction);
  optionsMenu->addAction(removeAction);
  optionsMenu->addSeparator();
  optionsMenu->addAction(consoleAction);
  optionsMenu->addSeparator();
  optionsMenu->addAction(statsAction);
  optionsMenu->addAction(chartAction);

  goMenu = menuBar()->addMenu(tr("&Go"));
  goMenu->addAction(homeAction);
  goMenu->addAction(upAction);

  historyMenu = menuBar()->addMenu(tr("&History"));
  historyMenu->addAction(clearAction);
  historyMenu->addSeparator();
  connect(historyMenu, SIGNAL(triggered(QAction *)), this, SLOT(history(QAction *)));

  helpMenu = menuBar()->addMenu(tr("Help"));
  helpMenu->addAction(aboutAction);
  helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->setFloatable(false);
  fileToolBar->setMovable(false);
  fileToolBar->setIconSize(QSize(16, 16));
  fileToolBar->addAction(openAction);
  fileToolBar->addAction(closeAction);

  optionsToolBar = addToolBar(tr("Options"));
  optionsToolBar->setFloatable(false);
  optionsToolBar->setMovable(false);
  optionsToolBar->setIconSize(QSize(16, 16));
  optionsToolBar->addAction(importAction);
  optionsToolBar->addAction(exportAllAction);
  optionsToolBar->addAction(removeAction);
  optionsToolBar->addAction(consoleAction);
  optionsToolBar->addAction(statsAction);
  optionsToolBar->addAction(chartAction);

  goToolBar = addToolBar(tr("Go"));
  goToolBar->setFloatable(false);
  goToolBar->setMovable(false);
  goToolBar->setIconSize(QSize(16, 16));
  goToolBar->addAction(homeAction);
  goToolBar->addAction(upAction);

  addToolBarBreak(Qt::TopToolBarArea);

  addressAction = new QAction(QIcon(":/images/right.png"), QString(tr("address")), this);
  connect(addressAction, SIGNAL(triggered()), this, SLOT(address()));

  addressBar = new QLineEdit(this);
  addressBar->setFrame(false);
  addressBar->addAction(addressAction, QLineEdit::TrailingPosition);
  connect(addressBar, SIGNAL(returnPressed()), this, SLOT(address()));

  searchAction = new QAction(QIcon(":/images/search.png"), QString(tr("search")), this);
  connect(searchAction, SIGNAL(triggered()), this, SLOT(search()));

  searchBar = new QLineEdit(this);
  searchBar->setPlaceholderText(QString(tr("Search with wildcards")));
  searchBar->setFrame(false);
  searchBar->addAction(searchAction, QLineEdit::TrailingPosition);
  connect(searchBar, SIGNAL(returnPressed()), this, SLOT(search()));

  searchSplitter = new QSplitter(Qt::Horizontal, this);
  searchSplitter->addWidget(addressBar);
  searchSplitter->addWidget(searchBar);
  searchSplitter->setStretchFactor(1, 1);
  searchSplitter->setHandleWidth(1);

  searchToolBar = addToolBar(tr("Search"));
  searchToolBar->setFloatable(false);
  searchToolBar->setMovable(false);
  searchToolBar->addWidget(searchSplitter);
}

void MainWindow::createStatusBar()
{
  statusLabel = new QLabel(this);
  statusBar()->addWidget(statusLabel, 1);
  statusBar()->show();

  progressBar = new QProgressBar(this);
  progressBar->setFixedHeight(statusLabel->height());
  progressBar->hide();
}

void MainWindow::createWidgets()
{
  for (int i = 0; i < TREE_MAX; ++i) {
    treeHeader << tr("");
  }
  treeHeader[TREE_NAME] = tr("Name");
  treeHeader[TREE_INO] = tr("Ino");

  treeModel = new FsTreeModel(treeHeader, this);
  treeView = new FsTreeView(treeModel, this);
  treeItemSelectionModel = treeView->selectionModel();

  connect(treeView, SIGNAL(pressed(const QModelIndex &)), this, SLOT(pressTreeItem(const QModelIndex &)));

  for (int i = 0; i < LIST_MAX; ++i) {
    listHeader << tr("");
  }
  listHeader[LIST_NAME] = tr("Name");
  listHeader[LIST_SIZE] = tr("Size");
  listHeader[LIST_MTIME] = tr("Data Modified");
  listHeader[LIST_ATIME] = tr("Data Accessed");
  listHeader[LIST_CTIME] = tr("Data Created");
  listHeader[LIST_INO] = tr("Ino");
  listHeader[LIST_MODE] = tr("Mode");
  listHeader[LIST_UID] = tr("UID");
  listHeader[LIST_GID] = tr("GID");
  listHeader[LIST_TYPE] = tr("Type");

  listModel = new FsListModel(listHeader, this);
  listView = new FsListView(listModel, this);
  listItemSelectionModel = listView->selectionModel();

  connect(listView, SIGNAL(clicked(QModelIndex)), this, SLOT(clickListItem(QModelIndex)));
  connect(listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickListItem(QModelIndex)));
  connect(listView, SIGNAL(activated(const QModelIndex &)), this, SLOT(activateListItem(const QModelIndex &)));
  connect(listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
  connect(listItemSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(currentListItem(const QModelIndex &, const QModelIndex &)));

  horiSplitter = new QSplitter(Qt::Horizontal, this);
  horiSplitter->addWidget(treeView);
  horiSplitter->addWidget(listView);
  horiSplitter->setStretchFactor(1, 1);
  horiSplitter->setHandleWidth(1);

  outputView = new QTextEdit(this);
  outputView->setReadOnly(true);
  outputView->setLineWrapMode(QTextEdit::NoWrap);

  vertSplitter = new QSplitter(Qt::Vertical, this);
  vertSplitter->setVisible(false);
  vertSplitter->addWidget(horiSplitter);
  vertSplitter->addWidget(outputView);
  vertSplitter->setStretchFactor(1, 1);
  vertSplitter->setHandleWidth(1);

  bgLabel = new QLabel(label, this);
  bgLabel->setVisible(false);
  bgLabel->setTextFormat(Qt::RichText);

  hBoxLayout = new QHBoxLayout();
  hBoxLayout->addWidget(vertSplitter);
  hBoxLayout->addWidget(bgLabel);

  layoutWidget = new QWidget(this);
  layoutWidget->setLayout(hBoxLayout);

  setCentralWidget(layoutWidget);
}

void MainWindow::createConnections()
{
  connect(this, SIGNAL(mountedOpen(bool)), openAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), closeAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mountedRw(bool)), importAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), exportAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), exportAllAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mountedRw(bool)), removeAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), consoleAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), propAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), statsAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), chartAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mountedHome(bool)), homeAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mountedHome(bool)), upAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mountedWidgets(bool)), this, SLOT(showWidgets(bool)));

  connect(this, SIGNAL(syncTreeItem(unsigned long long)), this, SLOT(handleSyncTreeItem(unsigned long long)));
  connect(this, SIGNAL(syncListItem(unsigned long long)), this, SLOT(handleSyncListItem(unsigned long long)));
}

QList<QAction *> MainWindow::getHistoryActions()
{
  QList<QAction *> actions = historyMenu->actions();
  QList<QAction *>::iterator index = actions.end();
  QList<QAction *>::iterator i;

  for (i = actions.begin(); i != actions.end(); ++i) {
    if ((*i)->isSeparator()) {
      index = i;
      break;
    }
  }

  if (index == actions.end()) {
    actions.clear();
    return actions;
  }

  for (i = actions.begin(); i <= index; ++i) {
    actions.erase(i);
  }

  return actions;
}

void MainWindow::insertHistoryAction(const QString &name)
{
  int index = -1;

  QAction *history = new QAction(name, this);
  history->setEnabled(true);

  QList<QAction *> actions = getHistoryActions();

  if (actions.size() == 0) {
    historyMenu->addAction(history);
  } else {
    for (int i = 0; i < historyMenu->actions().size(); ++i) {
      if (historyMenu->actions().at(i)->isSeparator()) {
        index = i;
        break;
      }
    }

    if (index == -1) {
      delete history;
      return;
    }

    historyMenu->insertAction(historyMenu->actions().at(index + 1), history);
  }

  clearAction->setEnabled(true);
}

void MainWindow::appendHistoryAction(const QString &name)
{
  QAction *history = new QAction(name, this);

  history->setEnabled(true);
  historyMenu->addAction(history);

  clearAction->setEnabled(true);
}

void MainWindow::clearHistoryActions()
{
  QList<QAction *> actions = getHistoryActions();

  for (int i = 0; i < actions.size(); ++i) {
    historyMenu->removeAction(actions.at(i));
  }
}

void MainWindow::clearHistory()
{
  QList<QAction *> actions = getHistoryActions();

  if (actions.size() == 0) {
    return;
  }

  QMessageBox msgBox;

  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText(QString(tr("Do you want to clear history?")));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  int ret = msgBox.exec();
  switch (ret) {
  case QMessageBox::Yes:
    clearHistoryActions();
    clearAction->setEnabled(false);
    break;

  case QMessageBox::No:
  default:
    break;
  }
}

bool MainWindow::confirmFileStatus()
{
  QMessageBox msgBox;
  bool status;

  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText(QString(tr("The fs image has been open.")));
  msgBox.setInformativeText(QString(tr("Do you want to close it?")));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  int ret = msgBox.exec();
  switch (ret) {
  case QMessageBox::Yes:
    status = false;
    closeFile();
    break;

  case QMessageBox::No:
  default:
    status = true;
    break;
  }

  return status;
}

void MainWindow::confirmAddressStatus(const QString &text)
{
  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText(tr("Invalid address found!"));
  msgBox.setInformativeText(text);
  msgBox.setStandardButtons(QMessageBox::Ok);

  int ret = msgBox.exec();
  switch (ret) {
  case QMessageBox::Ok:
    // Do nothing here
    break;

  default:
    // Do nothing here
    break;
  }
}

bool MainWindow::isSparseFile(const QString &src)
{
  return SparseEngine::isSparseFile(src);
}

bool MainWindow::unsparseFile(const QString &src, QString &dst)
{
  bool ret;

  progressBar = new QProgressBar (this);
  progressBar->setFixedHeight(statusLabel->height());
  progressBar->setRange(0, 0);

  statusBar()->removeWidget(statusLabel);
  statusBar()->addWidget(progressBar, 1);
  statusBar()->show();

  emit mounted(false);
  emit mountedRw(false);
  emit mountedOpen(false);
  emit mountedHome(false);

  ret = SparseEngine::unsparseFile(src, dst);

  emit mounted(fsStatus);
  emit mountedRw(!fsEngine->isReadOnly());
  emit mountedOpen(true);
  emit mountedHome(!fsHome);

  statusBar()->removeWidget(progressBar);
  statusBar()->addWidget(statusLabel, 1);
  statusBar()->show();

  return ret;
}

void MainWindow::loadFile(const QString &name)
{
  QString pathOpen = name;
  bool ret;

  if (isSparseFile(pathOpen)) {
    ret = unsparseFile(pathOpen, sparsePathOpen);
    if (ret && !sparsePathOpen.isEmpty()) {
      pathOpen = sparsePathOpen;
    }
  }

  ret = fsEngine->openFile(pathOpen);
  if (ret) {
    setWindowTitle(tr("%1[*] - %2 - %3").arg(title).arg(name).arg(fsEngine->getFileType()));

    addressBar->setText(separator);

    struct fs_dirent treeRoot = fsEngine->getFileRoot();

    treeFileDentList.clear();
    createFileDentList(treeRoot.d_ino, treeFileDentList);

    listFileDentList.clear();
    listFileDentList = treeFileDentList;

    treeFileStatList.clear();
    createFileStatList(treeFileDentList, treeFileStatList);

    listFileStatList.clear();
    listFileStatList = treeFileStatList;

    removeTreeAll();
    createTreeRoot(treeRoot.d_name, treeRoot.d_ino);
    createTreeItem(treeFileDentList);

    removeListAll();
    createListItem(listFileDentList, listFileStatList);

    treeView->setColumnHidden(TREE_INO, true);
    connect(treeItemSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(currentTreeItem(const QModelIndex &, const QModelIndex &)));

    QDateTime dt = QDateTime::currentDateTime();
    QString text =  QObject::tr("%1 ").arg(dt.toString(tr("yyyy-MM-dd hh:mm:ss")));
    text.append(tr("mount filesystem successfully.\n\n"));
    text.append(tr("name : %1\n").arg(name));
    text.append(tr("type : %1\n").arg(fsEngine->getFileType()));
    setOutput(text);

    fsStatus = true;
  } else {
    statusBar()->showMessage(tr("Invalid fs image!"), 2000);
    fsStatus = false;
  }

  emit mounted(fsStatus);
  emit mountedRw(!fsEngine->isReadOnly());
  emit mountedWidgets(fsStatus);

  fsHome = true;
  emit mountedHome(!fsHome);
}

QString MainWindow::stripString(const QString &name)
{
  QString str;
  int firstIndex, lastIndex;

  firstIndex = name.indexOf(QRegExp("\\S"), 0);
  lastIndex = name.lastIndexOf(QRegExp("\\S"));

  str.clear();

  if (firstIndex >= 0 && lastIndex >= 0) {
    for (int i = firstIndex; i <= lastIndex; ++i) {
      str += name[i];
    }
  }

  return str;
}

QStringList MainWindow::parseAddress(const QString &name)
{
#if 0 // DISUSED here
  QString str = stripString(name);
#else
  QString str = name;
#endif

  QStringList list;

  list.clear();

  if (str.isEmpty()) {
    return list;
  }

  if (str.size() == 1 && !QString::compare(separator, str)) {
    list << str;
    return list;
  }

  list = str.split(separator, QString::SkipEmptyParts);

#if 0 // DISUSED here
  for (int i = 0; i < list.size(); ++i) {
    list[i] = stripString(list[i]);
  }
#endif

  return list;
}

bool MainWindow::findTreeAddress(const QString &name, QModelIndex &index)
{
  QModelIndex childIndex;
  QString childName;
  bool found = false;

  for (int i = 0; i < treeModel->rowCount(index); ++i) {
    childIndex = treeModel->index(i, TREE_NAME, index);
    childName = treeModel->data(childIndex, TREE_NAME, Qt::DisplayRole).toString();

    if (!QString::compare(childName, name)) {
      index = childIndex;
      found = true;
      break;
    }
  }

  return found;
}

bool MainWindow::findListFile(const QString &name, QModelIndex &index)
{
  QModelIndex childIndex;
  QString childName;
  bool found = false;

  for (int i = 0; i < listModel->rowCount(); ++i) {
    childIndex = listModel->index(i, LIST_NAME);
    childName = listModel->data(childIndex, LIST_NAME, Qt::DisplayRole).toString();
    if (!QString::compare(childName, name)) {
      index = childIndex;
      found = true;
      break;
    }
  }

  return found;
}

void MainWindow::showTreeAddress(const QModelIndex &index) const
{
  QAbstractItemModel *model = treeView->model();
  QModelIndex parent = index;
  QVariant data;
  QStringList list;
  QString address;

  if (!parent.isValid()) {
    return;
  }

  while (parent.isValid()) {
    data = model->data(parent, Qt::DisplayRole);
    list << data.toString();

    parent = parent.parent();
  }

  address.clear();
  for (int i = list.size() - 1; i >= 0; --i) {
    if (i == list.size() - 1) {
      address.append(list[i]);
    } else {
      address.append(list[i]).append(separator);
    }
  }

  addressBar->clear();
  addressBar->setText(address);
}

void MainWindow::showFileStat(unsigned long long ino) const
{
  int i = 0;
  bool found = false;

  for (i = 0; i < listFileStatList.size(); ++i) {
   if (listFileStatList[i].ino == ino) {
     found = true;
     break;
   }
  }

  if (!found) {
    return;
  }

  QString text = QObject::tr("inode: %1\n").arg(listFileStatList[i].ino);
  text.append(tr("mode: %1\n").arg(listFileStatList[i].mode, 0, 8));
  text.append(tr("nlink: %1\n").arg(listFileStatList[i].nlink));
  text.append(tr("uid: %1\n").arg(listFileStatList[i].uid));
  text.append(tr("gid: %1\n").arg(listFileStatList[i].gid));
  text.append(tr("size: %1\n").arg(listFileStatList[i].size));
  text.append(tr("atime: sec %1 nsec %2\n").arg(listFileStatList[i].atime.tv_sec).arg(listFileStatList[i].atime.tv_nsec));
  text.append(tr("mtime: sec %1 nsec %2\n").arg(listFileStatList[i].mtime.tv_sec).arg(listFileStatList[i].mtime.tv_nsec));
  text.append(tr("ctime: sec %1 nsec %2\n").arg(listFileStatList[i].ctime.tv_sec).arg(listFileStatList[i].ctime.tv_nsec));
  text.append(tr("blksize: %1\n").arg(listFileStatList[i].blksize));
  text.append(tr("blocks: %1\n").arg(listFileStatList[i].blocks));
  setOutput(text);
}

void MainWindow::createFileDentList(unsigned long long ino, QList<struct fs_dirent> &list)
{
  unsigned int num = fsEngine->getFileChildsNum(ino);
  if (num == 0) {
    return;
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  bool ret = fsEngine->getFileChildsList(ino, childs, num);
  if (!ret) {
    goto createFileDentListExit;
  }

  for (int i = static_cast<int> (num) - 1; i >= 0; --i) {
    list << childs[i];
  }

createFileDentListExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }
}

void MainWindow::createFileStatList(QList<struct fs_dirent> &dentList, QList<struct fs_kstat> &statList)
{
  for (int i = 0; i < dentList.size(); ++i) {
    statList << fsEngine->getFileChildsStat((unsigned long long)dentList[i].d_ino);
  }
}

void MainWindow::createTreeRoot(const char *name, unsigned long long ino)
{
  QStringList stringList;

  for (int i = 0; i < TREE_MAX; ++i) {
    stringList << tr("");
  }
  stringList[TREE_NAME] = tr("%1").arg(name);
  stringList[TREE_INO] = tr("%1").arg(ino);

  insertTreeRow(stringList);

  QModelIndex index = treeModel->index(0, 0);
  treeView->setCurrentIndex(index);
}

void MainWindow::createTreeItem(const QList<struct fs_dirent> &list)
{
  QModelIndex parent = treeView->selectionModel()->currentIndex();
  struct fs_dirent dent;

  for (int i = 0; i < list.size(); ++i) {
    dent = list[i];
    if (dent.d_type != FT_DIR) {
      continue;
    }

    if (!strcmp((const char *)dent.d_name, (const char *)FS_DNAME_DOT)
        || !strcmp((const char *)dent.d_name, (const char *)FS_DNAME_DOTDOT)) {
      continue;
    }

    QStringList stringList;

    for (int j = 0; j < TREE_MAX; ++j) {
      stringList << tr("");
    }
    stringList[TREE_NAME] = tr("%1").arg(dent.d_name);
    stringList[TREE_INO] = tr("%1").arg(dent.d_ino);

    insertTreeChild(stringList, parent);
  }

  treeView->setCurrentIndex(parent);
  treeView->expand(parent);
}

void MainWindow::createListItem(const QList<struct fs_dirent> &dentList, const QList<struct fs_kstat> &statList)
{
  struct fs_dirent childDentList;
  struct fs_kstat childStatList;
  const char *str = NULL;
  int64_t size;
  QDateTime dtMtime, dtAtime, dtCtime;
  QString mtime, atime, ctime;

  if (dentList.size() != statList.size()) {
    return;
  }

  for (int i = 0; i < dentList.size(); ++i) {
    childDentList = dentList[i];
    childStatList = statList[i];

    if (childStatList.size >= 0 && childStatList.size < 1024) {
      str = "%1 B";
      size = childStatList.size;
    } else if (childStatList.size >= 1024 && childStatList.size < 1048576) {
      str = "%1 KB";
      size = childStatList.size >> 10;
    } else if (childStatList.size >= 1048576) {
      str = "%1 MB";
      size = childStatList.size >> 10;
      size >>= 10;
    } else {
      str = "%1 B";
      size = 0;
    }

    dtMtime = QDateTime::fromTime_t(static_cast<uint> (childStatList.mtime.tv_sec));
    mtime = dtMtime.toString(tr("yyyy-MM-dd hh:mm:ss"));

    dtAtime = QDateTime::fromTime_t(static_cast<uint> (childStatList.atime.tv_sec));
    atime = dtAtime.toString(tr("yyyy-MM-dd hh:mm:ss"));

    dtCtime = QDateTime::fromTime_t(static_cast<uint> (childStatList.ctime.tv_sec));
    ctime = dtCtime.toString(tr("yyyy-MM-dd hh:mm:ss"));

    QStringList stringList;

    for (int j = 0; j < LIST_MAX; ++j) {
      stringList << tr("");
    }
    stringList[LIST_NAME] = tr("%1").arg(childDentList.d_name);
    stringList[LIST_SIZE] = tr(str).arg(size);
    stringList[LIST_MTIME] = tr("%1").arg(mtime);
    stringList[LIST_ATIME] = tr("%1").arg(atime);
    stringList[LIST_CTIME] = tr("%1").arg(ctime);
    stringList[LIST_INO] = tr("%1").arg(childDentList.d_ino);
    stringList[LIST_MODE] = tr("%1").arg(childStatList.mode, 0, 8);
    stringList[LIST_UID] = tr("%1").arg(childStatList.uid);
    stringList[LIST_GID] = tr("%1").arg(childStatList.gid);
    stringList[LIST_TYPE] = tr("%1").arg(childDentList.d_type);

    insertListRow(stringList);
  }

  listView->setColumnHidden(listModel->columnCount() - 1, true);

  listView->setColumnWidth(LIST_NAME, listView->getColumnWidthMax());
  listView->setColumnWidth(LIST_SIZE, listView->getColumnWidthMin());
  listView->setColumnWidth(LIST_MTIME, listView->getColumnWidthMax());
  listView->setColumnWidth(LIST_ATIME, listView->getColumnWidthMax());
  listView->setColumnWidth(LIST_CTIME, listView->getColumnWidthMax());

  for (int column = LIST_INO; column < listModel->columnCount(); ++column) {
    listView->resizeColumnToContents(column);
  }
}

void MainWindow::expandTreeItem(const QModelIndex &index)
{
  unsigned long long ino = treeModel->data(index, TREE_INO, Qt::DisplayRole).toULongLong();

  treeFileDentList.clear();
  createFileDentList(ino, treeFileDentList);

  treeFileStatList.clear();
  createFileStatList(treeFileDentList, treeFileStatList);

  createTreeItem(treeFileDentList);
}

void MainWindow::expandListItem(unsigned long long ino)
{
  listFileDentList.clear();
  createFileDentList(ino, listFileDentList);

  listFileStatList.clear();
  createFileStatList(listFileDentList, listFileStatList);

  createListItem(listFileDentList, listFileStatList);
}

void MainWindow::insertTreeRow(const QStringList &data)
{
  QModelIndex index = treeView->selectionModel()->currentIndex();
  QModelIndex child;
  QAbstractItemModel *model = treeView->model();

  if (model->columnCount() == 0) {
    bool ret = model->insertColumns(0, treeHeader.size(), index.parent());
    if (!ret) {
      return;
    }

    for (int column = 0; column < model->columnCount(); ++column) {
      if (!model->headerData(column, Qt::Horizontal).isValid()) {
        model->setHeaderData(column, Qt::Horizontal, treeHeader[column], Qt::DisplayRole);
      }
    }
  }

  if (!model->insertRow(index.row()+1, index.parent())) {
    return;
  }

  for (int column = 0; column < model->columnCount(); ++column) {
    child = model->index(index.row()+1, column, index.parent());
    model->setData(child, QVariant(data[column]), Qt::DisplayRole);
  }
}

void MainWindow::insertTreeChild(const QStringList &data, const QModelIndex &parent)
{
  QAbstractItemModel *model = treeView->model();
  QModelIndex child;

  if (model->columnCount() == 0) {
    if (!model->insertColumn(0, parent)) {
      return;
    }
  }

  if (!model->insertRow(0, parent)) {
    return;
  }

  for (int column = 0; column < model->columnCount(); ++column) {
    child = model->index(0, column, parent);
    model->setData(child, QVariant(data[column]), Qt::DisplayRole);
    if (!model->headerData(column, Qt::Horizontal).isValid()) {
      model->setHeaderData(column, Qt::Horizontal, QVariant(tr("[No header]")), Qt::DisplayRole);
    }
  }

  treeView->selectionModel()->setCurrentIndex(model->index(0, 0, parent),
                                              QItemSelectionModel::ClearAndSelect);
}

void MainWindow::insertListRow(const QStringList &data)
{
  QModelIndex index = listView->selectionModel()->currentIndex();
  QModelIndex child;
  QAbstractItemModel *model = listView->model();

  if (model->columnCount() == 0) {
    bool ret = model->insertColumns(0, listHeader.size(), index.parent());
    if (!ret) {
      return;
    }

    for (int column = 0; column < model->columnCount(); ++column) {
      if (!model->headerData(column, Qt::Horizontal).isValid()) {
        model->setHeaderData(column, Qt::Horizontal, listHeader[column], Qt::DisplayRole);
      }
    }
  }

  if (!model->insertRow(index.row()+1, index.parent())) {
    return;
  }

  for (int column = 0; column < model->columnCount(); ++column) {
    child = model->index(index.row()+1, column, index.parent());
    model->setData(child, QVariant(data[column]), Qt::DisplayRole);
  }
}

void MainWindow::removeTreeAll()
{
  removeTreeColumnsAll();
}

void MainWindow::removeTreeColumnsAll()
{
  QModelIndex index = treeModel->index(0, 0);
  QAbstractItemModel *model = treeView->model();
  model->removeColumns(0, model->columnCount(), index);
}

void MainWindow::removeTreeRowsAll()
{
  QModelIndex index = treeModel->index(0, 0);
  QAbstractItemModel *model = treeView->model();
  model->removeRows(0, model->rowCount(), index);
}

void MainWindow::removeListAll()
{
  removeListColumnsAll();
}

void MainWindow::removeListColumnsAll()
{
  QModelIndex index = listModel->index(0, 0);
  QAbstractItemModel *model = listView->model();
  model->removeColumns(0, model->columnCount(), index);
}

void MainWindow::removeListRowsAll()
{
  QModelIndex index = listModel->index(0, 0);
  QAbstractItemModel *model = listView->model();
  model->removeRows(0, model->rowCount(), index);
}

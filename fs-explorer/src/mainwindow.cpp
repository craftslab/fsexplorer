/**
 * mainwindow.cpp - The entry of mainwindow
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

#include "mainwindow.h"

static const QString mainWindowTitle = QObject::tr("FS Explorer");
static const QString version = QObject::tr("14.04");

#if 0 // DISUSED here
static const QString bgLabelText = QObject::tr("<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> <img src= :/images/label.png </img> <span style=\" font-size:30pt; font-weight:600;\">" "FS Explorer" "</span></p>");
#else
static const QString bgLabelText = QObject::tr("<p align=\"center\"> <img src= :/images/label.png </img> </p>");
#endif

static int columnNameWidth = 224;
static int columnSizeWidth = 96;
static int columnMtimeWidth = 224;
static int columnAtimeWidth = 224;
static int columnCtimeWidth = 224;

MainWindow::MainWindow()
{
  initSettings();
  readSettings();

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createWidgets();
  createConnections();
  showWidgets(false);

  fsEngine = new FsEngine;
  fsPath = fsPath;
  fsStatus = false;
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
    confirmFileStatus(fsStatus);
  }

  if (!fsStatus) {
    fsPath = QDir::toNativeSeparators(name);
    loadFile(fsPath);
  }

  event->acceptProposedAction();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  delete fsEngine;
  fsEngine = NULL;

  event->accept();
}

void MainWindow::openFile()
{
  QString filter = tr("FS Image (*.img *.ext4 *.fat)");
  filter += tr(";;All Files (*)");

  QString name = QFileDialog::getOpenFileName(this, tr("Choose File"), fsPath, filter);
  if (name.isEmpty()) {
    return;
  }

  if (fsStatus) {
    confirmFileStatus(fsStatus);
  }

  if (!fsStatus) {
    fsPath = QDir::toNativeSeparators(name);
    writeSettings();
    loadFile(fsPath);
  }
}

void MainWindow::importFile()
{
  // TODO
}

void MainWindow::exportFile()
{
  // TODO
}

void MainWindow::closeFile()
{
  removeTreeAll();
  removeListAll();

  fsEngine->closeFile();
  setWindowTitle(tr("%1").arg(mainWindowTitle));

  fsStatus = false;

  emit mounted(fsStatus);
}

void MainWindow::prop()
{
  // TODO
}

void MainWindow::stats()
{
  QString stat = fsEngine->getFileStat();

  statsWindow = new StatsWindow(stat, this);
  statsWindow->show();
}

void MainWindow::console()
{
  consoleWindow = new ConsoleWindow(this);
  consoleWindow->show();
}

void MainWindow::goHome()
{
  QModelIndex treeIndex = treeModel->index(0, 0);
  pressTreeItem(treeIndex);
}

void MainWindow::goUp()
{
  unsigned long long ino = 0;
  bool found = false;

  for (int i = 0; i < fileDentList.size(); ++i) {
    if (fileDentList[i].d_type == FT_DIR && !strcmp(fileDentList[i].d_name, FS_DNAME_DOTDOT)) {
      ino = fileDentList[i].d_ino;
      found = true;
      break;
    }
  }

  if (!found) {
    return;
  }

  removeListAll();
  updateListItem(ino);
}

void MainWindow::about()
{
  AboutDialog *aboutDialog = new AboutDialog(version, this);
  aboutDialog->exec();
}

void MainWindow::showWidgets(bool show)
{
  if (!vertSplitter || !bgLabel) {
    return;
  }

  if (show) {
    vertSplitter->setVisible(true);
    bgLabel->setVisible(false);
  } else {
    vertSplitter->setVisible(false);
    bgLabel->setVisible(true);
  }
}

void MainWindow::pressTreeItem(QModelIndex index)
{
  QAbstractItemModel *model = treeView->model();
  QVariant data = model->data(index, Qt::DisplayRole);
  unsigned long long ino = mapTreeNameIno[data.toString()];

  if (!mapTreeInoExpand[ino]) {
    updateTreeItem(ino);
  }

  treeView->setCurrentIndex(index);
  treeView->expand(index);

  emit syncList(ino);
}

void MainWindow::syncTreeItem(unsigned long long ino)
{
  QModelIndex index = mapTreeInoIndex[ino];
  QVariant parentData = treeModel->data(index.parent(), Qt::DisplayRole);
  unsigned long long parentIno = mapTreeNameIno[parentData.toString()];

  /* 
   * Go up if 'parentIno == ino', otherwise, go down.
   */
  if (parentIno == ino) {
    treeView->setCurrentIndex(index.parent());
  } else {
    if (!mapTreeInoExpand[ino]) {
      treeView->setCurrentIndex(index);
      updateTreeItem(ino);
    }

    treeView->expandAll();
  }
}

void MainWindow::clickListItem(QModelIndex index)
{
  QAbstractItemModel *model = listView->model();
  QVariant data = model->data(index, Qt::DisplayRole);
  unsigned long long ino = mapListNameIno[data.toString()];

  showFileStat(ino);
}

void MainWindow::doubleClickListItem(QModelIndex index)
{
  QAbstractItemModel *model = listView->model();
  QVariant data = model->data(index, Qt::DisplayRole);
  unsigned long long ino = mapListNameIno[data.toString()];

  if (mapListInoType[ino] != FT_DIR) {
    return;
  }

  removeListAll();
  updateListItem(ino);

  showFileStat(ino);

  emit syncTree(ino);
}

void MainWindow::syncListItem(unsigned long long ino)
{
  removeListAll();
  updateListItem(ino);
}

void MainWindow::showContextMenu(const QPoint &pos)
{
  QPoint globalPos = listView->mapToGlobal(pos);

  QMenu menu;
  menu.addAction(exportAction);
  menu.addAction(consoleAction);
  menu.addSeparator();
  menu.addAction(propAction);

  QAction *selectedItem = menu.exec(globalPos);
  if (selectedItem) {
    // Do nothing here
  }
}

void MainWindow::initSettings()
{
  QCoreApplication::setOrganizationName(mainWindowTitle);
  QCoreApplication::setOrganizationDomain(mainWindowTitle);
  QCoreApplication::setApplicationName(mainWindowTitle);

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
  settings = new QSettings(tr("settings.ini"), QSettings::IniFormat);
#elif defined(Q_OS_MAC)
  settings = new QSettings(tr("settings.plist"), QSettings::NativeFormat);
#else
  settings = new QSettings(tr("settings.ini"), QSettings::IniFormat);
#endif /* Q_OS_LINUX */

  setWindowIcon(QPixmap(":/images/icon.png"));
  setWindowTitle(tr("%1").arg(mainWindowTitle));

  setAcceptDrops(true);
}

void MainWindow::writeSettings()
{
  settings->beginGroup(tr("MainWindow"));
  settings->setValue(tr("fsPath"), fsPath);
  settings->endGroup();

  settings->sync();
}

void MainWindow::readSettings()
{
  settings->beginGroup(tr("MainWindow"));
  fsPath = settings->value(tr("fsPath"), QString(QDir::homePath())).value<QString>();
  settings->endGroup();
}

void MainWindow::createActions()
{
  openAction = new QAction(tr("&Open file..."), this);
#if 0 // DISUSED here
  QStyle *appStyle = QApplication::style();
  openAction->setIcon(appStyle->standardIcon(QStyle::SP_DirOpenIcon));
#else
  openAction->setIcon(QIcon(":/images/open.png"));
#endif
  openAction->setShortcut(QKeySequence::Open);
  openAction->setStatusTip(tr("Open an existing file"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

  importAction = new QAction(tr("&Import from..."), this);
  importAction->setIcon(QIcon(":/images/import.png"));
  importAction->setShortcut(QKeySequence(tr("Ctrl+I")));
  importAction->setStatusTip(tr("Import file"));
  importAction->setEnabled(false);
  importAction->setVisible(false);
  connect(importAction, SIGNAL(triggered()), this, SLOT(importFile()));

  exportAction = new QAction(tr("&Export to..."), this);
  exportAction->setIcon(QIcon(":/images/export.png"));
  exportAction->setShortcut(QKeySequence(tr("Ctrl+E")));
  exportAction->setStatusTip(tr("Export file"));
  exportAction->setEnabled(true);
  connect(exportAction, SIGNAL(triggered()), this, SLOT(exportFile()));

  closeAction = new QAction(tr("&Close"), this);
  closeAction->setIcon(QIcon(":/images/close.png"));
  closeAction->setShortcut(QKeySequence::Close);
  closeAction->setStatusTip(tr("Close the file or directory"));
  closeAction->setEnabled(false);
  connect(closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

  exitAction = new QAction(tr("E&xit"), this);
  exitAction->setShortcut(tr("Ctrl+Q"));
  exitAction->setStatusTip(tr("Exit the application"));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

  propAction = new QAction(tr("&Properties"), this);
  propAction->setShortcut(QKeySequence(tr("Ctrl+P")));
  propAction->setStatusTip(tr("Show properties"));
  propAction->setEnabled(true);
  connect(propAction, SIGNAL(triggered()), this, SLOT(prop()));

  statsAction = new QAction(tr("&Stats"), this);
  statsAction->setIcon(QIcon(":/images/stats.png"));
  statsAction->setShortcut(QKeySequence(tr("Ctrl+S")));
  statsAction->setStatusTip(tr("Show stats"));
  statsAction->setEnabled(false);
  connect(statsAction, SIGNAL(triggered()), this, SLOT(stats()));

  consoleAction = new QAction(tr("Co&nsole"), this);
  consoleAction->setIcon(QIcon(":/images/console.png"));
  consoleAction->setShortcut(QKeySequence(tr("Ctrl+N")));
  consoleAction->setStatusTip(tr("Run console"));
  consoleAction->setEnabled(false);
  connect(consoleAction, SIGNAL(triggered()), this, SLOT(console()));

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

  QString about = tr("&About ");
  about.append(mainWindowTitle);
  aboutAction = new QAction(about, this);
  aboutAction->setStatusTip(tr("Show the application's About box"));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAction = new QAction(tr("About &Qt"), this);
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
  optionsMenu->addAction(statsAction);
  optionsMenu->addAction(consoleAction);

  goMenu = menuBar()->addMenu(tr("&Go"));
  goMenu->addAction(homeAction);
  goMenu->addAction(upAction);

  helpMenu = menuBar()->addMenu(tr("&Help"));
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
  optionsToolBar->addAction(statsAction);
  optionsToolBar->addAction(consoleAction);

  goToolBar = addToolBar(tr("Go"));
  goToolBar->setFloatable(false);
  goToolBar->setMovable(false);
  goToolBar->setIconSize(QSize(16, 16));
  goToolBar->addAction(homeAction);
  goToolBar->addAction(upAction);
}

void MainWindow::createStatusBar()
{
  readyLabel = new QLabel(tr(" Ready"));
  statusBar()->addWidget(readyLabel, 1);
}

void MainWindow::createWidgets()
{
  treeHeader << tr("Name");

  treeModel = new FsTreeModel(treeHeader);
  treeView = new QTreeView();
  treeView->setModel(treeModel);
  QModelIndex treeIndex = treeModel->index(0, 0);
  treeView->scrollTo(treeIndex);
  treeView->expand(treeIndex);
  treeView->setCurrentIndex(treeIndex);
  treeView->setHeaderHidden(true);
  treeView->setColumnHidden(0, false);
  for (int column = 0; column < treeModel->columnCount(); ++column) {
    treeView->resizeColumnToContents(column);
  }

  listHeader << tr("Name") << tr("Size") << tr("Data Modified") << tr("Data Accessed") << tr("Data Created")
             << tr("Ino") << tr("Mode") << tr("UID") << tr("GID")
             << tr("Type");

  listModel = new FsListModel(listHeader);
  listView = new QTreeView();
  listView->setModel(listModel);
  QModelIndex listIndex = listModel->index(0, 0);
  listView->scrollTo(listIndex);
  listView->expand(listIndex);
  listView->setCurrentIndex(listIndex);
  listView->setHeaderHidden(false);
  listView->setColumnHidden(listModel->columnCount() - 1, true);
  listView->setContextMenuPolicy(Qt::CustomContextMenu);

  /*
   * Set column width of 'Name'
   */
  listView->setColumnWidth(0, columnNameWidth);

  /*
   * Set column width of 'Size'
   */
  listView->setColumnWidth(1, columnSizeWidth);

  /*
   * Set column width of 'Data Modified'
   */
  listView->setColumnWidth(2, columnMtimeWidth);

  /*
   * Set column width of 'Data Accessed'
   */
  listView->setColumnWidth(3, columnAtimeWidth);

  /*
   * Set column width of 'Data Created'
   */
  listView->setColumnWidth(4, columnCtimeWidth);

  for (int column = 5; column < listModel->columnCount(); ++column) {
    listView->resizeColumnToContents(column);
  }

  horiSplitter = new QSplitter(Qt::Horizontal);
  horiSplitter->addWidget(treeView);
  horiSplitter->addWidget(listView);
  horiSplitter->setStretchFactor(1, 1);
  horiSplitter->setHandleWidth(1);

  QList<int> horiList = horiSplitter->sizes();
  horiList[0] = horiSplitter->widget(0)->sizeHint().width();
  horiList[0] -= horiList[0] / 4;
  horiList[1] = horiSplitter->widget(1)->sizeHint().width();
  horiSplitter->setSizes(horiList);

  outputView = new QTextEdit();
  outputView->setReadOnly(true);
  outputView->setLineWrapMode(QTextEdit::NoWrap);

  vertSplitter = new QSplitter(Qt::Vertical);
  vertSplitter->setVisible(false);
  vertSplitter->addWidget(horiSplitter);
  vertSplitter->addWidget(outputView);
  vertSplitter->setStretchFactor(1, 1);
  vertSplitter->setHandleWidth(1);

  QList<int> vertList = vertSplitter->sizes();
  vertList[0] = vertSplitter->widget(0)->sizeHint().width();
  vertList[0] += vertList[0] / 24;
  vertList[1] = vertSplitter->widget(1)->sizeHint().width();
  vertSplitter->setSizes(vertList);

  bgLabel = new QLabel(bgLabelText);
  bgLabel->setVisible(false);
  bgLabel->setTextFormat(Qt::RichText);

  hBoxLayout = new QHBoxLayout();
  hBoxLayout->addWidget(vertSplitter);
  hBoxLayout->addWidget(bgLabel);

  layoutWidget = new QWidget();
  layoutWidget->setLayout(hBoxLayout);

  setCentralWidget(layoutWidget);
}

void MainWindow::createConnections()
{
  connect(this, SIGNAL(mounted(bool)), closeAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), statsAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), consoleAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), homeAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), upAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), this, SLOT(showWidgets(bool)));

  connect(treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressTreeItem(QModelIndex)));

  connect(listView, SIGNAL(clicked(QModelIndex)), this, SLOT(clickListItem(QModelIndex)));
  connect(listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickListItem(QModelIndex)));
  connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

  connect(this, SIGNAL(syncTree(unsigned long long)), this, SLOT(syncTreeItem(unsigned long long)));
  connect(this, SIGNAL(syncList(unsigned long long)), this, SLOT(syncListItem(unsigned long long)));
}

void MainWindow::confirmFileStatus(bool &status)
{
  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText("The fs image has been open.");
  msgBox.setInformativeText("Do you want to close it?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  int ret = msgBox.exec();
  switch (ret) {
  case QMessageBox::Yes:
    status = false;
    closeFile();
    break;

  case QMessageBox::No:
    // Do nothing here
    break;

  default:
    // Do nothing here
    break;
  }
}

void MainWindow::loadFile(QString &name)
{
  bool ret = fsEngine->openFile(name);
  if (ret) {
    setWindowTitle(tr("%1[*] - %2 - %3").arg(mainWindowTitle).arg(name).arg(fsEngine->getFileType()));

    struct fs_dirent treeRoot = fsEngine->getFileRoot();

    fileDentList.clear();
    createFileDentList(treeRoot.d_ino, fileDentList);

    fileStatList.clear();
    createFileStatList(fileDentList, fileStatList);

    createTreeRoot(treeRoot.d_name, treeRoot.d_ino);
    createTreeItem(treeRoot.d_ino, fileDentList);
    createListItem(fileDentList, fileStatList);

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
}

void MainWindow::setOutput(const QString &text) const
{
  if (outputView) {
    outputView->clear();
    outputView->setPlainText(text);
  }
}

void MainWindow::createFileDentList(unsigned long long ino, QList<struct fs_dirent> &list)
{
  struct fs_dirent child;

  fsEngine->initFileChilds(ino);

  unsigned int childsNum = fsEngine->getFileChildsNum();
  if (childsNum == 0) {
    fsEngine->deinitFileChilds();
    return;
  }

  for (int i = (int)childsNum - 1; i >= 0; --i) {
    child = fsEngine->getFileChilds((unsigned int)i);
    list << child;
  }

  fsEngine->deinitFileChilds();
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
  stringList << tr("%1").arg(name);
  insertTreeRow(stringList);

  QModelIndex index = treeModel->index(0, 0);
  treeView->setCurrentIndex(index);

  mapTreeNameIno[QString(name)] = ino;
  mapTreeInoIndex[ino] = index;
  mapTreeInoExpand[ino] = false;
}

void MainWindow::createTreeItem(unsigned long long ino, const QList<struct fs_dirent> &list)
{
  QModelIndex parent = treeView->selectionModel()->currentIndex();
  QModelIndex index;
  struct fs_dirent child;

  mapTreeInoIndex[ino] = parent;
  mapTreeInoExpand[ino] = true;

  for (int i = 0; i < list.size(); ++i) {
    child = list[i];
    if (child.d_type != FT_DIR) {
      continue;
    }

    if (!strcmp((const char *)child.d_name, (const char *)FS_DNAME_DOT)
        || !strcmp((const char *)child.d_name, (const char *)FS_DNAME_DOTDOT)) {
      continue;
    }

    QStringList stringList;
    stringList << tr("%1").arg(child.d_name);

    insertTreeChild(stringList, parent);

    mapTreeNameIno[QString(child.d_name)] = child.d_ino;
    index = treeView->selectionModel()->currentIndex();
    mapTreeInoIndex[child.d_ino] = index;
    mapTreeInoExpand[child.d_ino] = false;
  }

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
    } else if (childStatList.size >= 1024){
      str = "%1 KB";
      size = childStatList.size >> 10;
    } else {
      str = "%1 B";
      size = 0;
    }

    dtMtime = QDateTime::fromTime_t((uint)childStatList.mtime.tv_sec);
    mtime = dtMtime.toString(tr("yyyy-MM-dd hh:mm:ss"));

    dtAtime = QDateTime::fromTime_t((uint)childStatList.atime.tv_sec);
    atime = dtAtime.toString(tr("yyyy-MM-dd hh:mm:ss"));

    dtCtime = QDateTime::fromTime_t((uint)childStatList.ctime.tv_sec);
    ctime = dtCtime.toString(tr("yyyy-MM-dd hh:mm:ss"));

    QStringList stringList;
    stringList << tr("%1").arg(childDentList.d_name) << tr(str).arg(size) << tr("%1").arg(mtime) << tr("%1").arg(atime) << tr("%1").arg(ctime)
               << tr("%1").arg(childDentList.d_ino) << tr("%1").arg(childStatList.mode, 0, 8) << tr("%1").arg(childStatList.uid) << tr("%1").arg(childStatList.gid)
               << tr("%1").arg(childDentList.d_type);

    insertListRow(stringList);

    mapListNameIno[QString(childDentList.d_name)] = childDentList.d_ino;
    mapListInoType[childDentList.d_ino] = childDentList.d_type;
  }

  listView->setColumnHidden(listModel->columnCount() - 1, true);

  /*
   * Set column width of 'Name'
   */
  listView->setColumnWidth(0, columnNameWidth);

  /*
   * Set column width of 'Size'
   */
  listView->setColumnWidth(1, columnSizeWidth);

  /*
   * Set column width of 'Data Modified'
   */
  listView->setColumnWidth(2, columnMtimeWidth);

  /*
   * Set column width of 'Data Accessed'
   */
  listView->setColumnWidth(3, columnAtimeWidth);

  /*
   * Set column width of 'Data Created'
   */
  listView->setColumnWidth(4, columnCtimeWidth);

  for (int column = 5; column < listModel->columnCount(); ++column) {
    listView->resizeColumnToContents(column);
  }
}

void MainWindow::updateTreeItem(unsigned long long ino)
{
  fileDentList.clear();
  createFileDentList(ino, fileDentList);

  fileStatList.clear();
  createFileStatList(fileDentList, fileStatList);

  createTreeItem(ino, fileDentList);
}

void MainWindow::updateListItem(unsigned long long ino)
{
  fileDentList.clear();
  createFileDentList(ino, fileDentList);

  fileStatList.clear();
  createFileStatList(fileDentList, fileStatList);

  createListItem(fileDentList, fileStatList);
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
      model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"), Qt::DisplayRole);
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
  mapTreeInoExpand.clear();
  mapTreeInoIndex.clear();
  mapTreeNameIno.clear();

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
  mapListNameIno.clear();
  mapListInoType.clear();

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

void MainWindow::showFileStat(unsigned long long ino) const
{
  int i = 0;
  bool found = false;

  for (i = 0; i < fileStatList.size(); ++i) {
   if (fileStatList[i].ino == ino) {
     found = true;
     break;
   }
  }

  if (!found) {
    return;
  }

  QString text = QObject::tr("inode: %1\n").arg(fileStatList[i].ino);
  text.append(tr("mode: %1\n").arg(fileStatList[i].mode, 0, 8));
  text.append(tr("nlink: %1\n").arg(fileStatList[i].nlink));
  text.append(tr("uid: %1\n").arg(fileStatList[i].uid));
  text.append(tr("gid: %1\n").arg(fileStatList[i].gid));
  text.append(tr("size: %1\n").arg(fileStatList[i].size));
  text.append(tr("atime: sec %1 nsec %2\n").arg(fileStatList[i].atime.tv_sec).arg(fileStatList[i].atime.tv_nsec));
  text.append(tr("mtime: sec %1 nsec %2\n").arg(fileStatList[i].mtime.tv_sec).arg(fileStatList[i].mtime.tv_nsec));
  text.append(tr("ctime: sec %1 nsec %2\n").arg(fileStatList[i].ctime.tv_sec).arg(fileStatList[i].ctime.tv_nsec));
  text.append(tr("blksize: %1\n").arg(fileStatList[i].blksize));
  text.append(tr("blocks: %1\n").arg(fileStatList[i].blocks));

  setOutput(text);
}

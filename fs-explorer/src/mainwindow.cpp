/**
 * mainwindow.cpp - The entry of mainwindow
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

#include "mainwindow.h"

static const QString mainWindowTitle = QObject::tr("FS Explorer");
static const QString version = QObject::tr("14.04");

#if 0
static const QString bgLabelText = QObject::tr("<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> <img src= :/images/label.png </img> <span style=\" font-size:30pt; font-weight:600;\">" "FS Explorer" "</span></p>");
#else
static const QString bgLabelText = QObject::tr("<p align=\"center\"> <img src= :/images/label.png </img> </p>");
#endif

MainWindow::MainWindow()
{
  setWindowIcon(QPixmap(":/images/icon.png"));
  setWindowTitle(tr("%1").arg(mainWindowTitle));

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createWidgets();
  createConnections();
  showWidgets(false);

  fsEngine = new FsEngine;
  fsPath = QString(QDir::homePath());
  fsStatus = false;
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
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("The fs image has been open!");
    msgBox.setInformativeText("Do you want to close it?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
      fsStatus = false;
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

  if (!fsStatus) {
    fsPath = QDir::toNativeSeparators(name);
    loadFile(fsPath);
  }
}

void MainWindow::importDir()
{
}

void MainWindow::exportDir()
{
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

void MainWindow::stats()
{
  statsWindow = new StatsWindow(this);
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

  emit syncList(index);
}

void MainWindow::syncTreeItem(QModelIndex index)
{
  index = index;
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

  emit syncTree(index);
}

void MainWindow::syncListItem(QModelIndex index)
{
  QVariant data = treeModel->data(index, Qt::DisplayRole);
  unsigned long long ino = mapTreeNameIno[data.toString()];

  removeListAll();
  updateListItem(ino);
}

void MainWindow::createActions()
{
  openAction = new QAction(tr("&Open file..."), this);
#if 0
  QStyle *appStyle = QApplication::style();
  openAction->setIcon(appStyle->standardIcon(QStyle::SP_DirOpenIcon));
#else
  openAction->setIcon(QIcon(":/images/open.png"));
#endif
  openAction->setShortcut(QKeySequence::Open);
  openAction->setStatusTip(tr("Open an existing file"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

  importAction = new QAction(tr("&Import from directory..."), this);
  importAction->setIcon(QIcon(":/images/import.png"));
  importAction->setShortcut(QKeySequence(tr("Ctrl+I")));
  importAction->setStatusTip(tr("Import an existing directory"));
  importAction->setEnabled(false);
  importAction->setVisible(false);
  connect(importAction, SIGNAL(triggered()), this, SLOT(importDir()));

  exportAction = new QAction(tr("&Export to directory"), this);
  exportAction->setIcon(QIcon(":/images/export.png"));
  exportAction->setShortcut(QKeySequence(tr("Ctrl+E")));
  exportAction->setStatusTip(tr("Export to the directory"));
  exportAction->setEnabled(false);
  exportAction->setVisible(false);
  connect(exportAction, SIGNAL(triggered()), this, SLOT(exportDir()));

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

  aboutAction = new QAction(tr("&About"), this);
  aboutAction->setStatusTip(tr("Show the application's About box"));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAction);
  fileMenu->addSeparator();
  fileMenu->addAction(importAction);
  fileMenu->addAction(exportAction);
  fileMenu->addSeparator();
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
}

void MainWindow::createToolBars()
{
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->setFloatable(false);
  fileToolBar->setMovable(false);
  fileToolBar->setIconSize(QSize(16, 16));
  fileToolBar->addAction(openAction);
  fileToolBar->addAction(importAction);
  fileToolBar->addAction(exportAction);
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

  listHeader << tr("Name") << tr("Size") << tr("Data Modified") << tr("Data Accessed")
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

  columnWidth = 192;
  listView->setColumnWidth(0, columnWidth);

  for (int column = 1; column < listModel->columnCount(); ++column) {
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
  // TODO: connect(this, SIGNAL(mounted(bool)), consoleAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), homeAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), upAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), this, SLOT(showWidgets(bool)));

  connect(treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressTreeItem(QModelIndex)));

  connect(listView, SIGNAL(clicked(QModelIndex)), this, SLOT(clickListItem(QModelIndex)));
  connect(listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickListItem(QModelIndex)));

  connect(this, SIGNAL(syncTree(QModelIndex)), this, SLOT(syncTreeItem(QModelIndex)));
  connect(this, SIGNAL(syncList(QModelIndex)), this, SLOT(syncListItem(QModelIndex)));
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
    createListItem(fileDentList);

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
  fsEngine->initFileChilds(ino);

  unsigned int childsNum = fsEngine->getFileChildsNum();
  if (childsNum == 0) {
    fsEngine->deinitFileChilds();
    return;
  }

  for (int i = (int)childsNum - 1; i >= 0; --i) {
    struct fs_dirent child = fsEngine->getFileChilds((unsigned int)i);
    list << child;
  }

  fsEngine->deinitFileChilds();
}

void MainWindow::createFileStatList(QList<struct fs_dirent> &dentList, QList<struct fs_kstat> &statList)
{
  for (int i = 0; i < dentList.size(); ++i) {
    statList << fsEngine->getFileStat((unsigned long long)dentList[i].d_ino);
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
}

void MainWindow::createTreeItem(unsigned long long ino, const QList<struct fs_dirent> &list)
{
  mapTreeInoExpand[ino] = true;

  QModelIndex index = treeView->selectionModel()->currentIndex();

  for (int i = 0; i < list.size(); ++i) {
    struct fs_dirent child = list[i];
    if (child.d_type != FT_DIR) {
      continue;
    }

    if (!strcmp((const char *)child.d_name, (const char *)FS_DNAME_DOT)
        || !strcmp((const char *)child.d_name, (const char *)FS_DNAME_DOTDOT)) {
      continue;
    }

    QStringList stringList;
    stringList << tr("%1").arg(child.d_name);
    insertTreeChild(stringList, index);

    mapTreeNameIno[QString(child.d_name)] = child.d_ino;
    mapTreeInoExpand[child.d_ino] = false;
  }

  index = treeModel->index(0, 0);
  treeView->setCurrentIndex(index);
  treeView->expand(index);
}

void MainWindow::createListItem(const QList<struct fs_dirent> &list)
{
  for (int i = 0; i < list.size(); ++i) {
    struct fs_dirent child = list[i];

    QStringList stringList;
    stringList << tr("%1").arg(child.d_name) << tr("0") << tr("0") << tr("0")
               << tr("%1").arg(child.d_type);

    insertListRow(stringList);

    mapListNameIno[QString(child.d_name)] = child.d_ino;
    mapListInoType[child.d_ino] = child.d_type;
  }

  listView->setColumnHidden(listModel->columnCount() - 1, true);
  listView->setColumnWidth(0, columnWidth);

  for (int column = 1; column < listModel->columnCount(); ++column) {
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

  createListItem(fileDentList);
}

void MainWindow::insertTreeRow(const QStringList &data)
{
  QModelIndex index = treeView->selectionModel()->currentIndex();
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
    QModelIndex child = model->index(index.row()+1, column, index.parent());
    model->setData(child, QVariant(data[column]), Qt::DisplayRole);
  }
}

void MainWindow::insertTreeChild(const QStringList &data, const QModelIndex &parent)
{
  QAbstractItemModel *model = treeView->model();

  if (model->columnCount() == 0) {
    if (!model->insertColumn(0, parent)) {
      return;
    }
  }

  if (!model->insertRow(0, parent)) {
    return;
  }

  for (int column = 0; column < model->columnCount(); ++column) {
    QModelIndex child = model->index(0, column, parent);
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
    QModelIndex child = model->index(index.row()+1, column, index.parent());
    model->setData(child, QVariant(data[column]), Qt::DisplayRole);
  }
}

void MainWindow::removeTreeAll()
{
  mapTreeInoExpand.clear();
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
  text.append(tr("mode: %1\n").arg(fileStatList[i].mode));
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

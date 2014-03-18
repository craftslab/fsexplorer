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
  fsPath = QDir::toNativeSeparators(name);

  loadFile(fsPath);
}

void MainWindow::importDir()
{
}

void MainWindow::exportDir()
{
}

void MainWindow::closeFile()
{
  removeTreeView();

  fsEngine->closeFile();
  setWindowTitle(tr("%1").arg(mainWindowTitle));

  emit mounted(false);
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

void MainWindow::about()
{
  AboutDialog *aboutDialog = new AboutDialog(tr("1.00"), this);
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

void MainWindow::pressTreeItem()
{
  showTreeItem();
}

void MainWindow::clickListItem()
{
}

void MainWindow::doubleClickListItem()
{
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
  optionsToolBar->addSeparator();
}

void MainWindow::createStatusBar()
{
  readyLabel = new QLabel(tr(" Ready"));
  statusBar()->addWidget(readyLabel, 1);
}

void MainWindow::createWidgets()
{
  QStringList treeHeader;
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

  QStringList listHeader;
  listHeader << tr("Name") << tr("Size") << tr("Type")
             << tr("Data Modified") << tr("Data Accessed")
             << tr("Permissions") << tr("Owner") << tr("Group");

  listModel = new FsListModel(listHeader);
  listView = new QTreeView();
  listView->setModel(listModel);
  QModelIndex listIndex = listModel->index(0, 0);
  listView->scrollTo(listIndex);
  listView->expand(listIndex);
  listView->setCurrentIndex(listIndex);
  listView->setHeaderHidden(false);
  listView->setColumnHidden(0, false);
  for (int column = 0; column < listModel->columnCount(); ++column) {
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
  connect(this, SIGNAL(mounted(bool)), consoleAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), statsAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), this, SLOT(showWidgets(bool)));

  connect(treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressTreeItem()));

  connect(listView, SIGNAL(clicked(QModelIndex)), this, SLOT(clickListItem()));
  connect(listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickListItem()));
}

void MainWindow::loadFile(QString &name)
{
  bool status;

  bool ret = fsEngine->openFile(name);
  if (ret) {
    setWindowTitle(tr("%1[*] - %2 - %3").arg(mainWindowTitle).arg(name).arg(fsEngine->getFileType()));

    struct fs_dirent treeRoot = fsEngine->getFileRoot();

    QList<struct fs_dirent> fileList;
    createFileList(treeRoot.d_ino, fileList);
    createTreeRoot(treeRoot.d_name, treeRoot.d_ino);
    createTreeItem(treeRoot.d_ino, fileList);
    createListItem(fileList);

    QDateTime dt = QDateTime::currentDateTime();
    QString text =  QObject::tr("%1 ").arg(dt.toString(tr("yyyy-MM-dd hh:mm:ss")));
    text.append(tr("mount filesystem successfully.\n\n"));
    text.append(tr("name: %1\n").arg(name));
    text.append(tr("type: %1\n").arg(fsEngine->getFileType()));
    setOutput(text);

    status = true;
  } else {
    statusBar()->showMessage(tr("Invalid fs image!"), 2000);
    status = false;
  }

  emit mounted(status);
}

void MainWindow::setOutput(const QString &text)
{
  if (outputView) {
    outputView->clear();
    outputView->setPlainText(text);
  }
}

void MainWindow::createFileList(unsigned long long ino, QList<struct fs_dirent> &list)
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

void MainWindow::createTreeRoot(const char *name, unsigned long long ino)
{
  QStringList stringList;
  stringList << tr("%1").arg(name);
  insertTreeRow(stringList);

  QModelIndex index = treeModel->index(0, 0);
  treeView->setCurrentIndex(index);

  mapTreeNameIno[name] = ino;
}

void MainWindow::createTreeItem(unsigned long long ino, const QList<struct fs_dirent> &list)
{
  mapTreeInoExpand[ino] = true;

  QModelIndex index = treeView->selectionModel()->currentIndex();

  for (int i = 0; i < list.size(); ++i) {
    struct fs_dirent child = list.at(i);
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

    mapTreeNameIno[child.d_name] = child.d_ino;
    mapTreeInoExpand[child.d_ino] = false;
  }

  index = treeModel->index(0, 0);
  treeView->setCurrentIndex(index);
  treeView->expand(index);
}

void MainWindow::createListItem(const QList<struct fs_dirent> &list)
{
  for (int i = 0; i < list.size(); ++i) {
    struct fs_dirent child = list.at(i);

    QStringList stringList;
    stringList << tr("%1").arg(child.d_name) << tr("0") << tr("%1").arg(child.d_type)
               << tr("0") << tr("0")
               << tr("0") << tr("0") << tr("0");
    insertListRow(stringList);
  }
}

void MainWindow::insertTreeRow(const QStringList &data)
{
  QModelIndex index = treeView->selectionModel()->currentIndex();
  QAbstractItemModel *model = treeView->model();

  if (!model->insertRow(index.row()+1, index.parent())) {
    return;
  }

  for (int column = 0; column < model->columnCount(index.parent()); ++column) {
    QModelIndex child = model->index(index.row()+1, column, index.parent());
    model->setData(child, QVariant(data[column]), Qt::DisplayRole);
  }
}

void MainWindow::insertTreeChild(const QStringList &data, const QModelIndex &parent)
{
  QAbstractItemModel *model = treeView->model();

  if (model->columnCount(parent) == 0) {
    if (!model->insertColumn(0, parent)) {
      return;
    }
  }

  if (!model->insertRow(0, parent)) {
    return;
  }

  for (int column = 0; column < model->columnCount(parent); ++column) {
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

  if (!model->insertRow(index.row()+1, index.parent())) {
    return;
  }

  for (int column = 0; column < model->columnCount(index.parent()); ++column) {
    QModelIndex child = model->index(index.row()+1, column, index.parent());
    model->setData(child, QVariant(data[column]), Qt::DisplayRole);
  }
}

void MainWindow::removeTreeView()
{
  mapTreeInoExpand.clear();
  mapTreeNameIno.clear();

  removeTreeColumnsAll();
  removeTreeRowsAll();
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

void MainWindow::showTreeItem()
{
  QAbstractItemModel *model = treeView->model();
  QModelIndex index = treeView->selectionModel()->currentIndex();
  QVariant data = model->data(index, Qt::DisplayRole);
  unsigned long long ino = mapTreeNameIno[data];

  if (!mapTreeInoExpand[ino]) {
    // TODO
  }

  treeView->setCurrentIndex(index);
  treeView->expand(index);
}

void MainWindow::updateTreeItem(int row, const QStringList &data)
{
  QModelIndex index = treeModel->index(0, 0);
  QAbstractItemModel *model = treeView->model();

  for (int column = 0; column < model->columnCount(index.parent()); ++column) {
    QModelIndex child = model->index(row, column, index.parent());
    model->setData(child, QVariant(data[column]), Qt::DisplayRole);
  }
}

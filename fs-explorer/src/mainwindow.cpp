/**
 * mainwindow.cpp - The entry of mainwindow
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

#include <QtGui>
#include <QSplitter>
#include <QTreeView>
#include <QListView>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QFileDialog>

#include "fsengine.h"
#include "mainwindow.h"

static const QString mainWindowTitle = "FS Explorer";

MainWindow::MainWindow()
{
  setWindowIcon(QPixmap(":/images/icon.png"));
  setWindowTitle(tr("%1").arg(mainWindowTitle));

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createConnections();

  treeView = NULL;
  listView = NULL;
  outputView = NULL;
  vertSplitter = NULL;
  horiSplitter = NULL;
  tabWidget = NULL;
  showWidget(false);

  fsEngine = new FsEngine;

  filePath = QString(QDir::homePath());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  closeFile();
  event->accept();
}

void MainWindow::openFile()
{
  QString filter = tr("Filesystem Image (*.img *.ext4 *.fat)");
  filter += tr(";;All Files (*)");

  QString fileName = QFileDialog::getOpenFileName(this, tr("Choose File"), filePath, filter);
  if (fileName.isEmpty()) {
    return;
  }

  fileName = QDir::toNativeSeparators(fileName);
  filePath = fileName;

  emit load(fileName);
}

void MainWindow::importDir()
{
}

void MainWindow::exportDir()
{
}

void MainWindow::closeFile()
{
  fsEngine->closeFile();
  showWidget(false);
  setWindowTitle(tr("%1").arg(mainWindowTitle));
  emit mounted(false);
}

void MainWindow::console()
{
}

void MainWindow::stats()
{
  fsEngine->dumpInfo();
}

void MainWindow::about()
{
  QMessageBox::about(this,
                    tr("FS Explorer"),
                    tr("<h3><center>Filesystem Explorer</center></h3>"
                       "<p>Copyright &copy; 2013 angersax@gmail.com</p>"));
}

void MainWindow::loadFile(QString &name)
{
  bool ret;
  bool status;

  ret = fsEngine->openFile(name);
  if (ret) {
    setWindowTitle(tr("%1[*] - %2 - %3").arg(mainWindowTitle).arg(name).arg(fsEngine->getFileType()));
    status = true;
  } else {
    statusBar()->showMessage(tr("Invalid fs image!"), 2000);
    status = false;
  }

  showWidget(status);

  emit mounted(status);
}

void MainWindow::showWidget(bool show)
{
  if (!show) {
    if (treeView) {
      delete treeView;
      treeView = NULL;
    }

    if (listView) {
      delete listView;
      listView = NULL;
    }

    if (outputView) {
      delete outputView;
      outputView = NULL;
    }

    if (vertSplitter) {
      delete vertSplitter;
      vertSplitter = NULL;
    }

    if (horiSplitter) {
      delete horiSplitter;
      horiSplitter = NULL;
    }

    if (tabWidget) {
      delete tabWidget;
      tabWidget = NULL;
    }

    return;
  }

  vertSplitter = new QSplitter(Qt::Vertical);
  horiSplitter = new QSplitter(Qt::Horizontal);
  tabWidget = new QTabWidget;
  tabWidget->setTabPosition(QTabWidget::South);

  treeView = new QTreeView();
  listView = new QListView();

  outputView = new QTextEdit();
  outputView->setReadOnly(true);
  tabWidget->addTab(outputView, tr("Output"));

#if 1 // test only
  QFileSystemModel *model = new QFileSystemModel;
  QModelIndex index = model->index(QDir::homePath());
  model->setRootPath(QDir::homePath());

  treeView->setModel(model);
  treeView->scrollTo(index);
  treeView->expand(index);
  treeView->setCurrentIndex(index);

  listView->setModel(model);
  listView->setCurrentIndex(index);
#endif

  treeView->setHeaderHidden(true);
  treeView->setColumnHidden(1, true);
  treeView->setColumnHidden(2, true);
  treeView->setColumnHidden(3, true);

  vertSplitter->addWidget(listView);
  vertSplitter->addWidget(tabWidget);
  vertSplitter->setStretchFactor(1, 1);

  QList<int> vertList = vertSplitter->sizes();
  vertList[0] = vertSplitter->widget(0)->sizeHint().width();
  vertList[0] += vertList[0] / 2;
  vertList[1] = vertSplitter->widget(1)->sizeHint().width();
  vertSplitter->setSizes(vertList);

  horiSplitter->addWidget(treeView);
  horiSplitter->addWidget(vertSplitter);
  horiSplitter->setStretchFactor(1, 1);

  QList<int> horiList = horiSplitter->sizes();
  horiList[0] = horiSplitter->widget(0)->sizeHint().width();
  horiList[0] -= horiList[0] / 4;
  horiList[1] = horiSplitter->widget(1)->sizeHint().width();
  horiSplitter->setSizes(horiList);

  setCentralWidget(horiSplitter);
}

void MainWindow::createActions()
{
  openAction = new QAction(tr("&Open file..."), this);
  openAction->setIcon(QIcon(":/images/open.png"));
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

  consoleAction = new QAction(tr("Co&nsole"), this);
  consoleAction->setIcon(QIcon(":/images/console.png"));
  consoleAction->setShortcut(QKeySequence(tr("Ctrl+N")));
  consoleAction->setStatusTip(tr("Run console"));
  consoleAction->setEnabled(false);
  connect(consoleAction, SIGNAL(triggered()), this, SLOT(console()));

  statsAction = new QAction(tr("&Stats"), this);
  statsAction->setIcon(QIcon(":/images/stats.png"));
  statsAction->setShortcut(QKeySequence(tr("Ctrl+S")));
  statsAction->setStatusTip(tr("Show stats"));
  statsAction->setEnabled(false);
  connect(statsAction, SIGNAL(triggered()), this, SLOT(stats()));

  aboutAction = new QAction(tr("&About"), this);
  aboutAction->setStatusTip(tr("Show the application's About box"));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

#if 0
  aboutQtAction = new QAction(tr("About &Qt"), this);
  aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
#endif
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
  optionsMenu->addAction(consoleAction);
  optionsMenu->addAction(statsAction);

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAction);

#if 0
  helpMenu->addAction(aboutQtAction);
#endif
}

void MainWindow::createToolBars()
{
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->setFloatable(false);
  fileToolBar->setMovable(false);
  fileToolBar->addAction(openAction);
  fileToolBar->addAction(importAction);
  fileToolBar->addAction(exportAction);
  fileToolBar->addAction(closeAction);

  optionsToolBar = addToolBar(tr("Options"));
  optionsToolBar->setFloatable(false);
  optionsToolBar->setMovable(false);
  optionsToolBar->addAction(consoleAction);
  optionsToolBar->addAction(statsAction);
  optionsToolBar->addSeparator();
}

void MainWindow::createStatusBar()
{
  readyLabel = new QLabel(tr(" Ready"));
  statusBar()->addWidget(readyLabel, 1);
}

void MainWindow::createConnections()
{
  connect(this, SIGNAL(load(QString&)), this, SLOT(loadFile(QString&)));

  connect(this, SIGNAL(mounted(bool)), closeAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), consoleAction, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(mounted(bool)), statsAction, SLOT(setEnabled(bool)));
}

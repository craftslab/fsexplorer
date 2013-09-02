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
#include <QFileSystemModel>

#include "explorer.h"
#include "mainwindow.h"

MainWindow::MainWindow()
{
  treeView = new QTreeView;

#if 1 // test only
  QFileSystemModel *model = new QFileSystemModel;
  model->setRootPath(QDir::homePath());
  treeView->setModel(model);

  QModelIndex index = model->index(QDir::homePath());
  treeView->scrollTo(index);
  treeView->expand(index);
  treeView->setCurrentIndex(index);
#endif

  treeView->setHeaderHidden(true);
  treeView->setColumnHidden(1, true);
  treeView->setColumnHidden(2, true);
  treeView->setColumnHidden(3, true);

  splitter = new QSplitter;
  splitter->addWidget(treeView);

  setCentralWidget(splitter);
  setWindowTitle(tr("Filesystem Explorer"));

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  explorer = new Explorer;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void MainWindow::openFile()
{
  explorer->openFile();
}

void MainWindow::closeFile()
{
  explorer->closeFile();
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About FS Explorer"),
                     tr("<h2>Filesystem Explorer 1.0</h2>"
                        "<p>Copyright &copy; 2013 angersax@gmail.com."));
}

void MainWindow::updateActions()
{
}

void MainWindow::createActions()
{
  openAction = new QAction(tr("&Open..."), this);
  openAction->setIcon(QIcon(":/images/open.png"));
  openAction->setShortcut(QKeySequence::Open);
  openAction->setStatusTip(tr("Open an existing file"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

  closeAction = new QAction(tr("&Close"), this);
  closeAction->setIcon(QIcon(":/images/close.png"));
  closeAction->setShortcut(QKeySequence::Close);
  closeAction->setStatusTip(tr("Close the file"));
  connect(closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

  exitAction = new QAction(tr("E&xit"), this);
  exitAction->setShortcut(tr("Ctrl+Q"));
  exitAction->setStatusTip(tr("Exit the application"));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

  aboutAction = new QAction(tr("&About"), this);
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

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAction);
  helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->addAction(openAction);
  fileToolBar->addAction(closeAction); 
}

void MainWindow::createStatusBar()
{
  readyLabel = new QLabel(tr(" Ready"));
  statusBar()->addWidget(readyLabel, 1);
}

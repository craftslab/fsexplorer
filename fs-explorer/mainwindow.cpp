#include <QtGui>
#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>

#include "explorer.h"
#include "mainwindow.h"

MainWindow::MainWindow()
{
  QFileSystemModel *model = new QFileSystemModel;
  model->setRootPath(QDir::homePath());
  QModelIndex index = model->index(QDir::homePath());

  treeView = new QTreeView;
  treeView->setModel(model);
  treeView->setColumnHidden(1, true);
  treeView->setColumnHidden(2, true);
  treeView->scrollTo(index);
  treeView->expand(index);
  treeView->setCurrentIndex(index);

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

void MainWindow::saveFile()
{
  explorer->saveFile();
}

void MainWindow::closeFile()
{
  explorer->closeFile();
}

void MainWindow::cut()
{
}

void MainWindow::copy()
{
}

void MainWindow::paste()
{
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

  saveAction = new QAction(tr("&Save"), this);
  saveAction->setIcon(QIcon(":/images/save.png"));
  saveAction->setShortcut(QKeySequence::Save);
  saveAction->setStatusTip(tr("Save the file to disk"));
  connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));

  closeAction = new QAction(tr("&Close"), this);
  //closeAction->setIcon(QIcon(":/images/close.png"));
  closeAction->setShortcut(QKeySequence::Close);
  closeAction->setStatusTip(tr("Close the file"));
  connect(closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

  exitAction = new QAction(tr("E&xit"), this);
  exitAction->setShortcut(tr("Ctrl+Q"));
  exitAction->setStatusTip(tr("Exit the application"));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

  cutAction = new QAction(tr("Cu&t"), this);
  cutAction->setIcon(QIcon(":/images/cut.png"));
  cutAction->setShortcut(QKeySequence::Cut);
  cutAction->setStatusTip(tr("Cut the current selection to the "
                             "clipboard"));
  connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

  copyAction = new QAction(tr("&Copy"), this);
  copyAction->setIcon(QIcon(":/images/copy.png"));
  copyAction->setShortcut(QKeySequence::Copy);
  copyAction->setStatusTip(tr("Copy the current selection to the "
                              "clipboard"));
  connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

  pasteAction = new QAction(tr("&Paste"), this);
  pasteAction->setIcon(QIcon(":/images/paste.png"));
  pasteAction->setShortcut(QKeySequence::Paste);
  pasteAction->setStatusTip(tr("Paste the clipboard's contents at "
                               "the cursor position"));
  connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

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
  fileMenu->addAction(saveAction);
  fileMenu->addAction(closeAction);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAction);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(cutAction);
  editMenu->addAction(copyAction);
  editMenu->addAction(pasteAction);

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAction);
  helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->addAction(openAction);
  fileToolBar->addAction(saveAction);
  fileToolBar->addAction(closeAction);
 
  editToolBar = addToolBar(tr("Edit"));
  editToolBar->addAction(cutAction);
  editToolBar->addAction(copyAction);
  editToolBar->addAction(pasteAction);
}

void MainWindow::createStatusBar()
{
  readyLabel = new QLabel(tr(" Ready"));
  statusBar()->addWidget(readyLabel, 1);
}

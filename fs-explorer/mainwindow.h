#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QToolBar;
class QSplitter;
class QTreeView;
class Explorer;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void openFile();
  void saveFile();
  void closeFile();
  void cut();
  void copy();
  void paste();
  void about();
  void updateActions();

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  QTreeView *treeView;
  QSplitter *splitter;
  QLabel *readyLabel;
  Explorer *explorer;

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *editToolBar;
  QAction *openAction;
  QAction *saveAction;
  QAction *closeAction;
  QAction *exitAction;
  QAction *cutAction;
  QAction *copyAction;
  QAction *pasteAction;
  QAction *aboutAction;
  QAction *aboutQtAction;
};
#endif

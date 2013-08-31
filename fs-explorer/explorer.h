#ifndef EXPLORER_H
#define EXPLORER_H

#include <QWidget>

class Explorer : public QWidget
{
  Q_OBJECT

public:
  Explorer(QWidget *parent = 0);

  bool openFile();
  bool saveFile();
  bool closeFile();

protected:
  void closeEvent(QCloseEvent *event);
};
#endif

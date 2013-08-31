#ifndef EXPLORER_H
#define EXPLORER_H

#include <QWidget>

class Explorer : public QWidget
{
  Q_OBJECT

public:
  Explorer(QWidget *parent = 0);

  bool saveFile();
  bool closeFile();

  static Explorer *openFile();

protected:
  void closeEvent(QCloseEvent *event);
};
#endif

#ifndef EXPLORER_H
#define EXPLORER_H

#include <QObject>

class Explorer : public QObject
{
  Q_OBJECT

public:
  Explorer(QWidget *parent = 0);

  bool openFile();
  bool closeFile();
};
#endif

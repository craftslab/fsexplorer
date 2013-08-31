#include <QtGui>

#include "explorer.h"

Explorer::Explorer(QWidget *parent)
  : QWidget(parent)
{
}

bool Explorer::openFile()
{
  return false;
}

bool Explorer::saveFile()
{
  return false;
}

bool Explorer::closeFile()
{
  return false;
}

void Explorer::closeEvent(QCloseEvent *event)
{
  event->accept();
}

#include <QtGui>

#include "explorer.h"

Explorer::Explorer(QWidget *parent)
  : QWidget(parent)
{
}

bool Explorer::saveFile()
{
  return false;
}

bool Explorer::closeFile()
{
  return false;
}

Explorer *Explorer::openFile()
{
  return NULL;
}

void Explorer::closeEvent(QCloseEvent *event)
{
  event->accept();
}

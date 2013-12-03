/**
 * main.c - Main entry of FS explorer
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

#include <QApplication>
#include <QDesktopWidget>

#include "mainwindow.h"

/*
 * Macro Definition
 */

/*
 * Type Definition
 */

/*
 * Global Variable Definition
 */

/*
 * Function Declaration
 */

/*
 * Function Definition
 */
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MainWindow mainWin;

  QDesktopWidget* desktopWidget = QApplication::desktop();
  //QRect deskRect = desktopWidget->availableGeometry();
  QRect screenRect = desktopWidget->screenGeometry();

  int width, height;
  if (((screenRect.width() % 16) == 0) && ((screenRect.height() % 10) == 0)) {
    width = 640;
    height = 400;
  } else if (((screenRect.width() % 16) == 0) && ((screenRect.height() % 9) == 0)) {
    width = 640;
    height = 360;
  } else if (((screenRect.width() % 4) == 0) && ((screenRect.height() % 3) == 0)) {
    width = 640;
    height = 480;
  } else {
    width = 640;
    height = 480;
  }

  mainWin.move((screenRect.width() - width) / 2, (screenRect.height() - height) / 2);
  mainWin.resize(width, height);
  mainWin.show();

  return app.exec();
}

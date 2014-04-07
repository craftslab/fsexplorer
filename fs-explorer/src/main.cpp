/**
 * main.c - Main entry of FS explorer
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This file is part of FS Explorer.
 *
 * FS explorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FS Explorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FS Explorer.  If not, see <http://www.gnu.org/licenses/>.
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
  QApplication::addLibraryPath(QApplication::applicationFilePath() + QDir::separator() + "lib");
  QApplication::addLibraryPath(QApplication::applicationFilePath() + QDir::separator() + "plugins");

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

  if ((screenRect.width() - width) >= 0 && ((screenRect.height() - height) >= 0)) {
    mainWin.move((screenRect.width() - width) / 2, (screenRect.height() - height) / 2);
  } else {
    mainWin.move(0, 0);
  }
  mainWin.resize(width, height);
  mainWin.show();

  app.exec();

  return 0;
}

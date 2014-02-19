/**
 * main.c - Main entry of FS explorer
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

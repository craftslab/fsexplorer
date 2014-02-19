/**
 * consolewindow.cpp - The entry of consolewindow
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

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

#include "consolethread.h"
#include "consolewindow.h"

ConsoleWindow::ConsoleWindow(QWidget *parent)
    : QWidget(parent)
{
  textEdit = new QTextEdit;
  textEdit->setReadOnly(false);
  textEdit->setLineWrapMode(QTextEdit::NoWrap);

#if 0
  textEdit->setTextColor(QColor(0, 255, 0));
  textEdit->setStyleSheet("background-color: black");
#else
  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::black);
  p.setColor(QPalette::Text, Qt::green);
  textEdit->setPalette(p);
#endif

  layout = new QVBoxLayout;
  layout->addWidget(textEdit);
  setLayout(layout);

  setWindowTitle(tr("FS Console"));
  Qt::WindowFlags flags = Qt::Window | Qt::WindowStaysOnTopHint;
  setWindowFlags(flags);
  setAttribute(Qt::WA_DeleteOnClose, true);

  int width = 640;
  int height = 480;
  QDesktopWidget *desktopWidget = QApplication::desktop();
  QRect screenRect = desktopWidget->screenGeometry();
  if ((screenRect.width() - width) >= 0 && ((screenRect.height() - height) >= 0)) {
    move((screenRect.width() - width) / 2, (screenRect.height() - height) / 2);
  } else {
    move(0, 0);
  }
  resize(width, height);

  consoleThread = NULL;
  startConsoleThread();
}

void ConsoleWindow::closeEvent(QCloseEvent *event)
{
  if (consoleThread) {
    consoleThread->quitConsole();
  }

  event->accept();
}

void ConsoleWindow::startConsoleThread()
{
  consoleThread = new ConsoleThread(this);
  connect(consoleThread, SIGNAL(finished()), consoleThread, SLOT(deleteLater()));
  consoleThread->start();
}

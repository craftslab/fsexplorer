/**
 * consolewindow.cpp - The entry of consolewindow
 *
 * Copyright (c) 2013-2014 angersax@gmail.com
 *
 * This file is part of Fs Explorer.
 *
 * Fs Explorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fs Explorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fs Explorer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "consolewindow.h"

const int ConsoleWindow::width = 640;
const int ConsoleWindow::height = 480;

const QString ConsoleWindow::prompt = QObject::tr("$ ");

ConsoleWindow::ConsoleWindow(FsEngine *engine, QWidget *parent)
  : QWidget(parent)
{
  textEdit = new QTextEdit(this);
  textEdit->setReadOnly(false);
  textEdit->setLineWrapMode(QTextEdit::NoWrap);

#if 0 // DISUSED here
  textEdit->setTextColor(QColor(0, 255, 0));
  textEdit->setStyleSheet("background-color: black");
#else
  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::black);
  p.setColor(QPalette::Text, Qt::green);
  textEdit->setPalette(p);
#endif

  textEdit->clear();
  textEdit->setPlainText(prompt);

  layout = new QVBoxLayout(this);
  layout->addWidget(textEdit);
  setLayout(layout);

  shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

  setWindowTitle(tr("Fs Console"));
  setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_DeleteOnClose, true);
  setWindowModality(Qt::WindowModal);

  QDesktopWidget *desktopWidget = QApplication::desktop();
  QRect screenRect = desktopWidget->screenGeometry();
  if ((screenRect.width() - width) >= 0 && ((screenRect.height() - height) >= 0)) {
    move((screenRect.width() - width) / 2, (screenRect.height() - height) / 2);
  } else {
    move(0, 0);
  }
  resize(width, height);

  consoleEngine = new ConsoleEngine(engine, this);
  consoleEngine->moveToThread(&consoleThread);

  connect(&consoleThread, &QThread::finished, consoleEngine, &QObject::deleteLater);
  connect(this, &ConsoleWindow::operate, consoleEngine, &ConsoleEngine::doWork);
  connect(consoleEngine, &ConsoleEngine::resultReady, this, &ConsoleWindow::handleResults);

  consoleThread.start();
}

ConsoleWindow::~ConsoleWindow()
{
  consoleThread.quit();
  consoleThread.wait();
}

void ConsoleWindow::handleResults(const QStringList &list)
{
  QString text;

  for (int i = 0; i < list.size(); ++i) {
    text.append(list[i]);
  }
  text.append(tr("\n"));
  text.append(prompt);

  textEdit->setPlainText(text);
}

void ConsoleWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void ConsoleWindow::keyPressEvent(QKeyEvent *event)
{
  qDebug() << __func__ << event->key();
}

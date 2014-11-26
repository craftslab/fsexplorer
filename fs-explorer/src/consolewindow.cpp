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

const QString ConsoleWindow::prompt = QObject::tr("console $ ");

ConsoleWindow::ConsoleWindow(const QString &welcome, FsEngine *engine, QWidget *parent)
  : QConsole(parent, welcome)
{
  /*
   * Set QConsole property
   */
  setFont(QFont("Helvetica", 10, true));
  setCurrentFont(QFont("Helvetica", 8, false));
  setCmdColor(Qt::green);
  setOutColor(Qt::green);
  setErrColor(Qt::green);
  setCompletionColor(Qt::green);
  setPrompt(prompt);

  setReadOnly(false);
  setLineWrapMode(QTextEdit::NoWrap);
  setWindowTitle(tr("Fs Console"));
  setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_DeleteOnClose, true);
  setWindowModality(Qt::WindowModal);

#if 0 // DISUSED here
  setTextColor(QColor(0, 255, 0));
  setStyleSheet("background-color: black");
#else
  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::black);
  p.setColor(QPalette::Text, Qt::green);
  setPalette(p);
#endif

  QDesktopWidget *desktopWidget = QApplication::desktop();
  QRect screenRect = desktopWidget->screenGeometry();
  if ((screenRect.width() - width) >= 0 && ((screenRect.height() - height) >= 0)) {
    move((screenRect.width() - width) / 2, (screenRect.height() - height) / 2);
  } else {
    move(0, 0);
  }
  resize(width, height);

  shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

  consoleEngine = new ConsoleEngine(engine, this);
  connect(this, SIGNAL(closeConsole()), this, SLOT(close()));

  connect(this, SIGNAL(commandExecuted(const QString &)), SLOT(handleCommandExecuted(const QString &)));
}

ConsoleWindow::~ConsoleWindow()
{
  // Do nothing here
}

void ConsoleWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

QString ConsoleWindow::interpretCommand(const QString &command, int *res)
{
  QStringList list;
  QString cmd;
  QStringList args;
  QString result;

  if (command.isEmpty() || !res) {
    return result;
  }

  list = command.split(QRegExp("\\s+"));
  cmd = list[0];

  if (list.size() > 1) {
    for (int i = 1; i < list.size(); ++i) {
      args << list[i];
    }
  }

  if (cmd.compare(tr("exit")) == 0) {
    emit closeConsole();
  } else {
    QStringList ret = consoleEngine->run(cmd, args);

    for (int i = 0; i < ret.size(); ++i) {
      result.append(ret[i]);
    }
  }

  *res = 0;
  QConsole::interpretCommand(command, res);

  return result;
}

QStringList ConsoleWindow::suggestCommand(const QString &/*cmd*/, QString &/*prefix*/)
{
  QStringList list;

  // TODO

  return list;
}

void ConsoleWindow::handleCommandExecuted(const QString &/*command*/)
{
  // TODO
}

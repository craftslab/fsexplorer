/**
 * consolethread.h - Header of consolethread
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

#ifndef CONSOLETHREAD_H
#define CONSOLETHREAD_H

#include <QThread>
#include <QMutex>
#include <QtGui>

class ConsoleThread : public QThread
{
  Q_OBJECT

public:
  ConsoleThread(QObject *parent = 0);
  ~ConsoleThread();

public slots:
  void quitConsole();

protected:
  void run();

private:
  bool quit;
  QMutex mutex;
};
#endif

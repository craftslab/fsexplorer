/**
 * consolethread.h - Header of consolethread
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

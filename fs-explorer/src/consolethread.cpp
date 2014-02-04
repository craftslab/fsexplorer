/**
 * consolethread.cpp - The entry of consolethread
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

#include <QThread>
#include <QtGui>

#include "consolethread.h"

ConsoleThread::ConsoleThread(QObject *parent)
    : QThread(parent)
{
  quit = false;
}

ConsoleThread::~ConsoleThread()
{
  mutex.lock();
  quit = true;
  mutex.unlock();
  wait();
}

void ConsoleThread::quitConsole()
{
  mutex.lock();
  quit = true;
  mutex.unlock();
}

void ConsoleThread::run()
{
  while (!quit) {
    // TODO
    quit = quit;
  }
}

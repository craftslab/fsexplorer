/**
 * consoleengine.cpp - The entry of consoleengine
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

#include "consoleengine.h"

ConsoleEngine::ConsoleEngine(FsEngine *engine, QObject *parent)
{
  fsEngine = engine;
  parent = parent;
}

ConsoleEngine::~ConsoleEngine()
{
  // Do nothing here
}

QString ConsoleEngine::run(const QString &cmd)
{
  QString result;

  if (cmd.compare(tr("help")) == 0) {
    return handleHelp();
  } else {
    return handleInvalid(cmd);
  }

  return result;
}

QString ConsoleEngine::handleHelp()
{
  return tr("\
Available commands:\n\
cd - change directory\n\
exit - exit from console\n\
help - show help information\n\
ls - list directory contents\n\
pwd - print name of working directory\n\
stat - show inode information\n\
statfs - show filesystem information\n\
");
}

QString ConsoleEngine::handleInvalid(const QString &cmd)
{
  return tr("\
%1: command not found\n\
Type \"help\" for more informatation.\
").arg(cmd);
}

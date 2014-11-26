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

  curDent = fsEngine->getFileRoot();
}

ConsoleEngine::~ConsoleEngine()
{
  // Do nothing here
}

QStringList ConsoleEngine::run(const QString &cmd, const QStringList &args)
{
  QStringList result;

  if (cmd.compare(tr("help")) == 0) {
    return handleHelp();
  } else if (cmd.compare(tr("ls")) == 0) {
    return handleList(args);
  } else if (cmd.compare(tr("pwd")) == 0) {
    return handlePrintCurDir(args);
  } else {
    return handleInvalid(cmd);
  }

  return result;
}

QStringList ConsoleEngine::handleHelp()
{
  return QStringList(tr("\
Available commands:\n\
cd - change directory\n\
exit - exit from console\n\
help - show help information\n\
ls - list directory contents\n\
pwd - print name of working directory\n\
stat - show inode information\n\
statfs - show filesystem information\
"));
}

QStringList ConsoleEngine::handleList(const QStringList &args)
{
  struct fs_dirent *childs = NULL, *subChilds = NULL;
  QStringList list;
  unsigned int i, j;

  if (args.size() > 1) {
    list << args << tr(": invalid option");
    return list;    
  }

  unsigned long long ino = static_cast<unsigned long long> (curDent.d_ino);

  unsigned int num = fsEngine->getFileChildsNum(ino);
  if (num == 0) {
    return list;
  }

  childs = new fs_dirent[num];
  if (!childs) {
    return list;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  bool ret = fsEngine->getFileChildsList(ino, childs, num);
  if (!ret) {
    goto handleListExit;
  }

  if (args.size() == 0) {
    for (i = 0; i < (int)num; ++i) {
      list << tr(childs[i].d_name);
    }
  } else {
    for (i = 0; i < num; ++i) {
      if (args[0].compare(tr(childs[i].d_name)) == 0) {
	ino = static_cast<unsigned long long> (childs[i].d_ino);

	num = fsEngine->getFileChildsNum(ino);
	if (num == 0) {
	  goto handleListExit;
	}

	subChilds = new fs_dirent[num];
	if (!subChilds) {
	  goto handleListExit;
	}
	memset((void *)subChilds, 0, sizeof(struct fs_dirent) * num);

	ret = fsEngine->getFileChildsList(ino, subChilds, num);
	if (!ret) {
	  goto handleListExit;
	}

	for (j = 0; j < num; ++j) {
	  list << tr(subChilds[j].d_name);
	}

	break;
      }
    }

    if (i == num) {
      list << args << tr(": invalid option");
    }
  }

handleListExit:

  if (subChilds) {
    delete[] subChilds;
    subChilds = NULL;
  }

  if (childs) {
    delete[] childs;
    childs = NULL;
  }

  return list;
}

QStringList ConsoleEngine::handlePrintCurDir(const QStringList &args)
{
  QStringList list;

  if (args.size() == 0) {
    list = QStringList(tr(curDent.d_name));
  } else {
    list << args << tr(": invalid option");
  }

  return list;
}

QStringList ConsoleEngine::handleInvalid(const QString &cmd)
{
  return QStringList(tr("\
%1: command not found\n\
Type \"help\" for more informatation.\
").arg(cmd));
}

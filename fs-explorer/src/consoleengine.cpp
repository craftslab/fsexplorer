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
  curPath = tr(curDent.d_name);
}

ConsoleEngine::~ConsoleEngine()
{
  // Do nothing here
}

QStringList ConsoleEngine::run(const QString &cmd, const QStringList &args)
{
  if (cmd.compare(tr("cd")) == 0) {
    return handleChangeDir(args);
  } else if (cmd.compare(tr("help")) == 0) {
    return handleHelp();
  } else if (cmd.compare(tr("ls")) == 0) {
    return handleList(args);
  } else if (cmd.compare(tr("pwd")) == 0) {
    return handlePrintCurDir(args);
  } else if (cmd.compare(tr("stat")) == 0) {
    return handleStat(args);
  } else if (cmd.compare(tr("statfs")) == 0) {
    return handleStatFs(args);
  } else {
    return handleInvalid(cmd);
  }

  return QStringList(tr(""));
}

bool ConsoleEngine::traversePath(unsigned long long curIno, QStringList path, unsigned long long &foundIno)
{
  bool ret;

  if (path.size() == 0) {
    return true;
  }

  if (path.join(QString(tr(""))).size() == 0) {
    return true;
  }

  unsigned int num = fsEngine->getFileChildsNum(curIno);
  if (num == 0) {
    return true;
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return false;
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  ret = fsEngine->getFileChildsList(curIno, childs, num);
  if (!ret) {
    goto traversePathExit;
  }

  ret = false;

  for (unsigned int i = 0; i < num; ++i) {
    if (path[0].compare(tr(childs[i].d_name)) == 0) {
      foundIno = static_cast<unsigned long long> (childs[i].d_ino);
      ret = true;
      break;
    }
  }

  if (ret) {
    curIno = foundIno;
    path.removeFirst();
    ret = traversePath(curIno, path, foundIno);
  }

traversePathExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }

  return ret;
}

QStringList ConsoleEngine::handleChangeDir(const QStringList &args)
{
  unsigned long long ino;
  bool ret;

  if (args.size() > 1) {
    return QStringList(args.join(QString(tr(""))) + tr("\
: invalid args\n\
Usage: cd [DIRECTORY]\
"));
  }

  if (args.size() == 0) {
    curDent = fsEngine->getFileRoot();
    curPath = tr(curDent.d_name);
    return QStringList(curPath);
  }

  struct fs_dirent rootDent = fsEngine->getFileRoot();
  QStringList pathList = args[0].split(QString(tr("/")));

  if (args[0].startsWith(QString(tr(rootDent.d_name)))) {
    pathList[0] = tr(".");
    ret = traversePath(rootDent.d_ino, pathList, ino);
  } else {
    ret = traversePath(curDent.d_ino, pathList, ino);
  }

  if (!ret) {
    return QStringList(args[0] + tr("\
: invalid args\n\
Usage: cd [DIRECTORY]\
"));
  }

  curDent = fsEngine->getFileChildsDent(ino);

  if (args[0].startsWith(QString(tr(rootDent.d_name)))) {
    curPath = args[0];
  } else {
    if (curPath.size() == 1) {
      curPath.append(args[0]);
    } else {
      curPath.append(tr("/")).append(args[0]);
    }
  }

  // TODO

  return QStringList(curPath);
}

QStringList ConsoleEngine::handleHelp()
{
  return QStringList(tr("\
Available commands:\n\
cd - change directory\n\
clear - clear the screen\n\
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
  unsigned long long ino;
  QStringList list;
  bool ret;

  if (args.size() > 1) {
    return QStringList(args.join(QString(tr(""))) + tr("\
: invalid args\n\
Usage: ls [FILE]\
"));
  }

  if (args.size() == 0) {
    ino = curDent.d_ino;
  } else {
    struct fs_dirent rootDent = fsEngine->getFileRoot();
    QStringList pathList = args[0].split(QString(tr("/")));
    ino = 0;

    if (args[0].startsWith(QString(tr(rootDent.d_name)))) {
      pathList[0] = tr(".");
      ret = traversePath(rootDent.d_ino, pathList, ino);
    } else {
      ret = traversePath(curDent.d_ino, pathList, ino);
    }

    if (!ret) {
      return QStringList(args[0] + tr("\
: invalid args\n\
Usage: ls [FILE]\
"));
    }
  }

  unsigned int num = fsEngine->getFileChildsNum(ino);
  if (num == 0) {
    return QStringList(tr(""));
  }

  struct fs_dirent *childs = new fs_dirent[num];
  if (!childs) {
    return QStringList(tr(""));
  }
  memset((void *)childs, 0, sizeof(struct fs_dirent) * num);

  ret = fsEngine->getFileChildsList(ino, childs, num);
  if (!ret) {
    goto handleListExit;
  }

  for (unsigned int i = 0; i < num; ++i) {
    list << tr(childs[i].d_name);
  }

handleListExit:

  if (childs) {
    delete[] childs;
    childs = NULL;
  }

  return list;
}

QStringList ConsoleEngine::handlePrintCurDir(const QStringList &args)
{
  if (args.size() != 0) {
    return QStringList(args.join(QString(tr(""))) + tr("\
: invalid args\n\
Usage: pwd\
"));
  }

  return QStringList(curPath);
}

QStringList ConsoleEngine::handleStat(const QStringList &args)
{
  unsigned long long ino;
  bool ret;

  if (args.size() != 1) {
    return QStringList(args.join(QString(tr(""))) + tr("\
: invalid args\n\
Usage: stat FILE\
"));
  }

  struct fs_dirent rootDent = fsEngine->getFileRoot();
  QStringList pathList = args[0].split(QString(tr("/")));

  if (args[0].startsWith(QString(tr(rootDent.d_name)))) {
    pathList[0] = tr(".");
    ret = traversePath(rootDent.d_ino, pathList, ino);
  } else {
    ret = traversePath(curDent.d_ino, pathList, ino);
  }

  if (!ret) {
    return QStringList(args[0] + tr("\
: invalid args\n\
Usage: stat FILE\
"));
  }

  return QStringList(fsEngine->getFileChildsStatDetail(ino));
}

QStringList ConsoleEngine::handleStatFs(const QStringList &args)
{
  if (args.size() != 0) {
    return QStringList(args.join(QString(tr(""))) + tr("\
: invalid args\n\
Usage: statfs\
"));
  }

  return QStringList(fsEngine->getFileStatDetail());
}

QStringList ConsoleEngine::handleInvalid(const QString &cmd)
{
  return QStringList(tr("\
%1: command not found\n\
Type \"help\" for more informatation.\
").arg(cmd));
}

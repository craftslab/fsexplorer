/**
 * consoleengine.h - Header of consoleengine
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

#ifndef CONSOLEENGINE_H
#define CONSOLEENGINE_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QObject>
#include <QLibrary>
#include <QMutex>

#include "fsengine.h"

class ConsoleEngine : public QObject
{
  Q_OBJECT

public:
  ConsoleEngine(FsEngine *engine, QObject *parent = 0);
  ~ConsoleEngine();

  QStringList run(const QString &cmd, const QStringList &args);

private:
  QStringList handleHelp();
  QStringList handleList(const QStringList &args);
  QStringList handlePrintCurDir(const QStringList &args);
  QStringList handleInvalid(const QString &cmd);

  FsEngine *fsEngine;
  struct fs_dirent curDent;
};
#endif

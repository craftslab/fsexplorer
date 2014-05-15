/**
 * exportdialog.h - Header of exportdialog
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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QString>
#include <QWidget>
#include <QProgressDialog>

#include "fsengine.h"

class ExportDialog : public QProgressDialog
{
  Q_OBJECT

public:
  ExportDialog(const QString &title, unsigned long long ino, FsEngine *engine, const QString &name, QWidget *parent = 0);
};
#endif

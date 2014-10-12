/**
 * fstreeview.h - Header of fstreeview
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

#ifndef FSTREEVIEW_H
#define FSTREEVIEW_H

#include <QModelIndex>
#include <QVariant>
#include <QTreeView>

#include "fstreemodel.h"

enum FsTreeHeader {
  TREE_NAME = 0,
  TREE_INO,
  TREE_MAX
};

class FsTreeView : public QTreeView
{
  Q_OBJECT

public:
  FsTreeView(FsTreeModel *model, QWidget *parent = 0);
  ~FsTreeView();
};
#endif
